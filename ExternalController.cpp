/*
 *	Copyright (C) 2009,2010,2013,2018 by Jonathan Naylor, G4KLX
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

#include "ExternalController.h"

#include "FMDefines.h"

#include <cstdio>
#include <cassert>

CExternalController::CExternalController(IHardwareController* controller, bool pttInvert, bool squelchInvert) :
CThread(),
m_controller(controller),
m_pttInvert(pttInvert),
m_squelchInvert(squelchInvert),
m_squelch(false),
m_disable(false),
m_heartbeat(false),
m_active(false),
m_transmit(false),
m_kill(false)
{
	assert(controller != NULL);

	if (m_squelchInvert)
		m_squelch = true;

	if (m_pttInvert)
		m_transmit = true;
}

CExternalController::~CExternalController()
{
	delete m_controller;
}

bool CExternalController::open()
{
	bool res = m_controller->open();
	if (!res)
		return false;

	run();

	return true;
}

void CExternalController::entry()
{
	assert(m_controller != NULL);

	while (!m_kill) {
		m_controller->setDigitalOutputs(m_transmit, m_heartbeat, m_active);
		m_controller->getDigitalInputs(m_squelch, m_disable);

		sleep(ANALOGUE_FRAME_TIME_MS / 2U);
	}

	if (m_pttInvert)
		m_controller->setDigitalOutputs(true, false, false);
	else
		m_controller->setDigitalOutputs(false, false, false);
	m_controller->getDigitalInputs(m_squelch, m_disable);

	sleep(ANALOGUE_FRAME_TIME_MS * 3U);

	if (m_pttInvert)
		m_controller->setDigitalOutputs(true, false, false);
	else
		m_controller->setDigitalOutputs(false, false, false);
	m_controller->getDigitalInputs(m_squelch, m_disable);

	m_controller->close();
}

bool CExternalController::getSquelch() const
{
	if (m_squelchInvert)
		return !m_squelch;
	else
		return m_squelch;
}

bool CExternalController::getDisable() const
{
	return m_disable;
}

void CExternalController::setTransmit(bool value)
{
	if (m_pttInvert)
		value = !value;

	m_transmit = value;
}

void CExternalController::setHeartbeat()
{
	m_heartbeat = !m_heartbeat;
}

void CExternalController::setActive(bool value)
{
	m_active = value;
}

void CExternalController::close()
{
	m_kill = true;

	wait();
}

