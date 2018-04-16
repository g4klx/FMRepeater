/*
 *	Copyright (C) 2009,2010,2011,2013,2018 by Jonathan Naylor, G4KLX
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; version 2 of the License.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 */

#ifndef	AnalogueDefines_H
#define	AnalogueDefines_H

const unsigned int ANALOGUE_RADIO_SAMPLE_RATE = 48000U;
const unsigned int ANALOGUE_RADIO_BLOCK_SIZE  = 960U;

const float MAX_VOGAD_GAIN  = 6.0F;
const float MAX_AUDIO_LEVEL = 0.5F;

const unsigned int ANALOGUE_FRAME_TIME_MS = 20U;

const unsigned int ANALOGUE_TICKS_PER_SEC = ANALOGUE_RADIO_SAMPLE_RATE / ANALOGUE_RADIO_BLOCK_SIZE;

enum ANALOGUE_RPT_STATE {
	ARS_SHUTDOWN,
	ARS_LISTENING,
	ARS_RELAYING,
	ARS_WAITING,
	ARS_TIMEOUT,
	ARS_LOCKOUT
};

enum ANALOGUE_SQUELCH {
	AS_CLOSED,
	AS_CARRIER,
	AS_CTCSS,
	AS_EXTERNAL
};

enum ANALOGUE_CALLSIGN_START {
	ACS_NONE,
	ACS_OPEN,
	ACS_LATCH
};

enum ANALOGUE_CTCSS_OUTPUT {
	ACO_NONE,
	ACO_WHEN_OPEN,
	ACO_ON_AUDIO,
	ACO_ALWAYS
};

#endif
