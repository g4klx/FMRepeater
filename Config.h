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

#ifndef	Config_H
#define	Config_H

#include "TimeoutTones.h"
#include "FMDefines.h"

#include <string>

class CConfig {
public:
	CConfig(const std::string& filename);
	~CConfig();

	bool isDaemon() const;

	void getId(std::string& callsign, std::string& beacon, unsigned int& speed, unsigned int& freq, float& levelHi, float& levelLo) const;

	void getAck(std::string& style, unsigned int& speed, unsigned int& freq, float& level, unsigned int& ack, unsigned int& minimum) const;

	void getTimes(unsigned int& callsignTime, unsigned int& timeout, unsigned int& lockout, unsigned int& hangTime, unsigned int& kerchunkTime) const;

	void getTones(float& ctcssFreq, float& ctcssThresh, float& ctcssLevel, unsigned int& ctcssHangTime, ANALOGUE_CTCSS_OUTPUT& ctcssTXMode) const;

	void getFeel(ANALOGUE_CALLSIGN_START& callsignAtStart, unsigned int& callsignStartDelay, bool& callsignAtEnd, ANALOGUE_TIMEOUT_TYPE& timeoutStyle, unsigned int& callsignHoldoff) const;

	void getRadio(std::string& readDevice, std::string& writeDevice, unsigned int& delay) const;

	void getController(std::string& type, std::string& port, unsigned int& pttDelay, unsigned int& squelchDelay, bool& pttInvert, bool& squelchInvert) const;

private:
	std::string   m_fileName;

	bool          m_daemon;

	std::string   m_idCallsign;
	std::string   m_idBeacon;
	unsigned int  m_idSpeed;
	unsigned int  m_idFreq;
	float         m_idLevelHi;
	float         m_idLevelLo;
	ANALOGUE_CALLSIGN_START m_idAtStart;
	unsigned int  m_idStartDelay;
	bool          m_idAtEnd;
	unsigned int  m_idTime;
	unsigned int  m_idHoldoff;

	std::string   m_ackStyle;
	unsigned int  m_ackSpeed;
	unsigned int  m_ackFreq;
	float         m_ackLevel;
	unsigned int  m_ackDelay;
	unsigned int  m_ackMin;

	unsigned int  m_timeHang;
	unsigned int  m_timeKerchunk;

	float         m_ctcssFreq;
	float         m_ctcssThresh;
	float         m_ctcssLevel;
	unsigned int  m_ctcssHangTime;
	ANALOGUE_CTCSS_OUTPUT m_ctcssTXMode;

	unsigned int  m_timeoutTime;
	ANALOGUE_TIMEOUT_TYPE m_timeoutStyle;
	unsigned int  m_timeoutLockout;

	std::string   m_audioReadDevice;
	std::string   m_audioWriteDevice;
	unsigned int  m_audioDelay;

	// unsigned int  m_lockoutTime;
	std::string   m_interfaceType;
	std::string   m_interfacePort;
	unsigned int  m_pttDelay;
	unsigned int  m_squelchDelay;
	bool          m_pttInvert;
	bool          m_squelchInvert;
};

#endif
