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

#ifndef	FMRepeaterThread_H
#define	FMRepeaterThread_H

#include "SoundCardReaderWriter.h"
#include "ExternalController.h"
#include "FMDefines.h"
#include "AudioCallback.h"
#include "TimeoutTones.h"
#include "RingBuffer.h"
#include "AudioDelay.h"
#include "FIRFilter.h"
#include "Goertzel.h"
#include "PTTDelay.h"
#include "CWKeyer.h"
#include "Timer.h"
#include "NCO.h"

#include <string>

enum ACK_TYPE {
	ACK_NONE,
	ACK_RADIO_WAITING,
	ACK_RADIO_SENDING,
	ACK_BATTERY_WAITING,
	ACK_BATTERY_SENDING
};

class CFMRepeaterThread : public IAudioCallback {
public:
	CFMRepeaterThread();
	virtual ~CFMRepeaterThread();

	virtual void readCallback(const float* input, unsigned int nSamples);
	virtual void writeCallback(float* output, int& nSamples);

	virtual void setCallsign(CCWKeyer* callsign, CCWKeyer* beacon, float levelHi, float levelLo);
	virtual void setAck(CCWKeyer* ackAudio, float level, unsigned int ack, unsigned int minimum);
	virtual void setTimes(unsigned int callsignTime, unsigned int timeout, unsigned int lockoutTime, unsigned int hangTime, unsigned int& kerchunkTime);
	virtual void setTones(float ctcssFreq, float ctcssThresh, float ctcssLevel, unsigned int ctcssHangTime, ANALOGUE_CTCSS_OUTPUT ctcssOutput);
	virtual void setFeel(ANALOGUE_CALLSIGN_START callsignAtStart, unsigned int callsignStartDelay, bool callsignAtEnd, ANALOGUE_TIMEOUT_TYPE timeoutType, unsigned int callsignHoldoff);
	virtual void setRadio(CSoundCardReaderWriter* soundcard, unsigned int audioDelay);
	virtual void setController(CExternalController* controller, unsigned int pttDelay, unsigned int squelchDelay);

	virtual void run();

private:
	CSoundCardReaderWriter*   m_soundcard;
	CExternalController*      m_controller;
	CPTTDelay*                m_pttDelay;
	CPTTDelay*                m_squelchDelay;
	bool                      m_stopped;
	ANALOGUE_RPT_STATE        m_state;
	CRingBuffer<float>        m_radioInBuffer;
	CRingBuffer<float>        m_radioOutBuffer;
	CGoertzel*                m_ctcssIn;
	CNCO*                     m_ctcssOut;
	ANALOGUE_CTCSS_OUTPUT     m_ctcssOutput;
	CTimeoutTones*            m_timeoutTones;
	CCWKeyer*                 m_callsignAudio;
	CCWKeyer*                 m_beaconAudio;
	CCWKeyer*                 m_ackAudio;
	CTimer                    m_timeoutTimer;
	CTimer                    m_lockoutTimer;
	CTimer                    m_callsignTimer;
	CTimer                    m_callsignStartTimer;
	CTimer                    m_hangTimer;
	CTimer                    m_kerchunkTimer;
	CTimer                    m_minimumTimer;
	CTimer                    m_ackTimer;
	float                     m_idLevelHi;
	float                     m_idLevelLo;
	float                     m_ackLevel;
	float                     m_ctcssLevel;
	CAudioDelay*              m_radioAudioDelay;
	CFIRFilter                m_audioFilter;
	ANALOGUE_CALLSIGN_START   m_callsignAtStart;
	bool                      m_callsignAtEnd;
	unsigned int              m_callsignHoldoff;
	ANALOGUE_SQUELCH          m_squelch;
	bool                      m_radioTransmit;
	bool                      m_firstTime;
	bool                      m_sendOpen;
	bool                      m_sendClose;
	bool                      m_sendCallsign;
	bool                      m_sendBeacon;
	ACK_TYPE                  m_sendAck;
	CTimer                    m_ctcssHangTimer;
	bool                      m_shutdown;

	void getAudio(float* audio, unsigned int& n);
	ANALOGUE_SQUELCH checkRadioSquelch(const float* audio, unsigned int length, ANALOGUE_SQUELCH squelch);
	void sendTones(float* audio, unsigned int length);
	void setState(ANALOGUE_RPT_STATE state);
	void stateMachine();
	void feedRadio(const float* audio, unsigned int length);
	void insertCTCSS(float* audio, unsigned int length);
	void filterAudio(float* audio, unsigned int length);
	void sendOpen();
	void sendClose();
	void sendCallsign();
	void clock(unsigned int ms);
};

#endif
