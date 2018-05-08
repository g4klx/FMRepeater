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

CUDRCController::CUDRCController()
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

	return true;
}

bool CUDRCController::getSquelch()
{
	return ::digitalRead(SQL_PIN) == LOW;
}

bool CUDRCController::getDisable()
{
	return ::digitalRead(PKSQL_PIN) == LOW;
}

void CUDRCController::setTransmit(bool value)
{
	::digitalWrite(PTT_PIN, value ? LOW : HIGH);
}

void CUDRCController::setActive(bool value)
{
	::digitalWrite(BASE_PIN, value ? LOW : HIGH);
}

void CUDRCController::setHeartbeat(bool)
{
}

void CUDRCController::close()
{
}

#else

CUDRCController::CUDRCController()
{
}

CUDRCController::~CUDRCController()
{
}

bool CUDRCController::open()
{
	return true;
}

bool CUDRCController::getSquelch()
{
	return false;
}

bool CUDRCController::getDisable()
{
	return false;
}

void CUDRCController::setTransmit(bool value)
{
}

void CUDRCController::setHeartbeat(bool value)
{
}

void CUDRCController::setActive(bool value)
{
}

void CUDRCController::close()
{
}

#endif

