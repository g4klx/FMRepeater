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

#ifndef	ArduinoController_H
#define	ArduinoController_H

#include "SerialDataController.h"
#include "HardwareController.h"
#include "Thread.h"

#include <string>

class CArduinoController : public IHardwareController, public CThread {
public:
	CArduinoController(const std::string& port);
	virtual ~CArduinoController();

	virtual bool open();

	virtual void entry();

	virtual bool getSquelch();
	virtual bool getDisable();

	virtual void setTransmit(bool value);
	virtual void setHeartbeat(bool value);
	virtual void setActive(bool value);

	virtual void close();

private:
	CSerialDataController m_serial;
	bool                  m_squelch;
	bool                  m_disable;
	bool                  m_transmit;
	bool                  m_heartbeat;
	bool                  m_active;
	bool                  m_kill;

	void setOutputs();
};

#endif
