/*
 *   Copyright (C) 2006-2009,2012,2013,2015,2018 by Jonathan Naylor G4KLX
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

#ifndef RingBuffer_H
#define RingBuffer_H

#include <cassert>

enum RBSTATE {
	RBSTATE_EMPTY,
	RBSTATE_FULL,
	RBSTATE_DATA
};

template<class T> class CRingBuffer {
public:
	CRingBuffer(unsigned int length) :
	m_length(length),
	m_buffer(NULL),
	m_iPtr(0U),
	m_oPtr(0U),
	m_state(RBSTATE_EMPTY)
	{
		assert(length > 0U);

		m_buffer = new T[length];

		::memset(m_buffer, 0x00, length * sizeof(T));
	}

	~CRingBuffer()
	{
		delete[] m_buffer;
	}

	unsigned int addData(const T* buffer, unsigned int nSamples)
	{
		unsigned int space = freeSpace();

		if (nSamples > space)
			return 0U;

		m_state = (nSamples == space) ? RBSTATE_FULL : RBSTATE_DATA;

		for (unsigned int i = 0U; i < nSamples; i++) {
			m_buffer[m_iPtr++] = buffer[i];

			if (m_iPtr == m_length)
				m_iPtr = 0U;
		}

		return nSamples;
	}

	unsigned int getData(T* buffer, unsigned int nSamples)
	{
		unsigned int space = dataSpace();

		if (space < nSamples)
			return 0U;

		m_state = (nSamples == space) ? RBSTATE_EMPTY : RBSTATE_DATA;

		for (unsigned int i = 0U; i < nSamples; i++) {
			buffer[i] = m_buffer[m_oPtr++];

			if (m_oPtr == m_length)
				m_oPtr = 0U;
		}

		return nSamples;
	}

	unsigned int peek(T* buffer, unsigned int nSamples)
	{
		unsigned int space = dataSpace();

		if (space < nSamples)
			return 0U;

		unsigned int ptr = m_oPtr;
		for (unsigned int i = 0U; i < nSamples; i++) {
			buffer[i] = m_buffer[ptr++];

			if (ptr == m_length)
				ptr = 0U;
		}

		return nSamples;
	}

	void clear()
	{
		m_iPtr  = 0U;
		m_oPtr  = 0U;
		m_state = RBSTATE_EMPTY;

		::memset(m_buffer, 0x00, m_length * sizeof(T));
	}

	unsigned int freeSpace()
	{
		if (isEmpty())
			return m_length;

		if (isFull())
			return 0U;

		if (m_oPtr > m_iPtr)
			return m_oPtr - m_iPtr;

		return m_length - (m_iPtr - m_oPtr);
	}

	unsigned int dataSpace()
	{
		if (isEmpty())
			return 0U;

		if (isFull())
			return m_length;

		if (m_iPtr >= m_oPtr)
			return m_iPtr - m_oPtr;

		return m_length - (m_oPtr - m_iPtr);
	}

	bool isEmpty()
	{
		return m_state == RBSTATE_EMPTY;
	}

	bool isFull()
	{
		return m_state == RBSTATE_FULL;
	}

private:
	unsigned int          m_length;
	T*                    m_buffer;
	volatile unsigned int m_iPtr;
	volatile unsigned int m_oPtr;
	volatile RBSTATE      m_state;
};

#endif
