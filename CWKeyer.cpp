/*
 *	Copyright (C) 2009,2012,2013,2018 by Jonathan Naylor, G4KLX
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

#include "CWKeyer.h"

#include <cstdio>
#include <cassert>
#include <cstring>
#include <cmath>

const struct SSymbols {
	const char  c;
	const char* symbol;
} cwTable[] = {
	{'A', ".-"},
	{'B', "-..."},
	{'C', "-.-."},
	{'D', "-.."},
	{'E', "."},
	{'F', "..-."},
	{'G', "--."},
	{'H', "...."},
	{'I', ".."},
	{'J', ".---"},
	{'K', "-.-"},
	{'L', ".-.."},
	{'M', "--"},
	{'N', "-."},
	{'O', "---"},
	{'P', ".--."},
	{'Q', "--.-"},
	{'R', ".-."},
	{'S', "..."},
	{'T', "-"},
	{'U', "..-"},
	{'V', "...-"},
	{'W', ".--"},
	{'X', "-..-"},
	{'Y', "-.--"},
	{'Z', "--.."},
	{'0', "-----"},
	{'1', ".----"},
	{'2', "..---"},
	{'3', "...--"},
	{'4', "....-"},
	{'5', "....."},
	{'6', "-...."},
	{'7', "--..."},
	{'8', "---.."},
	{'9', "----."},
	{'/', "-..-."},
	{'=', "-...-"},
	{',', "--..--"},
	{'?', "..--.."},
	{' ', "  "},
	{'$', ""}
};

const unsigned int RC_LEN = 5U;			// The length of the raised cosine in ms

CCWKeyer::CCWKeyer(const std::string& text, unsigned int speed, unsigned int freq, unsigned int sampleRate) :
m_data(NULL),
m_length(0U),
m_current(0U)
{
	assert(speed > 0U);
	assert(freq > 0U);
	assert(sampleRate > 0U);

	if (text.empty())
		return;

	std::string cw = " ";
	for (unsigned int i = 0U; i < text.size(); i++) {
		cw += charToCW(text.at(i));
		cw += " ";
	}
	cw += "  ";

	for (unsigned int i = 0U; i < cw.size(); i++) {
		char c = cw.at(i);

		switch (c) {
			case ' ':
				m_length += 2U;
				break;
			case '-':
				m_length += 4U;
				break;
			case '.':
				m_length += 2U;
				break;
		}
	}

	float dotLenS = 1.2F / float(speed);
	unsigned int dotLenSamples = (unsigned int)(float(sampleRate) * dotLenS + 0.5F);

	unsigned int dotLen  = dotLenSamples * 1U;
	unsigned int dashLen = dotLenSamples * 3U;

	m_length *= dotLenSamples;

	m_data = new float[m_length];

	unsigned int sqLen = (sampleRate / freq) / 2U;

	float* dot  = new float[dotLen];
	float* dash = new float[dashLen];

	bool high = false;
	for (unsigned int i = 0U; i < dashLen; i++) {
		if ((i % sqLen) == 0U)
			high = !high;

		dash[i] = high ? 1.0F : -1.0F;
		if (i < dotLen)
			dot[i] = high ? 1.0F : -1.0F;
	}

	// Calculate the length of the raised cosine shaping section
	unsigned int rcLen = RC_LEN * sampleRate / 1000U;

	// Shape the start and end of the tones
	for (unsigned int i = 0U; i < rcLen; i++) {
		float ampl = 0.5F * (1.0F - float(::cos(M_PI * (float(i) / float(rcLen)))));

		dot[i]  *= ampl;
		dash[i] *= ampl;

		dot[dotLen - 1U - i]   *= ampl;
		dash[dashLen - 1U - i] *= ampl;
	}

	// Clear the output buffer to ensure silence between the symbols
	::memset(m_data, 0x00, m_length * sizeof(float));

	unsigned int pos = 0U;
	for (unsigned int i = 0U; i < cw.size(); i++) {
		char c = cw.at(i);

		switch (c) {
			case ' ':
				pos += dotLenSamples * 2U;
				break;
			case '.':
				::memcpy(m_data + pos, dot, dotLen * sizeof(float));
				pos += dotLenSamples * 2U;
				break;
			case '-':
				::memcpy(m_data + pos, dash, dashLen * sizeof(float));
				pos += dotLenSamples * 4U;
				break;
		}
	}

	delete[] dot;
	delete[] dash;
}

CCWKeyer::~CCWKeyer()
{
	delete[] m_data;
}

bool CCWKeyer::isEmpty() const
{
	return m_length == 0U;
}

unsigned int CCWKeyer::getAudio(float* audio, unsigned int length, float amplitude)
{
	assert(audio != NULL);

	if (m_length == 0U)
		return 0U;

	unsigned int n = 0U;
	while (n < length && m_current < m_length)
		audio[n++] += m_data[m_current++] * amplitude;

	return n;
}

void CCWKeyer::reset()
{
	m_current = 0U;
}

std::string CCWKeyer::charToCW(char c) const
{
	for (const SSymbols* s = cwTable; s->c != '$'; s++) {
		if (s->c == c)
			return s->symbol;
	}

	return "";
}
