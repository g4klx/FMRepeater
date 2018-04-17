/*
 *	Copyright (C) 2012,2013,2015,2018 by Jonathan Naylor, G4KLX
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

#include "UDRCController.h"

#if defined(UDRC)

#include "wiringPi.h"

#include <cstdio>

CUDRCController::CUDRCController() :
m_outp1(false),
m_outp2(false),
m_outp3(false)
{
}

CUDRCController::~CUDRCController()
{
}

bool CUDRCController::open()
{
	bool ret = ::wiringPiSetup() != -1;
	if (!ret) {
		::fprintf(stderr, "Unable to initialise wiringPi\n");
		return false;
	}

	::pinMode(8, INPUT);
	::pinMode(9, INPUT);

	// Set pull ups on the input pins
	::pullUpDnControl(8, PUD_UP);
	::pullUpDnControl(9, PUD_UP);

	::pinMode(12, OUTPUT);
	::pinMode(13, OUTPUT);
	::pinMode(14, OUTPUT);

	setDigitalOutputs(false, false, false);

	return true;
}

void CUDRCController::getDigitalInputs(bool& inp1, bool& inp2)
{
	inp1 = ::digitalRead(8) == LOW;

	inp2 = ::digitalRead(9) == LOW;
}

void CUDRCController::setDigitalOutputs(bool outp1, bool outp2, bool outp3)
{
	if (outp1 != m_outp1) {
		::digitalWrite(12, outp1 ? HIGH : LOW);
		m_outp1 = outp1;
	}

	if (outp2 != m_outp2) {
		::digitalWrite(13, outp2 ? HIGH : LOW);
		m_outp2 = outp2;
	}

	if (outp3 != m_outp3) {
		::digitalWrite(14, outp3 ? HIGH : LOW);
		m_outp3 = outp3;
	}
}

void CUDRCController::close()
{
}

#else

CUDRCController::CUDRCController() :
m_outp1(false),
m_outp2(false),
m_outp3(false)
{
}

CUDRCController::~CUDRCController()
{
}

bool CUDRCController::open()
{
	return true;
}

void CUDRCController::getDigitalInputs(bool& inp1, bool& inp2)
{
}

void CUDRCController::setDigitalOutputs(bool outp1, bool outp2, bool outp3)
{
}

void CUDRCController::close()
{
}

#endif

