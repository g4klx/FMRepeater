/*
 *	Copyright (C) 2009,2013,2018 by Jonathan Naylor, G4KLX
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

#ifndef	CWKeyer_H
#define	CWKeyer_H

#include <string>

class CCWKeyer {
public:
	CCWKeyer(const std::string& text, unsigned int speed, unsigned int freq, unsigned int sampleRate);
	~CCWKeyer();

	unsigned int getAudio(float* audio, unsigned int length, float amplitude);

	bool isEmpty() const;

	void reset();

private:
	float*       m_data;
	unsigned int m_length;
	unsigned int m_current;

	std::string charToCW(char c) const;
};

#endif
