/*
 *  temperature.cpp
 *  Wadiscape 5
 *
 *  Created by Forschung on 2008-07-16.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "temperature.h"


CLIMATE::CLIMATE(void)
{
	theColdestDay = 15.625; // for setting X-value of minimum of cos function; this shifts the cos function by a half month (30.5 d) plus half a day.
}

CLIMATE::CLIMATE(float T_mean, float T_range, float phi_deg)
{	theMeanT = T_mean;
	theRangeT = T_range; 
	theLatitude_deg = phi_deg;
	theColdestDay = 15.625; // for setting X-value of minimum of cos function
}

cPCEnJaqob::cPCEnJaqob (void)
{
	theMeanT = 20.5;
	theRangeT = 7.5;
}

short int CLIMATE::set_day(ushort d)
{	short int jd = (d + 212) % days_in_year;
	calc_temperature(d);
	return jd;
}

void CLIMATE::calc_temperature(ushort d)
{
	float TIndex = cos( (d+theColdestDay )/float(days_in_year) * pi2 );
	theT = TIndex * theRangeT + theMeanT;
}

float CLIMATE::get_temperature() {return theT;}
