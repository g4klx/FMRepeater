/*
 *	Copyright (C) 2009,2011,2018 by Jonathan Naylor, G4KLX
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

#ifndef	Goertzel_H
#define	Goertzel_H

#include "Utils.h"

class CGoertzel {
public:
	CGoertzel(unsigned int sampleRate, float freq, unsigned int n, float threshold);
	~CGoertzel();

	void setThreshold(float threshold);

	float getResult() const;

	void process(const float* data, unsigned int length);

	TRISTATE getDetected();

private:
	float        m_freq;		// XXX
	unsigned int m_n;
	float        m_threshold;
	float        m_coeff;
	float        m_q1;
	float        m_q2;
	float        m_value;
	unsigned int m_count;
	bool         m_result;
};

#endif
