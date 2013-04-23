/*
 *  annrain.h
 *  Wadiscape5
 *
 *  Created by Martin Köchy on 2008-06-11.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "main.h"

#ifndef ANNRAIN_H
#define ANNRAIN_H

/// Time series of annual rain are generated from gamma distributions.
/**
This class was used in WADISCAPE 3.x for generating annual values. Now it is
 used only for seeding the ReGen-Algorithm.
 */
class ANNRAIN
{
protected:
	float MAP_CT;
	float MAP_step;
	float scale_intercept_CT;
	float scale_slope_CT;
	float scale_intercept_step;
	float scale_slope_step;
	RNG Zzo;
	
public:
		ANNRAIN(climate_names, scenario_names);
	~ANNRAIN(){};
	float get_annual_rain(int simulation_year); //< needs IPTC gamma values for each scenario
};

#endif
