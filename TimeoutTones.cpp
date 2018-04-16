/*
 *	Copyright (C) 2009,2018 by Jonathan Naylor, G4KLX
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

#include "TimeoutTones.h"
#include "NCO.h"

#include <cstdio>
#include <cassert>
#include <cstring>
#include <cmath>

const float DL_FREQ = 425.0F;		// Frequency of German busy tone
const float DL_LEN  = 0.5F;			// Length of German busy tone

const float UK_FREQ = 400.0F;		// Frequency of UK busy tone
const float UK_LEN  = 0.4F;			// Length of UK busy tone

const float US_FREQ = 620.0F;		// Frequency of US busy tone
const float US_LEN  = 0.5F;			// Length of US busy tone

const unsigned int RC_LEN = 5U;			// The length of the raised cosine in ms

CTimeoutTones::CTimeoutTones(unsigned int sampleRate, ANALOGUE_TIMEOUT_TYPE type) :
m_data(NULL),
m_length(0U),
m_current(0U)
{
	assert(sampleRate > 0U);

	float len  = DL_LEN;
	float freq = DL_FREQ;

	switch (type) {
		case ATT_DL:
			len  = DL_LEN;
			freq = DL_FREQ;
			break;
		case ATT_UK:
			len  = UK_LEN;
			freq = UK_FREQ;
			break;
		case ATT_US:
			len  = US_LEN;
			freq = US_FREQ;
			break;
	}

	unsigned int length = (unsigned int)(len * float(sampleRate) + 0.5F);

	m_length = 2U * length;
	m_data   = new float[m_length];

	::memset(m_data, 0x00, m_length * sizeof(float));

	CNCO tone(sampleRate, freq);
	tone.getAudio(m_data, length);

	// Calculate the length of the raised cosine shaping section
	unsigned int rcLen = RC_LEN * sampleRate / 1000U;

	// Shape the start and end of the tone
	for (unsigned int i = 0U; i < rcLen; i++) {
		float ampl = 0.5F * (1.0F - float(::cos(M_PI * (float(i) / float(rcLen)))));

		m_data[i] *= ampl;
		m_data[length - 1U - i] *= ampl;
	}
}

CTimeoutTones::~CTimeoutTones()
{
	delete[] m_data;
}

void CTimeoutTones::getAudio(float* audio, unsigned int length, float amplitude)
{
	assert(audio != NULL);

	for (unsigned int i = 0U; i < length; i++) {
		audio[i] = m_data[m_current++] * amplitude;

		if (m_current >= m_length)
			m_current = 0U;
	}
}
