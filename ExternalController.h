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

#ifndef	ExternalController_H
#define	ExternalController_H

#include "HardwareController.h"

class CExternalController {
public:
	CExternalController(IHardwareController* controller, bool pttInvert = false, bool squelchInvert = false);
	~CExternalController();

	bool open();

	bool getSquelch();
	bool getDisable();

	void setTransmit(bool value);
	void setHeartbeat();
	void setActive(bool value);

	void close();

private:
	IHardwareController* m_controller;
	bool                 m_pttInvert;
	bool                 m_squelchInvert;
	bool                 m_heartbeat;
};

#endif
