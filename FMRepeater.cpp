/*
 *   Copyright (C) 2009-2015,2018 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "ExternalController.h"
#include "ArduinoController.h"
#include "FMRepeater.h"
#include "DummyController.h"
#include "UDRCController.h"
#include "CWKeyer.h"
#include "Version.h"
#include "Config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pwd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
const char* DEFAULT_INI_FILE = "FMRepeater.ini";
#else
const char* DEFAULT_INI_FILE = "/etc/FMRepeater.ini";
#endif

int main(int argc, char** argv)
{
	const char* iniFile = DEFAULT_INI_FILE;
	if (argc > 1) {
		for (int currentArg = 1; currentArg < argc; ++currentArg) {
			std::string arg = argv[currentArg];
			if ((arg == "-v") || (arg == "--version")) {
				::printf("FMRepeater version %s\n", VERSION);
				return 0;
			} else if (arg.substr(0, 1) == "-") {
				::fprintf(stderr, "Usage: FMRepeater [-v|--version] [filename]\n");
				return 1;
			} else {
				iniFile = argv[currentArg];
			}
		}
	}

	CFMRepeater* repeater = new CFMRepeater(std::string(iniFile));
	repeater->run();
	delete repeater;

	return 0;
}

CFMRepeater::CFMRepeater(const std::string& filename) :
m_filename(filename),
m_thread(NULL)
{
}

CFMRepeater::~CFMRepeater()
{
}

bool CFMRepeater::run()
{
	::printf("Starting the FMRepeater - %s\n", VERSION);

	CConfig config(m_filename);

	m_thread = new CFMRepeaterThread;

	std::string callsign, beacon;
	unsigned int speed, freq;
	float levelHi, levelLo;
	config.getId(callsign, beacon, speed, freq, levelHi, levelLo);
	::printf("Callsign value: \"%s\", beacon value: \"%s\", speed: %u WPM, freq: %u Hz, level hi: %.3f, level lo: %.3f\n", callsign.c_str(), beacon.c_str(), speed, freq, levelHi, levelLo);

	CCWKeyer* callsignAudio = new CCWKeyer(callsign, speed, freq, ANALOGUE_RADIO_SAMPLE_RATE);
	CCWKeyer* beaconAudio   = new CCWKeyer(beacon,   speed, freq, ANALOGUE_RADIO_SAMPLE_RATE);

	m_thread->setCallsign(callsignAudio, beaconAudio, levelHi, levelLo);

	std::string style;
	unsigned int ack, minimum;
	float level;
	config.getAck(style, speed, freq, level, ack, minimum);
	::printf("Ack style: \"%s\", speed: %u WPM, freq: %u Hz, level: %.3f, ack: %u ms, minimum: %u ms\n", style.c_str(), speed, freq, level, ack, minimum);

	CCWKeyer* ackAudio = new CCWKeyer(style, speed, freq, ANALOGUE_RADIO_SAMPLE_RATE);

	m_thread->setAck(ackAudio, level, ack, minimum);

	unsigned int callsignTime, timeout, lockoutTime, hangTime, kerchunkTime;
	config.getTimes(callsignTime, timeout, lockoutTime, hangTime, kerchunkTime);
	m_thread->setTimes(callsignTime, timeout, lockoutTime, hangTime, kerchunkTime);
	::printf("Times set to: callsign time: %u mins, timeout: %u secs, lockout time: %u secs, hang time: %u secs, kerchunk time: %u secs\n", callsignTime, timeout, lockoutTime, hangTime, kerchunkTime);

	float ctcssFreq, ctcssThresh, ctcssLevel;
	unsigned int ctcssHangTime;
	ANALOGUE_CTCSS_OUTPUT ctcssOutput;
	config.getTones(ctcssFreq, ctcssThresh, ctcssLevel, ctcssHangTime, ctcssOutput);
	m_thread->setTones(ctcssFreq, ctcssThresh, ctcssLevel, ctcssHangTime, ctcssOutput);
	::printf("Tones set to: CTCSS freq: %.1f Hz, threshold: %.3f, level: %.3f, hang time: %u ms, output: %d\n", ctcssFreq, ctcssThresh, ctcssLevel, ctcssHangTime * 20U, ctcssOutput);

	ANALOGUE_CALLSIGN_START callsignAtStart;
	unsigned int callsignStartDelay, callsignHoldoff;
	bool callsignAtEnd;
	ANALOGUE_TIMEOUT_TYPE timeoutType;
	config.getFeel(callsignAtStart, callsignStartDelay, callsignAtEnd, timeoutType, callsignHoldoff);
	m_thread->setFeel(callsignAtStart, callsignStartDelay, callsignAtEnd, timeoutType, callsignHoldoff);
	::printf("Feel set to: callsignAtStart: %d, callsignStartDelay: %u s, callsignAtEnd: %u, timeoutType: %d, callsignHoldoff: %u mins\n", callsignAtStart, callsignStartDelay, callsignAtEnd, timeoutType, callsignHoldoff);

	std::string readDevice, writeDevice;
	unsigned int audioDelay;
	config.getRadio(readDevice, writeDevice, audioDelay);
	::printf("Soundcard set to %s:%s, delay: %u ms\n", readDevice.c_str(), writeDevice.c_str(), audioDelay * 20U);

	if (!readDevice.empty() && !writeDevice.empty()) {
		CSoundCardReaderWriter* soundcard = new CSoundCardReaderWriter(readDevice, writeDevice, ANALOGUE_RADIO_SAMPLE_RATE, ANALOGUE_RADIO_BLOCK_SIZE);
		soundcard->setCallback(m_thread);

		bool res = soundcard->open();
		if (!res) {
			::fprintf(stderr, "Cannot open the radio sound card\n");
			return false;
		}

		m_thread->setRadio(soundcard, audioDelay);
	}

	std::string type, port;
	unsigned int pttDelay, squelchDelay;
	bool pttInvert, squelchInvert;
	config.getController(type, port, pttDelay, squelchDelay, pttInvert, squelchInvert);
	::printf("Controller set to %s, port: %s, ptt delay: %u ms, squelch delay: %u ms, ptt invert: %d, squelch invert: %d\n", type.c_str(), port.c_str(), pttDelay * 20U, squelchInvert * 20U, pttInvert, squelchInvert);

	CExternalController* controller = NULL;

	if (type == "Arduino") {
		controller = new CExternalController(new CArduinoController(port), pttInvert, squelchInvert);
	} else if (type == "UDRC") {
		controller = new CExternalController(new CUDRCController, pttInvert, squelchInvert);
	} else {
		controller = new CExternalController(new CDummyController, pttInvert, squelchInvert);
	}

	bool res = controller->open();
	if (!res) {
		::fprintf(stderr, "Cannot open the hardware interface - %s\n", type.c_str());
		return false;
	}

	m_thread->setController(controller, pttDelay, squelchDelay);

#if !defined(_WIN32) && !defined(_WIN64)
	bool daemon = config.isDaemon();
	if (daemon) {
		// Create new process
		pid_t pid = ::fork();
		if (pid == -1) {
			::fprintf(stderr, "Couldn't fork() , exiting\n");
			return false;
		} else if (pid != 0)
			exit(EXIT_SUCCESS);

		// Create new session and process group
		if (::setsid() == -1) {
			::fprintf(stderr, "Couldn't setsid(), exiting\n");
			return false;
		}

		// Set the working directory to the root directory
		if (::chdir("/") == -1) {
			::fprintf(stderr, "Couldn't cd /, exiting\n");
			return false;
		}

		::close(STDIN_FILENO);
		::close(STDOUT_FILENO);
		::close(STDERR_FILENO);

		//If we are currently root...
		if (getuid() == 0) {
			struct passwd* user = ::getpwnam("mmdvm");
			if (user == NULL) {
				::fprintf(stderr, "Could not get the mmdvm user, exiting\n");
				return false;
			}

			uid_t mmdvm_uid = user->pw_uid;
			gid_t mmdvm_gid = user->pw_gid;

			//Set user and group ID's to mmdvm:mmdvm
			if (setgid(mmdvm_gid) != 0) {
				::fprintf(stderr, "Could not set mmdvm GID, exiting\n");
				return false;
			}

			if (setuid(mmdvm_uid) != 0) {
				::fprintf(stderr, "Could not set mmdvm UID, exiting\n");
				return false;
			}

			//Double check it worked (AKA Paranoia) 
			if (setuid(0) != -1) {
				::fprintf(stderr, "It's possible to regain root - something is wrong!, exiting\n");
				return false;
			}
		}
	}
#endif

	m_thread->run();

	return true;
}

