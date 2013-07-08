/*
 *  annrain.cpp
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-11.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include "annrain.h"

ANNRAIN::ANNRAIN(climate_names climate, scenario_names scenario)
{
	switch (climate) {
		case arid: MAP_CT = 100.0; break;
		case semiarid: MAP_CT = 300.0; break;
		case drymed: MAP_CT = 450.0; break;
		case typmed: MAP_CT = 600.0; break;
		case mesicmed: MAP_CT = 800.0; break;
	}
	
	scale_intercept_CT = 10;
	scale_slope_CT = 0.051;
	
	float MAP = 0.0;
	float scale_intercept = 0.0;
	float scale_slope = 0.0;
	
	switch (scenario) {
		case CT: MAP = MAP_CT;
			scale_intercept = scale_intercept_CT; scale_slope = scale_slope_CT; break;
		case A2: MAP = 0.87 * MAP_CT; 
			scale_intercept = 2.2; scale_slope = 0.143; break;
		case B2: MAP = -8 + 1.09 * MAP_CT; 
			scale_intercept = 8; scale_slope = 0.06; break;
		default: std::cerr << "no gamma parameters defined for this climate scenario" << std::endl;
	}
	
	if (years_trans) {
	MAP_step = (MAP - MAP_CT)/years_trans;
	scale_intercept_step = (scale_intercept - scale_intercept_CT)/years_trans;
	scale_slope_step = (scale_slope - scale_slope_CT)/years_trans;
	}
}

float ANNRAIN::get_annual_rain (int yy)
{
	//  MK 2006-06-28, 2006-10-16: Regen aus Gamma-verteilung (s.Ben-Gai et al. 1998). Die 
	//  aufbereitet von Alpert et al. [2005, pers. comm] fuer GLOWA, heruntergeladen
	//  2006-06-27). Szenario A1: 1961-1989, A2, B2
	
	float yMAP = MAP_CT;
	float yScale_intercept = scale_intercept_CT;
	float yScale_slope = scale_slope_CT;
	
	if (yy < years_0+years_CT)
		yy = 0;
	else if (yy < years_0+years_CT+years_trans)
		yy -= years_0+years_CT;
	else
		yy = years_trans;
	
	yMAP = MAP_CT + yy * MAP_step;
	yScale_intercept = scale_intercept_CT + yy * scale_intercept_step;
	yScale_slope = scale_slope_CT + yy * scale_slope_step;
	
	float yScale = yScale_intercept + yScale_slope * yMAP;
	float shape = yMAP/yScale;
	
	return Zzo.gamma(shape, 1/yScale);
}
