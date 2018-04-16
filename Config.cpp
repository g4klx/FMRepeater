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

#include "Config.h"

#include <cstdio>
#include <cstring>
#include <cassert>

enum SECTION {
	SECTION_NONE,
	SECTION_GENERAL,
	SECTION_ID,
	SECTION_ACK,
	SECTION_TIMES,
	SECTION_CTCSS,
	SECTION_TIMEOUT,
	SECTION_AUDIO,
	SECTION_INTERFACE
};

CConfig::CConfig(const std::string& filename) :
m_fileName(filename),

m_daemon(false),

m_idCallsign("G4KLX"),
m_idBeacon("G4KLX IO90TT"),
m_idSpeed(16U),
m_idFreq(1000U),
m_idLevelHi(0.5F),
m_idLevelLo(0.2F),
m_idAtStart(ACS_NONE),
m_idStartDelay(0U),
m_idAtEnd(true),
m_idTime(10U),
m_idHoldoff(6U),

m_ackStyle("K"),
m_ackSpeed(16U),
m_ackFreq(1750U),
m_ackLevel(0.5F),
m_ackDelay(1000U),
m_ackMin(4U),

m_timeHang(6U),
m_timeKerchunk(3U),

m_ctcssFreq(71.9F),
m_ctcssThresh(200.0F),
m_ctcssLevel(0.02F),
m_ctcssHangTime(100U),
m_ctcssTXMode(ACO_ALWAYS),

m_timeoutTime(120U),
m_timeoutStyle(ATT_UK),
m_timeoutLockout(1U),

m_audioReadDevice(),
m_audioWriteDevice(),
m_audioDelay(120U),

//m_lockoutTime(DEFAULT_LOCKOUTTIME),
m_interfaceType("None"),
m_interfacePort(),
m_pttDelay(200U),
m_squelchDelay(100U),
m_pttInvert(false),
m_squelchInvert(false)
{
	assert(!filename.empty());

	FILE* fp = ::fopen(filename.c_str(), "rt");
	if (fp == NULL) {
		::fprintf(stderr, "Cannot open the config file - %s\n", m_fileName.c_str());
		return;
	}

	SECTION section = SECTION_NONE;

	char buffer[200U];
	while (::fgets(buffer, 200, fp) != NULL) {
		if (buffer[0U] == '#')
			continue;

		if (buffer[0U] == '[') {
			if (::strncmp(buffer, "[General]", 9U) == 0)
				section = SECTION_GENERAL;
			else if (::strncmp(buffer, "[Id]", 4U) == 0)
				section = SECTION_ID;
			else if (::strncmp(buffer, "[Ack]", 5U) == 0)
				section = SECTION_ACK;
			else if (::strncmp(buffer, "[Times]", 7U) == 0)
				section = SECTION_TIMES;
			else if (::strncmp(buffer, "[CTCSS]", 7U) == 0)
				section = SECTION_CTCSS;
			else if (::strncmp(buffer, "[Timeout]", 9U) == 0)
				section = SECTION_TIMEOUT;
			else if (::strncmp(buffer, "[Audio]", 7U) == 0)
				section = SECTION_AUDIO;
			else if (::strncmp(buffer, "[Interface]", 11U) == 0)
				section = SECTION_INTERFACE;
			else
				section = SECTION_NONE;

			continue;
		}

		char* key = ::strtok(buffer, " \t=\r\n");
		if (key == NULL)
			continue;

		char* val = ::strtok(NULL, "\r\n");
		if (val == NULL)
			continue;

		if (section == SECTION_GENERAL) {
			if (::strcmp(key, "daemon") == 0) {
				m_daemon = ::atoi(val) == 1;
			}
		} else if (section == SECTION_ID) {
			if (::strcmp(key, "callsign") == 0) {
				m_idCallsign = val;
			} else if (::strcmp(key, "beacon") == 0) {
				m_idBeacon = val;
			} else if (::strcmp(key, "speed") == 0) {
				m_idSpeed = (unsigned int)::atoi(val);
			} else if (::strcmp(key, "freq") == 0) {
				m_idFreq = (unsigned int)::atoi(val);
			} else if (::strcmp(key, "levelHi") == 0) {
				m_idLevelHi = float(::atof(val));
			} else if (::strcmp(key, "levelLo") == 0) {
				m_idLevelLo = float(::atof(val));
			} else if (::strcmp(key, "atStart") == 0) {
				m_idAtStart = ANALOGUE_CALLSIGN_START(::atoi(val));
			} else if (::strcmp(key, "startDelay") == 0) {
				m_idStartDelay = (unsigned int)::atoi(val);
			} else if (::strcmp(key, "atEnd") == 0) {
				m_idAtEnd = ::atoi(val) == 1;
			} else if (::strcmp(key, "time") == 0) {
				m_idTime = (unsigned int)::atoi(val);
			} else if (::strcmp(key, "holdoff") == 0) {
				m_idHoldoff = (unsigned int)::atoi(val);
			}
		} else if (section == SECTION_ACK) {
			if (::strcmp(key, "style") == 0) {
				m_ackStyle = val;
			} else if (::strcmp(key, "speed") == 0) {
				m_ackSpeed = (unsigned int)::atoi(val);
			} else if (::strcmp(key, "freq") == 0) {
				m_ackFreq = (unsigned int)::atoi(val);
			} else if (::strcmp(key, "level") == 0) {
				m_ackLevel = float(::atof(val));
			} else if (::strcmp(key, "dekay") == 0) {
				m_ackDelay = (unsigned int)::atoi(val);
			} else if (::strcmp(key, "min") == 0) {
				m_ackMin = (unsigned int)::atoi(val);
			}
		} else if (section == SECTION_TIMES) {
			if (::strcmp(key, "hang") == 0) {
				m_timeHang = (unsigned int)::atoi(val);
			} else if (::strcmp(key, "kerchunk") == 0) {
				m_timeKerchunk = (unsigned int)::atoi(val);
			}
		} else if (section == SECTION_CTCSS) {
			if (::strcmp(key, "freq") == 0) {
				m_ctcssFreq = float(::atof(val));
			} else if (::strcmp(key, "threshold") == 0) {
				m_ctcssThresh = float(::atof(val));
			} else if (::strcmp(key, "level") == 0) {
				m_ctcssLevel = float(::atof(val));
			} else if (::strcmp(key, "hang") == 0) {
				m_ctcssHangTime = (unsigned int)::atoi(val);
			} else if (::strcmp(key, "txMode") == 0) {
				m_ctcssTXMode = ANALOGUE_CTCSS_OUTPUT(::atoi(val));
			}
		} else if (section == SECTION_TIMEOUT) {
			if (::strcmp(key, "time") == 0) {
				m_timeoutTime = (unsigned int)::atoi(val);
			} else if (::strcmp(key, "style") == 0) {
				m_timeoutStyle = ANALOGUE_TIMEOUT_TYPE(::atoi(val));
			} else if (::strcmp(key, "lockout") == 0) {
				m_timeoutLockout = (unsigned int)::atoi(val);
			}
		} else if (section == SECTION_AUDIO) {
			if (::strcmp(key, "rxDevice") == 0) {
				m_audioReadDevice = val;
			} else if (::strcmp(key, "txDevice") == 0) {
				m_audioWriteDevice = val;
			} else if (::strcmp(key, "delay") == 0) {
				m_audioDelay = (unsigned int)::atoi(val);
			}
		} else if (section == SECTION_INTERFACE) {
			// if (::strcmp(key, KEY_LOCKOUTTIME) == 0) {
			//	m_lockoutTime = (unsigned int)::atoi(val);
			if (::strcmp(key, "type") == 0) {
				m_interfaceType = val;
			} else if (::strcmp(key, "port") == 0) {
				m_interfacePort = val;
			} else if (::strcmp(key, "pttDelay") == 0) {
				m_pttDelay = (unsigned int)::atoi(val);
			} else if (::strcmp(key, "sqDelay") == 0) {
				m_squelchDelay = (unsigned int)::atoi(val);
			} else if (::strcmp(key, "pttInvert") == 0) {
				m_pttInvert = ::atoi(val) == 1;
			} else if (::strcmp(key, "sqInvert") == 0) {
				m_squelchInvert = ::atoi(val) == 1;
			}
		}
	}

	::fclose(fp);
}

CConfig::~CConfig()
{
}

bool CConfig::isDaemon() const
{
	return m_daemon;
}

void CConfig::getId(std::string& callsign, std::string& beacon, unsigned int& speed, unsigned int& freq, float& levelHi, float& levelLo) const
{
	callsign = m_idCallsign;
	beacon   = m_idBeacon;
	speed    = m_idSpeed;
	freq     = m_idFreq;
	levelHi  = m_idLevelHi;
	levelLo  = m_idLevelLo;
}

void CConfig::getAck(std::string& style, unsigned int& speed, unsigned int& freq, float& level, unsigned int& ack, unsigned int& minimum) const
{
	style      = m_ackStyle;
	speed      = m_ackSpeed;
	freq       = m_ackFreq;
	level      = m_ackLevel;
	ack        = m_ackDelay;
	minimum    = m_ackMin;
}

void CConfig::getTimes(unsigned int& callsignTime, unsigned int& timeout, unsigned int& lockout, unsigned int& hangTime, unsigned int& kerchunkTime) const
{
	callsignTime = m_idTime;
	timeout      = m_timeoutTime;
	lockout      = m_timeoutLockout;
	hangTime     = m_timeHang;
	kerchunkTime = m_timeKerchunk;
}

void CConfig::getTones(float& ctcssFreq, float& ctcssThresh, float& ctcssLevel, unsigned int& ctcssHangTime, ANALOGUE_CTCSS_OUTPUT& ctcssTXMode) const
{
	ctcssFreq     = m_ctcssFreq;
	ctcssThresh   = m_ctcssThresh;
	ctcssLevel    = m_ctcssLevel;
	ctcssHangTime = m_ctcssHangTime;
	ctcssTXMode   = m_ctcssTXMode;
}

void CConfig::getFeel(ANALOGUE_CALLSIGN_START& callsignAtStart, unsigned int& callsignStartDelay, bool& callsignAtEnd, ANALOGUE_TIMEOUT_TYPE& timeoutStyle, unsigned int& callsignHoldoff) const
{
	callsignAtStart    = m_idAtStart;
	callsignStartDelay = m_idStartDelay;
	callsignAtEnd      = m_idAtEnd;
	timeoutStyle       = m_timeoutStyle;
	callsignHoldoff    = m_idHoldoff;
}

void CConfig::getRadio(std::string& readDevice, std::string& writeDevice, unsigned int& delay) const
{
	readDevice  = m_audioReadDevice;
	writeDevice = m_audioWriteDevice;
	delay       = m_audioDelay;
}

void CConfig::getController(std::string& type, std::string& port, unsigned int& pttDelay, unsigned int& squelchDelay, bool& pttInvert, bool& squelchInvert) const
{
	type          = m_interfaceType;
	port          = m_interfacePort;
	pttDelay      = m_pttDelay;
	squelchDelay  = m_squelchDelay;
	pttInvert     = m_pttInvert;
	squelchInvert = m_squelchInvert;
}

