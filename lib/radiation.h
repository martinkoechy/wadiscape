/*
 *  radiation.h
 *  radiation
 *
 *  Created by Martin Köchy on 2008-08-20.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *  
 * 
 *  The functions in this unit copy the equations reported in Allen et al. (2006) and adjusted as indicated in the 
 *  descriptions of the individual functions.
 *  
 *  Allen, R. G., R. Trezza, and M. Tasumi. 2006. Analytical integrated functions for daily solar radiation on slopes. 
 *  Agricultural and Forest Meteorology 139:55-73.
 */


#ifndef RADIATION_H
#define RADIATION_H


class SOLAR_RADIATION
{
public:
	SOLAR_RADIATION(float latitude_deg);
	~SOLAR_RADIATION(){};
	
	 /// calculates the total extraterrestrial radiation for a given of the day
	float get_daily_radiation(float slope, float aspect, short day_of_year);
	void  set_daily_parameters(short day_of_year);

private:
	short day_of_year;
	float s; ///< slope in radians
	float gamma; ///< aspect in radians
	float delta; ///< declination of the earth, in radians
	float rel_solar_distance; ///< relative distance between sun and earth
	const float phi; ///< latitude in radians, northern hemisphere has positive values
	const float solar_constant; ///< global solar constant (W/m2)
	
	void  set_delta(void); ///< calculate the declination of the earth
	float calc_rel_solar_distance(void); ///< calculate the seasonal relative distance between sun and earth
	void  set_omega_sunset(float& omega_rise, float& omega_set, float& omega_rise2, float& omega_set2); ///< set the times of sunrise and sunset
	float get_period_radiation(float omega1, float omega2);
	float calculate_cos_theta(float omega); ///< calculate the cosine of the altitude of the sun or incidence angle, this is the angle between zenith and the sun
	float integrate_cos_theta_time(float omega1, float omega2); ///< do the integration of cos theta over time
};

# endif
