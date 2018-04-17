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
m_controller(controller),
m_pttInvert(pttInvert),
m_squelchInvert(squelchInvert),
m_heartbeat(false)
{
	assert(controller != NULL);
}

CExternalController::~CExternalController()
{
}

bool CExternalController::open()
{
	bool ret = m_controller->open();
	if (!ret)
		return false;

	setTransmit(false);
	setActive(false);

	return true;
}

bool CExternalController::getSquelch()
{
	assert(m_controller != NULL);

	bool squelch = m_controller->getSquelch();

	if (m_squelchInvert)
		return !squelch;
	else
		return squelch;
}

bool CExternalController::getDisable()
{
	assert(m_controller != NULL);

	return m_controller->getDisable();
}

void CExternalController::setTransmit(bool value)
{
	assert(m_controller != NULL);

	if (m_pttInvert)
		value = !value;

	m_controller->setTransmit(value);
}

void CExternalController::setHeartbeat()
{
	assert(m_controller != NULL);

	m_controller->setHeartbeat(m_heartbeat);

	m_heartbeat = !m_heartbeat;
}

void CExternalController::setActive(bool value)
{
	assert(m_controller != NULL);

	m_controller->setActive(value);
}

void CExternalController::close()
{
	assert(m_controller != NULL);

	setTransmit(false);
	setActive(false);

	m_controller->close();

	delete m_controller;
	m_controller = NULL;
}
