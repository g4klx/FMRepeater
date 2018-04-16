/*
 *	Copyright (C) 2009,2010,2018 by Jonathan Naylor, G4KLX
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

#include "AudioDelay.h"

#include <cstdio>
#include <cassert>
#include <cstring>

CAudioDelay::CAudioDelay(unsigned int delay) :
m_delay(2U * delay),
m_in(delay),
m_out(0U),
m_bufList(NULL)
{
	assert(delay > 0U);

	m_bufList = new float[m_delay];
	::memset(m_bufList, 0x00, m_delay * sizeof(float));
}

CAudioDelay::~CAudioDelay()
{
	delete[] m_bufList;
}

void CAudioDelay::delay(float* audio, unsigned int length)
{
	assert(audio != NULL);
	assert(length > 0U);

	for (unsigned int i = 0U; i < length; i++) {
		m_bufList[m_in++] = audio[i];
		audio[i] = m_bufList[m_out++];

		if (m_in >= m_delay)
			m_in = 0U;
		if (m_out >= m_delay)
			m_out = 0U;
	}
}
