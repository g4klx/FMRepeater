/*
 *   Copyright (C) 2009,2018 by Jonathan Naylor G4KLX
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

#ifndef	NCO_H
#define	NCO_H

class CNCO {
public:
	CNCO(unsigned int sampleRate, float freq);
	~CNCO();

	void getAudio(float* audio, unsigned int length, float amplitude = 1.0F);

private:
	float*       m_audio;
	unsigned int m_length;
	unsigned int m_position;
};

#endif
