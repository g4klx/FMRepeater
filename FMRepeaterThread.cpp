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

#include "FMRepeaterThread.h"
#include "StopWatch.h"

#include <cstdio>
#include <cassert>
#include <cmath>

// A band pass filter created with
// b = fir1(200, [ 300 / 24000 3000 / 24000 ]);

const unsigned int FIRLP_LEN    = 201U;
const float    FIRLP_TAPS[] = {
	 4.348089722683099e-004F,  4.135042495540001e-004F,  3.565734316721585e-004F,
	 2.697620683049211e-004F,  1.630208019098757e-004F,  4.963421717875377e-005F,
	-5.511664728362165e-005F, -1.357146691842085e-004F, -1.784720386611927e-004F,
	-1.736941634835989e-004F, -1.176523500371192e-004F, -1.402211853398311e-005F,
	 1.255561999118871e-004F,  2.820687925363604e-004F,  4.310038699806798e-004F,
	 5.454413649542267e-004F,  6.000529950196235e-004F,  5.754846676938578e-004F,
	 4.624257460107961e-004F,  2.646027935871826e-004F, -8.998878026856994e-019F,
	-3.001857568578321e-004F, -5.950169554549807e-004F, -8.394648920034268e-004F,
	-9.915596586982927e-004F, -1.019998307501092e-003F, -9.109416298854615e-004F,
	-6.727015058714779e-004F, -3.372223785104403e-004F,  4.231523099903672e-005F,
	 3.978432840308519e-004F,  6.560960944864116e-004F,  7.503714920446452e-004F,
	 6.325803609781686e-004F,  2.835429460363310e-004F, -2.804399678111389e-004F,
	-1.006291058992636e-003F, -1.808947908432476e-003F, -2.582095951203979e-003F,
	-3.213662476370489e-003F, -3.603952054699695e-003F, -3.683632046000646e-003F,
	-3.428514864134490e-003F, -2.868316566104280e-003F, -2.087306999884778e-003F,
	-1.215927593116136e-003F, -4.138814968899417e-004F,  1.533200345187720e-004F,
	 3.410866837681884e-004F,  5.162936712129394e-005F, -7.453774810594479e-004F,
	-2.001260159459314e-003F, -3.588693388062809e-003F, -5.314977400685459e-003F,
	-6.947255754887167e-003F, -8.246635510914082e-003F, -9.006443163121124e-003F,
	-9.088753480449461e-003F, -8.453095966632742e-003F, -7.171983753681457e-003F,
	-5.429581110071774e-003F, -3.502232559034098e-003F, -1.722388887843640e-003F,
	-4.302625426330871e-004F,  8.012281697951944e-005F, -3.876344919835156e-004F,
	-1.891805835793981e-003F, -4.330331749140149e-003F, -7.442040563048283e-003F,
	-1.083314186121884e-002F, -1.402704694735273e-002F, -1.653176933584757e-002F,
	-1.791595309144118e-002F, -1.788250347606054e-002F, -1.632825233260431e-002F,
	-1.337927103416548e-002F, -9.394287349331992e-003F, -4.932853682002187e-003F,
	-6.899140995522271e-004F,  2.596466785959089e-003F,  4.252970468674810e-003F,
	 3.780101653662344e-003F,  9.468283669403368e-004F, -4.143948225037351e-003F,
	-1.103074294656373e-002F, -1.891637020172199e-002F, -2.673890106758282e-002F,
	-3.328462589955223e-002F, -3.733082798472451e-002F, -3.780140989137119e-002F,
	-3.391573795286361e-002F, -2.531094064719834e-002F, -1.212043181384624e-002F,
	 5.003619980645199e-003F,  2.492938215129132e-002F,  4.614378276854996e-002F,
	 6.690036788706782e-002F,  8.540010514415053e-002F,  9.998405763108859e-002F,
	 1.093149075489733e-001F,  1.125253099317730e-001F,  1.093149075489733e-001F,
	 9.998405763108859e-002F,  8.540010514415053e-002F,  6.690036788706782e-002F,
	 4.614378276854996e-002F,  2.492938215129132e-002F,  5.003619980645199e-003F,
	-1.212043181384624e-002F, -2.531094064719834e-002F, -3.391573795286361e-002F,
	-3.780140989137119e-002F, -3.733082798472451e-002F, -3.328462589955223e-002F,
	-2.673890106758282e-002F, -1.891637020172199e-002F, -1.103074294656373e-002F,
	-4.143948225037351e-003F,  9.468283669403368e-004F,  3.780101653662344e-003F,
	 4.252970468674810e-003F,  2.596466785959089e-003F, -6.899140995522271e-004F,
	-4.932853682002187e-003F, -9.394287349331992e-003F, -1.337927103416548e-002F,
	-1.632825233260431e-002F, -1.788250347606054e-002F, -1.791595309144118e-002F,
	-1.653176933584757e-002F, -1.402704694735273e-002F, -1.083314186121884e-002F,
	-7.442040563048283e-003F, -4.330331749140149e-003F, -1.891805835793981e-003F,
	-3.876344919835156e-004F,  8.012281697951944e-005F, -4.302625426330871e-004F,
	-1.722388887843640e-003F, -3.502232559034098e-003F, -5.429581110071774e-003F,
	-7.171983753681457e-003F, -8.453095966632742e-003F, -9.088753480449461e-003F,
	-9.006443163121124e-003F, -8.246635510914082e-003F, -6.947255754887167e-003F,
	-5.314977400685459e-003F, -3.588693388062809e-003F, -2.001260159459314e-003F,
	-7.453774810594479e-004F,  5.162936712129394e-005F,  3.410866837681884e-004F,
	 1.533200345187720e-004F, -4.138814968899417e-004F, -1.215927593116136e-003F,
	-2.087306999884778e-003F, -2.868316566104280e-003F, -3.428514864134490e-003F,
	-3.683632046000646e-003F, -3.603952054699695e-003F, -3.213662476370489e-003F,
	-2.582095951203979e-003F, -1.808947908432476e-003F, -1.006291058992636e-003F,
	-2.804399678111389e-004F,  2.835429460363310e-004F,  6.325803609781686e-004F,
	 7.503714920446452e-004F,  6.560960944864116e-004F,  3.978432840308519e-004F,
	 4.231523099903672e-005F, -3.372223785104403e-004F, -6.727015058714779e-004F,
	-9.109416298854615e-004F, -1.019998307501092e-003F, -9.915596586982927e-004F,
	-8.394648920034268e-004F, -5.950169554549807e-004F, -3.001857568578321e-004F,
	-8.998878026856994e-019F,  2.646027935871826e-004F,  4.624257460107961e-004F,
	 5.754846676938578e-004F,  6.000529950196235e-004F,  5.454413649542267e-004F,
	 4.310038699806798e-004F,  2.820687925363604e-004F,  1.255561999118871e-004F,
	-1.402211853398311e-005F, -1.176523500371192e-004F, -1.736941634835989e-004F,
	-1.784720386611927e-004F, -1.357146691842085e-004F, -5.511664728362165e-005F,
	 4.963421717875377e-005F,  1.630208019098757e-004F,  2.697620683049211e-004F,
	 3.565734316721585e-004F,  4.135042495540001e-004F,  4.348089722683099e-004F};


CFMRepeaterThread::CFMRepeaterThread() :
m_soundcard(NULL),
m_controller(NULL),
m_pttDelay(NULL),
m_squelchDelay(NULL),
m_stopped(true),
m_state(ARS_LISTENING),
m_radioInBuffer(ANALOGUE_RADIO_BLOCK_SIZE * 5U),
m_radioOutBuffer(ANALOGUE_RADIO_BLOCK_SIZE * 5U),
m_ctcssIn(NULL),
m_ctcssOut(NULL),
m_ctcssOutput(ACO_WHEN_OPEN),
m_timeoutTones(NULL),
m_callsignAudio(NULL),
m_beaconAudio(NULL),
m_ackAudio(NULL),
m_timeoutTimer(1000U),
m_lockoutTimer(1000U),
m_callsignTimer(1000U),
m_callsignStartTimer(1000U),
m_hangTimer(1000U),
m_kerchunkTimer(1000U),
m_minimumTimer(1000U),
m_ackTimer(1000U),
m_idLevelHi(0.0F),
m_idLevelLo(0.0F),
m_ackLevel(0.0F),
m_ctcssLevel(0.0F),
m_radioAudioDelay(NULL),
m_audioFilter(FIRLP_TAPS, FIRLP_LEN),
m_callsignAtStart(ACS_NONE),
m_callsignAtEnd(false),
m_callsignHoldoff(6U),
m_squelch(AS_CLOSED),
m_radioTransmit(false),
m_firstTime(false),
m_sendOpen(false),
m_sendClose(false),
m_sendCallsign(false),
m_sendBeacon(false),
m_sendAck(ACK_NONE),
m_ctcssHangTimer(1000U),
m_shutdown(false)
{
}

CFMRepeaterThread::~CFMRepeaterThread()
{
}

void CFMRepeaterThread::run()
{
	m_stopped = false;

	m_callsignTimer.start();
	m_controller->setTransmit(false);
	m_controller->setActive(false);

	m_radioInBuffer.clear();

	unsigned int counter = 0U;

	CStopWatch timer;

	try {
		for (;;) {
			timer.start();

			float radioAudio[ANALOGUE_RADIO_BLOCK_SIZE];
			unsigned int nRadio = 0U;
			getAudio(radioAudio, nRadio);

			// Set the watchdog port every one second
			counter++;
			if (counter == 50U) {
				m_controller->setHeartbeat();
				counter = 0U;
			}

			// Check the shutdown state
			bool disable = m_controller->getDisable();
			if (disable || m_shutdown) {
				if (m_state != ARS_SHUTDOWN)
					setState(ARS_SHUTDOWN);
			} else {
				if (m_state == ARS_SHUTDOWN)
					setState(ARS_LISTENING);
			}

			// If we're shutdown then don't go any further
			if (m_state == ARS_SHUTDOWN)
				continue;

			// Set the output state
			if (m_state != ARS_LISTENING || m_sendCallsign || m_sendOpen || m_sendClose || m_sendBeacon || m_callsignStartTimer.isRunning()) {
				m_controller->setActive(true);
			} else {
				m_controller->setActive(false);
			}

			// Detect a suitable access signal, 1750Hz, CTCSS, or carrier
			m_squelch = checkRadioSquelch(radioAudio, nRadio, m_squelch);

			stateMachine();

			m_radioTransmit = m_state == ARS_RELAYING ||
					  m_state == ARS_WAITING  ||
					  m_state == ARS_TIMEOUT  ||
					  m_sendCallsign || m_sendBeacon || m_sendOpen || m_sendClose || m_callsignStartTimer.isRunning();

			// The audio is chosen depending on the squelch and state
			if (m_state != ARS_RELAYING || m_squelch == AS_CLOSED) {
				// No open squelch, silence
				nRadio = ANALOGUE_RADIO_BLOCK_SIZE;
				::memset(radioAudio, 0x00, ANALOGUE_RADIO_BLOCK_SIZE * sizeof(float));
			}

			// From here onwards, the audio is destined for the radio only

			// Add tones
			sendTones(radioAudio, nRadio);

			// Filter the audio
			filterAudio(radioAudio, nRadio);

			// Insert CTCSS here because of the role off in the pre-emphasis filter
			insertCTCSS(radioAudio, nRadio);

			// Set the transmitter, and provide some audio
			feedRadio(radioAudio, nRadio);

			unsigned int ms = timer.elapsed();
			clock(ms);
		}
	}
	catch (std::exception& e) {
		::fprintf(stderr, "Exception raised - \"%s\"\n", e.what());
	}
	catch (...) {
		::fprintf(stderr, "Unknown exception raised\n");
	}

	m_soundcard->close();

	m_controller->setTransmit(false);
	m_controller->setActive(false);
	m_controller->close();

	delete m_timeoutTones;
	delete m_soundcard;
	delete m_controller;
	delete m_radioAudioDelay;
	delete m_pttDelay;
	delete m_squelchDelay;
	delete m_ctcssIn;
	delete m_ctcssOut;
	delete m_callsignAudio;
	delete m_beaconAudio;
	delete m_ackAudio;
}

void CFMRepeaterThread::setCallsign(CCWKeyer* callsignAudio, CCWKeyer* beaconAudio, float levelHi, float levelLo)
{
	m_callsignAudio = callsignAudio;
	m_beaconAudio   = beaconAudio;

	m_idLevelHi = levelHi * MAX_AUDIO_LEVEL / 1.0F;		// Max of 100% of full deviation
	m_idLevelLo = levelLo * MAX_AUDIO_LEVEL / 2.0F;		// Max of 50% of full deviation
}

void CFMRepeaterThread::setAck(CCWKeyer* ackAudio, float level, unsigned int ack, unsigned int minimum)
{
	m_ackAudio = ackAudio;

	// Add the ack delay to the minimum time, if it's set
	minimum *= 1000U;
	if (minimum > 0U)
		minimum += ack;

	m_ackTimer.setTimeout(0U, ack);
	m_minimumTimer.setTimeout(0U, minimum);
	m_ackLevel = level * MAX_AUDIO_LEVEL / 1.0F;			// Max of 100% of full deviation
}

void CFMRepeaterThread::setTimes(unsigned int callsignTime, unsigned int timeout, unsigned int lockoutTime, unsigned int hangTime, unsigned int& kerchunkTime)
{
	m_callsignTimer.setTimeout(callsignTime * 60U);
	m_timeoutTimer.setTimeout(timeout);
	m_lockoutTimer.setTimeout(lockoutTime);
	m_hangTimer.setTimeout(hangTime);
	m_kerchunkTimer.setTimeout(kerchunkTime);
}

void CFMRepeaterThread::setTones(float ctcssFreq, float ctcssThresh, float ctcssLevel, unsigned int ctcssHangTime, ANALOGUE_CTCSS_OUTPUT ctcssOutput)
{
	ctcssThresh *= 500000.0F;			// Based on testing by Jon, G4TSN

	m_ctcssOutput = ctcssOutput;

	if (ctcssFreq != 0.0F)
		m_ctcssIn = new CGoertzel(ANALOGUE_RADIO_SAMPLE_RATE, ctcssFreq, ANALOGUE_RADIO_SAMPLE_RATE / 5U, ctcssThresh);	// 5 Hz B/W

	if (ctcssFreq != 0.0F && ctcssOutput != ACO_NONE)
		m_ctcssOut = new CNCO(ANALOGUE_RADIO_SAMPLE_RATE, ctcssFreq);

	// CTCSS deviation in the UK for 5 kHz total deviation is 500 Hz +-200 Hz
	m_ctcssLevel = ctcssLevel * MAX_AUDIO_LEVEL / 5.0F;			// Max of 20% of full deviation

	m_ctcssHangTimer.setTimeout(0U, ctcssHangTime * 20U);
}

void CFMRepeaterThread::setFeel(ANALOGUE_CALLSIGN_START callsignAtStart, unsigned int callsignStartDelay, bool callsignAtEnd, ANALOGUE_TIMEOUT_TYPE timeoutStyle, unsigned int callsignHoldoff)
{
	m_callsignAtStart = callsignAtStart;
	m_callsignAtEnd   = callsignAtEnd;
	m_callsignHoldoff = callsignHoldoff;

	m_callsignStartTimer.setTimeout(callsignStartDelay);

	m_timeoutTones = new CTimeoutTones(ANALOGUE_RADIO_SAMPLE_RATE, timeoutStyle);
}

void CFMRepeaterThread::setRadio(CSoundCardReaderWriter* soundcard, unsigned int audioDelay)
{
	assert(soundcard != NULL);

	m_soundcard = soundcard;

	if (audioDelay > 0U)
		m_radioAudioDelay = new CAudioDelay(audioDelay * ANALOGUE_FRAME_TIME_MS * ANALOGUE_RADIO_SAMPLE_RATE / 1000U);
}

void CFMRepeaterThread::setController(CExternalController* controller, unsigned int pttDelay, unsigned int squelchDelay)
{
	assert(controller != NULL);

	m_controller = controller;

	if (pttDelay > 0U)
		m_pttDelay = new CPTTDelay(pttDelay);

	if (squelchDelay > 0U)
		m_squelchDelay = new CPTTDelay(squelchDelay);
}

void CFMRepeaterThread::readCallback(const float* input, unsigned int nSamples)
{
	if (m_stopped)
		return;

	m_radioInBuffer.addData(input, nSamples);
}

void CFMRepeaterThread::writeCallback(float* output, int& nSamples)
{
	if (nSamples == 0)
		return;

	::memset(output, 0x00, nSamples * sizeof(float));

	if (m_stopped)
		return;

	nSamples = m_radioOutBuffer.getData(output, nSamples);
}

void CFMRepeaterThread::getAudio(float* radioAudio, unsigned int& nRadio)
{
	assert(radioAudio != NULL);

	nRadio = 0U;

	::memset(radioAudio, 0x00, ANALOGUE_RADIO_BLOCK_SIZE * sizeof(float));

	// Get radio audio
	for (unsigned int n = 0U; nRadio < ANALOGUE_RADIO_BLOCK_SIZE && n < 20U; n++) {
		nRadio += m_radioInBuffer.getData(radioAudio + nRadio, ANALOGUE_RADIO_BLOCK_SIZE - nRadio);

		if (nRadio < ANALOGUE_RADIO_BLOCK_SIZE)
			CThread::sleep(ANALOGUE_FRAME_TIME_MS / 4UL);
	}

	if (m_radioAudioDelay != NULL && nRadio > 0U)
		m_radioAudioDelay->delay(radioAudio, nRadio);

	if (nRadio < ANALOGUE_RADIO_BLOCK_SIZE)
		::fprintf(stderr, "No radio audio is being received\n");
}

ANALOGUE_SQUELCH CFMRepeaterThread::checkRadioSquelch(const float* audio, unsigned int length, ANALOGUE_SQUELCH squelch)
{
	assert(audio != NULL);

	bool squelchIn = m_controller->getSquelch();

	if (m_squelchDelay != NULL)
		squelchIn = m_squelchDelay->delay(squelchIn);

	if (!squelchIn)
		return AS_CLOSED;

	if (m_state == ARS_LISTENING) {
		// 1750Hz access
	} else {
		// Hardware squelch open
		if (squelchIn)
			return AS_CARRIER;
	}

	if (m_ctcssIn != NULL) {
		m_ctcssIn->process(audio, length);
		TRISTATE state = m_ctcssIn->getDetected();

		if (squelchIn && state == STATE_TRUE) {
			m_ctcssHangTimer.start();
			return AS_CTCSS;
		}

		if (squelchIn && state == STATE_UNKNOWN && squelch == AS_CTCSS) {
			m_ctcssHangTimer.start();
			return AS_CTCSS;
		}

		if (squelchIn && m_ctcssHangTimer.isRunning() && !m_ctcssHangTimer.hasExpired())
			return AS_CTCSS;

		if (m_ctcssHangTimer.isRunning() && m_ctcssHangTimer.hasExpired())
			m_ctcssHangTimer.stop();
	}

	return AS_CLOSED;
}

void CFMRepeaterThread::sendTones(float* audio, unsigned int length)
{
	assert(audio != NULL);

	if (length == 0U)
		return;

	// Transmitting over the ack, abort it
	if (m_sendAck != ACK_NONE && m_squelch != AS_CLOSED) {
		m_sendAck = ACK_NONE;

		if (m_ackAudio != NULL)
			m_ackAudio->reset();

		return;
	}

	// Do callsign here so that it pre-empts the ack, unless ack is already sending
	if (m_sendCallsign && m_sendAck != ACK_RADIO_SENDING) {
		unsigned int len;
		if (m_state == ARS_LISTENING || m_state == ARS_TIMEOUT || m_state == ARS_LOCKOUT)
			len = m_callsignAudio->getAudio(audio, length, m_idLevelHi);
		else
			len = m_callsignAudio->getAudio(audio, length, m_idLevelLo);

		if (len < length) {
			m_sendCallsign = false;
			m_callsignAudio->reset();
			m_callsignTimer.start();
		}

		return;
	}

	if (m_sendOpen && m_sendAck != ACK_RADIO_SENDING) {
		unsigned int len;
		if (m_state == ARS_LISTENING || m_state == ARS_TIMEOUT || m_state == ARS_LOCKOUT)
			len = m_callsignAudio->getAudio(audio, length, m_idLevelHi);
		else
			len = m_callsignAudio->getAudio(audio, length, m_idLevelLo);

		if (len < length) {
			m_sendOpen = false;
			m_callsignAudio->reset();
			m_callsignTimer.start();
		}

		return;
	}

	if (m_sendClose && m_sendAck != ACK_RADIO_SENDING) {
		unsigned int len;
		if (m_state == ARS_LISTENING || m_state == ARS_TIMEOUT || m_state == ARS_LOCKOUT)
			len = m_callsignAudio->getAudio(audio, length, m_idLevelHi);
		else
			len = m_callsignAudio->getAudio(audio, length, m_idLevelLo);

		if (len < length) {
			m_sendClose = false;
			m_callsignAudio->reset();
			m_callsignTimer.start();
		}

		return;
	}

	if (m_sendBeacon) {
		unsigned int len;
		if (m_state == ARS_LISTENING)
			len = m_beaconAudio->getAudio(audio, length, m_idLevelHi);
		else
			len = m_beaconAudio->getAudio(audio, length, m_idLevelLo);

		if (len < length) {
			m_beaconAudio->reset();
	    		m_sendBeacon = false;
			m_callsignTimer.start();
			return;
		}
	}

	if (m_sendAck == ACK_RADIO_WAITING || m_sendAck == ACK_RADIO_SENDING) {
		if (m_ackAudio == NULL) {
			m_sendAck = ACK_NONE;
			return;
		}

		unsigned int len = m_ackAudio->getAudio(audio, length, m_ackLevel);

		if (len < length) {
			m_sendAck = ACK_NONE;
			m_ackAudio->reset();
			return;
		}

		m_sendAck = ACK_RADIO_SENDING;
		return;
	}

	// The timeout tone, here so that a callsign can pre-empt it
	if (m_state == ARS_TIMEOUT)
		m_timeoutTones->getAudio(audio, length, m_idLevelHi);
}

void CFMRepeaterThread::clock(unsigned int ms)
{
	m_callsignTimer.clock(ms);
	if (m_callsignTimer.hasExpired()) {
		if (m_state == ARS_LISTENING) {
			if (!m_beaconAudio->isEmpty() && !m_sendBeacon)
				m_sendBeacon = true;
		} else {
			if (!m_callsignAudio->isEmpty() && !m_sendCallsign)
				m_sendCallsign = true;
		}
	}

	m_callsignStartTimer.clock(ms);
	if (m_callsignStartTimer.isRunning() && m_callsignStartTimer.hasExpired()) {
		m_sendOpen = true;
		m_callsignStartTimer.stop();
	}

	m_hangTimer.clock(ms);
	m_ackTimer.clock(ms);
	m_ctcssHangTimer.clock(ms);

	if (m_squelch != AS_CLOSED) {
		m_kerchunkTimer.clock(ms);
		m_minimumTimer.clock(ms);
		m_timeoutTimer.clock(ms);
		m_lockoutTimer.clock(ms);
	}
}

void CFMRepeaterThread::stateMachine()
{
	switch (m_state) {
		case ARS_SHUTDOWN:
			break;

		case ARS_LISTENING:
			if (m_squelch != AS_CLOSED) {
				// Squelch open
				if (m_callsignAtStart == ACS_OPEN)
					sendOpen();

				setState(ARS_RELAYING);
			}
			break;

		case ARS_RELAYING:
			if (m_squelch != AS_CLOSED) {
				// Squelch open
				if (m_timeoutTimer.isRunning() && m_timeoutTimer.hasExpired()) {
					// The input has timed out
					setState(ARS_TIMEOUT);
				}
			} else {
				// Squelch closed
				if (m_firstTime && m_kerchunkTimer.isRunning() && !m_kerchunkTimer.hasExpired()) {
					// Transmission not long enough, closedown
					setState(ARS_LISTENING);
				} else {
					// If first time and the latch was running, send a callsign if needed
					if (m_firstTime && m_kerchunkTimer.isRunning() && m_kerchunkTimer.hasExpired()) {
						if (m_callsignAtStart == ACS_LATCH)
							sendOpen();
					}

					// Signal long enough, or not first time
					setState(ARS_WAITING);
				}
			}
			break;

		case ARS_WAITING:
			if (m_squelch != AS_CLOSED) {
				// Squelch open
				setState(ARS_RELAYING);
			} else {
				// Squelch closed, is it time for the ack?
				if (m_ackTimer.hasExpired()) {
					// Only send the audible ack after the minimum time
					if (!m_minimumTimer.isRunning() || (m_minimumTimer.isRunning() && m_minimumTimer.hasExpired()))
						m_sendAck = ACK_RADIO_WAITING;

					m_ackTimer.stop();
					m_timeoutTimer.stop();
					m_lockoutTimer.stop();
					m_hangTimer.start();
					m_minimumTimer.stop();
				}
				if (m_hangTimer.hasExpired()) {
					// Shutdown
					if (m_callsignAtEnd)
						sendClose();
					setState(ARS_LISTENING);
				}
			}
			break;

		case ARS_TIMEOUT:
			if (m_squelch == AS_CLOSED) {
				setState(ARS_WAITING);
			} else {
				if (m_lockoutTimer.hasExpired()) {
					sendCallsign();
					setState(ARS_LOCKOUT);
				}
			}
			break;

		case ARS_LOCKOUT:
			if (m_squelch == AS_CLOSED)
				setState(ARS_WAITING);
			break;
	}
}

void CFMRepeaterThread::setState(ANALOGUE_RPT_STATE state)
{
	// The 'from' state
	switch (m_state) {
		case ARS_SHUTDOWN:
			m_ackTimer.stop();
			m_hangTimer.stop();
			m_kerchunkTimer.stop();
			m_minimumTimer.stop();
			m_timeoutTimer.stop();
			m_lockoutTimer.stop();
			m_callsignTimer.start();
			m_callsignStartTimer.stop();
			m_radioTransmit = false;
			m_sendCallsign  = false;
			m_sendBeacon    = false;
			m_sendOpen      = false;
			m_sendClose     = false;
			m_sendAck       = ACK_NONE;
			break;

		case ARS_LISTENING:
			m_kerchunkTimer.start();
			m_firstTime = true;
			break;

		case ARS_TIMEOUT:
			m_timeoutTimer.stop();
			m_lockoutTimer.stop();
			break;

		case ARS_LOCKOUT:
			m_lockoutTimer.stop();
			break;

		default:
			break;
	}

	// The 'to' state
	switch (state) {
		case ARS_SHUTDOWN:
			m_ackTimer.stop();
			m_hangTimer.stop();
			m_kerchunkTimer.stop();
			m_minimumTimer.stop();
			m_timeoutTimer.stop();
			m_lockoutTimer.stop();
			m_callsignTimer.stop();
			m_callsignStartTimer.stop();
			m_radioTransmit = false;
			m_sendCallsign  = false;
			m_sendBeacon    = false;
			m_sendOpen      = false;
			m_sendClose     = false;
			m_sendAck       = ACK_NONE;
			m_state         = ARS_SHUTDOWN;
			m_controller->setTransmit(false);
			m_controller->setActive(false);
			break;

		case ARS_LISTENING:
			m_ackTimer.stop();
			m_kerchunkTimer.stop();
			m_kerchunkTimer.stop();
			m_minimumTimer.stop();
			m_timeoutTimer.stop();
			m_lockoutTimer.stop();
			m_state = ARS_LISTENING;
			break;

		case ARS_RELAYING:
			m_ackTimer.stop();
			m_hangTimer.stop();
			if (!m_minimumTimer.isRunning())
				m_minimumTimer.start();
			if (!m_timeoutTimer.isRunning())
				m_timeoutTimer.start();
			m_state = state;
			break;

		case ARS_WAITING:
			m_ackTimer.start();
			m_kerchunkTimer.stop();
			m_firstTime = false;
			m_state = state;
			break;

		case ARS_TIMEOUT:
			m_lockoutTimer.start();
			m_state = state;
			break;

		case ARS_LOCKOUT:
			m_lockoutTimer.stop();
			m_state = state;
			break;
	}
}

void CFMRepeaterThread::feedRadio(const float* audio, unsigned int length)
{
	assert(audio != NULL);

	if (m_radioTransmit)
		m_radioOutBuffer.addData(audio, length);

	if (m_pttDelay != NULL)
		m_controller->setTransmit(m_pttDelay->delay(m_radioTransmit));
	else
		m_controller->setTransmit(m_radioTransmit);
}

void CFMRepeaterThread::insertCTCSS(float* audio, unsigned int length)
{
	assert(audio != NULL);

	// Add the CTCSS to the output audio only when relaying user audio, not beacons or locked out

	if (length == 0U || m_ctcssOut == NULL || m_ctcssLevel == 0.0F)
		return;

	switch (m_ctcssOutput) {
		case ACO_WHEN_OPEN:
			switch (m_state) {
				// Always put CTCSS on if relaying audio
				case ARS_RELAYING:
					m_ctcssOut->getAudio(audio, length, m_ctcssLevel);
					return;

				case ARS_TIMEOUT:
					// If the callsign at the end is disabled.... put the CTCSS on until the tx closedown
					if (!m_callsignAtEnd) {
						m_ctcssOut->getAudio(audio, length, m_ctcssLevel);
						return;
					}

					// If the callsign at the end is enabled then shut off the CTCSS a second before it's sent
					if (!m_lockoutTimer.isRunning() || m_lockoutTimer.getRemaining() > 0U)
						m_ctcssOut->getAudio(audio, length, m_ctcssLevel);

					return;

				case ARS_WAITING:
					// If the callsign at the end is disabled.... put the CTCSS on until the tx closedown
					if (!m_callsignAtEnd) {
						m_ctcssOut->getAudio(audio, length, m_ctcssLevel);
						return;
					}

					// If the callsign at the end is enabled then shut off the CTCSS a second before it's sent
					if (!m_hangTimer.isRunning() || m_hangTimer.getRemaining() > 0U)
						m_ctcssOut->getAudio(audio, length, m_ctcssLevel);

					return;

				default:
					// No CTCSS otherwise
					return;
			}
			break;

		case ACO_ON_AUDIO:
			if (m_state == ARS_RELAYING)
				m_ctcssOut->getAudio(audio, length, m_ctcssLevel);
			break;

		case ACO_ALWAYS:
			if (m_radioTransmit)
				m_ctcssOut->getAudio(audio, length, m_ctcssLevel);
			break;

		default:
			break;
	}
}

void CFMRepeaterThread::filterAudio(float* audio, unsigned int length)
{
	assert(audio != NULL);

	if (length == 0U)
		return;

	if (m_radioTransmit)
		m_audioFilter.process(audio, length);
	else
		m_audioFilter.reset();
}

void CFMRepeaterThread::sendOpen()
{
	// Already sending, or waiting to send, ignore request
	if (m_sendCallsign || m_sendBeacon || m_sendOpen || m_sendClose || m_callsignStartTimer.isRunning())
		return;

	if (m_callsignHoldoff == 0U) {
		if (m_callsignStartTimer.getTimeout() > 0U)
			m_callsignStartTimer.start();
		else
			m_sendOpen = true;
		return;
	}

	unsigned int t = m_callsignTimer.getTimer();
	if (t >= m_callsignHoldoff) {
		if (m_callsignStartTimer.getTimeout() > 0U)
			m_callsignStartTimer.start();
		else
			m_sendOpen = true;
	}
}

void CFMRepeaterThread::sendClose()
{
	// Already sending, or waiting to send, ignore request
	if (m_sendCallsign || m_sendBeacon || m_sendOpen || m_sendClose || m_callsignStartTimer.isRunning())
		return;

	if (m_callsignHoldoff == 0U) {
		m_sendClose = true;
		return;
	}

	unsigned int t = m_callsignTimer.getTimer();
	if (t >= m_callsignHoldoff)
		m_sendClose = true;
}

void CFMRepeaterThread::sendCallsign()
{
	// Already sending, or waiting to send, ignore request
	if (m_sendCallsign || m_sendBeacon || m_sendOpen || m_sendClose || m_callsignStartTimer.isRunning())
		return;

	if (m_callsignHoldoff == 0U) {
		if (!m_callsignAudio->isEmpty())
			m_sendCallsign = true;
		return;
	}

	unsigned int t = m_callsignTimer.getTimer();
	if (t >= m_callsignHoldoff && !m_callsignAudio->isEmpty())
		m_sendCallsign = true;
}

