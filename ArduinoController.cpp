/*
 *	Copyright (C) 2013,2018 by Jonathan Naylor, G4KLX
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

#include "ArduinoController.h"

const char IN_PORT1 = 0x01U;
const char IN_PORT2 = 0x02U;
const char IN_PORT3 = 0x04U;
const char IN_PORT4 = 0x08U;
const char IN_PORT5 = 0x10U;

const char OUT_PORT1 = 0x01U;
const char OUT_PORT2 = 0x02U;
const char OUT_PORT3 = 0x04U;
const char OUT_PORT4 = 0x08U;
const char OUT_PORT5 = 0x10U;
const char OUT_PORT6 = 0x20U;
const char OUT_PORT7 = 0x40U;
const char OUT_PORT8 = 0x80U;


CArduinoController::CArduinoController(const std::string& port) :
CThread(),
m_serial(port, SERIAL_19200),
m_squelch(false),
m_disable(false),
m_transmit(false),
m_heartbeat(false),
m_active(false),
m_kill(false)
{
}

CArduinoController::~CArduinoController()
{
}

bool CArduinoController::open()
{
	bool ret = m_serial.open();
	if (!ret)
		return false;

	unsigned char buffer;
	while (m_serial.read(&buffer, 1U) == 1)
		;

	run();

	return true;
}

void CArduinoController::entry()
{
	while (!m_kill) {
		unsigned char in;
		int ret = m_serial.read(&in, 1U);
		if (ret == 1) {
			m_squelch = (in & IN_PORT1) == IN_PORT1;
			m_disable = (in & IN_PORT2) == IN_PORT2;
		}

		sleep(10U);
	}

	m_serial.close();
}

bool CArduinoController::getSquelch()
{
	return m_squelch;
}

bool CArduinoController::getDisable()
{
	return m_disable;
}

void CArduinoController::setTransmit(bool value)
{
	m_transmit = value;

	setOutputs();
}

void CArduinoController::setHeartbeat(bool value)
{
	m_heartbeat = value;

	setOutputs();
}

void CArduinoController::setActive(bool value)
{
	m_active = value;

	setOutputs();
}

void CArduinoController::setOutputs()
{
	unsigned char out = 0x00U;
	if (m_transmit)
		out |= OUT_PORT1;
	if (m_heartbeat)
		out |= OUT_PORT2;
	if (m_active)
		out |= OUT_PORT3;

	m_serial.write(&out, 1U);
}

void CArduinoController::close()
{
	m_kill = true;

	wait();
}
