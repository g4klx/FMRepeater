/*
 *	Copyright (C) 2012,2015,2018 by Jonathan Naylor, G4KLX
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

#ifndef	UDRCController_H
#define	UDRCController_H

#include "HardwareController.h"

class CUDRCController : public IHardwareController {
public:
	CUDRCController();
	virtual ~CUDRCController();

	virtual bool open();

	virtual bool getSquelch();
	virtual bool getDisable();

	virtual void setTransmit(bool value);
	virtual void setHeartbeat(bool value);
	virtual void setActive(bool value);

	virtual void close();

private:
};

#endif

