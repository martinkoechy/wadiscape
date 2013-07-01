/*
 *  temperature.h
 *  Wadiscape 5
 *
 *  Created by Forschung on 2008-07-16.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "main.h"

#ifndef TEMPERATURE_H
#define TEMPERATURE_H

class CLIMATE
{
protected:
	float theMeanT;
	float theRangeT;
	float theColdestDay;
	float theLatitude_deg;
	float theR; ///< the mean daily extraterrestrial radiation
	float theT; ///< the mean daily temperature

public:
	CLIMATE(void);
    CLIMATE(float meanT, float rangeT, float latitude);
	virtual ~CLIMATE() {};
	float getMeanT (void) const {return theMeanT;}
	float getRangeT (void) const {return theRangeT;}
	float getColdestDay (void) const {return theColdestDay;}
	
	short int set_day(ushort d);
	void calc_temperature(ushort d);
	void calc_radiation(ushort jd); // not used
	 float get_temperature(void);
	 float get_pot_evapo_flat_mm(void);
	
};

class cPCEnJaqob : public CLIMATE
{
public:
	cPCEnJaqob(void);
	~cPCEnJaqob() {};
};

#endif
