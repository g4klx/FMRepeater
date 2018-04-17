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

#include <wiringPi.h>

#include <cstdio>

const int BASE_PIN  = 6;
const int PTT_PIN   = 12;
const int PKSQL_PIN = 5;
const int SQL_PIN   = 25;

CUDRCController::CUDRCController() :
m_outp1(false),
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

	::pinMode(SQL_PIN,   INPUT);
	::pinMode(PKSQL_PIN, INPUT);

	// Set pull ups on the input pins
	::pullUpDnControl(SQL_PIN,   PUD_UP);
	::pullUpDnControl(PKSQL_PIN, PUD_UP);

	::pinMode(PTT_PIN,  OUTPUT);
	::pinMode(BASE_PIN, OUTPUT);

	::digitalWrite(PTT_PIN,  HIGH);
	::digitalWrite(BASE_PIN, HIGH);

	return true;
}

void CUDRCController::getDigitalInputs(bool& inp1, bool& inp2)
{
	inp1 = ::digitalRead(SQL_PIN) == LOW;

	inp2 = ::digitalRead(PKSQL_PIN) == LOW;
}

void CUDRCController::setDigitalOutputs(bool outp1, bool outp2, bool outp3)
{
	if (outp1 != m_outp1) {
		::digitalWrite(PTT_PIN, outp1 ? LOW : HIGH);
		m_outp1 = outp1;
	}

	if (outp3 != m_outp3) {
		::digitalWrite(BASE_PIN, outp3 ? LOW : HIGH);
		m_outp3 = outp3;
	}
}

void CUDRCController::close()
{
	::digitalWrite(PTT_PIN,  HIGH);
	::digitalWrite(BASE_PIN, HIGH);
}

#else

CUDRCController::CUDRCController() :
m_outp1(false),
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

