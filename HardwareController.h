/*
 *	Copyright (C) 2009,2013,2018 by Jonathan Naylor, G4KLX
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

#ifndef	HardwareController_H
#define	HardwareController_H

class IHardwareController {
public:
	virtual ~IHardwareController() = 0;

	virtual bool open() = 0;

	virtual void getDigitalInputs(bool& inp1, bool& inp2) = 0;

	virtual void setDigitalOutputs(bool outp1, bool outp2, bool outp3) = 0;

	virtual void close() = 0;

private:
};

#endif
