/*
 *  radiation.cpp
 *  radiation
 *
 *  Created by Martin Köchy on 2008-08-20.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include "main.h"
#include "radiation.h"
#include <cmath>
#include <cstdlib>

/**
Latitude converted to radians and adjusted for excentricity of the earth, http://wskw.de/service/solar/hinweis.html
*/
SOLAR_RADIATION::SOLAR_RADIATION(float latitude_deg): 
	phi((latitude_deg + 0.1924 * sin(2*latitude_deg*qpi2_360))*qpi2_360), solar_constant(1367.0)
{
}


void  SOLAR_RADIATION::set_daily_parameters(short doy)
{
	day_of_year = doy;
	rel_solar_distance = calc_rel_solar_distance();
	set_delta(); ///< seasonal declination of the sun
}

/**
Based on Duffie and Beckman 1991 cited in Allen et al. 2006
*/
float SOLAR_RADIATION::calc_rel_solar_distance(void)
{
	return 1 + 0.033 * cos(day_of_year * pi * q1_365); // Duffie and Beckman 1991	
}


/** 
More exact but still only approximate, from Partridge, G. W. and Platt, C. M. R. 1976. Radiative Processes in Meteorology
 and Climatology. Cited on http://solardat.uoregon.edu/SolarRadiationBasics.html 

float SOLAR_RADIATION::calc_rel_solar_distance(void)
{
	float b = pi2 * day_of_year * q1_365;
	return 1.00011
	+ 0.034221 * cos(b)
	+ 0.001280 * sin(b)
	+ 0.000719 * cos(2*b)
	+ 0.000077 * sin(2*b);
}
*/


/**
Allen et al. 2006, Eq. 6  Unit of return value is MJ/(m2 d).
 */
float SOLAR_RADIATION::get_period_radiation(float omega1, float omega2)
{
	float integrated_cos_theta = integrate_cos_theta_time(omega1, omega2); ///< angle of incidence
	float seconds = (omega2 - omega1)/pi * 12 * 60 * 60;
		
	return solar_constant * seconds  * 1E-6 * q1_pi * rel_solar_distance * integrated_cos_theta;
}


/**
Allen et al. 2006, Eq. 6 and Appendix
*/
float SOLAR_RADIATION::get_daily_radiation(float slope, float aspect, short day_of_year)
{	s = slope;
	gamma = aspect;

	float dailyR = 0.0;	///< sum of daily radiation
	
	float omega_rise = 0.0, omega_set = 0.0, omega_rise2 = 0.0, omega_set2  = 0.0;	///< hour angles of sunrise and sunset
	set_omega_sunset(omega_rise, omega_set, omega_rise2, omega_set2);
	
	if(omega_rise2 == 0.0 & omega_set2 == 0.0) // sun shines only once per day
		dailyR = get_period_radiation(omega_rise, omega_set);
	else
		dailyR = get_period_radiation(omega_rise, omega_set2) + get_period_radiation(omega_rise2, omega_set);
	
	return dailyR;
}

/**
Duffie and Beckman (1980) [quoted from WATFLOOD web site, http://www.civil.uwaterloo.ca/Watflood/intro/intro.htm],
													  and http://solardat.uoregon.edu/SolarRadiationBasics.html
*/
void SOLAR_RADIATION::set_delta(void)
{	// 23.45° ≘ 0.4093 rad
	delta = 0.4093 * sin(pi2 * (284 + day_of_year) * q1_365);
}


/**
Allen et al. 2006, Equation 3 and 4
 */
float SOLAR_RADIATION::calculate_cos_theta(float omega)
{
	float cos_theta = 0.0;
	
	if (s == 0.0) {// horizontal surface, Eq. 4
		cos_theta = sin(delta) * sin(phi) + cos(delta) * cos(phi) * cos(omega);
	}
	else { // tilted and rotated surface, Eq. 3
		cos_theta = sin(delta) * sin(phi) * cos(s)
		- sin(delta) * cos(phi) * sin(s) * cos (gamma)
		+ cos(delta) * cos(phi) * cos(s) * cos(omega)
		+ cos(delta) * sin(phi) * sin (s) * cos(gamma) * cos(omega)
		+ cos(delta) * sin(gamma) * sin(s) * sin(omega);
	}
	return cos_theta;
}


/**
Allen et al. 2006, Eq. 9-13
*/
void SOLAR_RADIATION::set_omega_sunset(float& omega_rise, float& omega_set, float& omega_rise2, float& omega_set2)
{	bool polarchecked = false;
	
	float omega1_24 = -pi, omega2_24 = pi;
	
	// check integration limits (Appendix A) ----
	// ---- step A ---- polar regions with >24 h day or night
	if (phi > 0) {// northern hemisphere
		if (delta + phi > pi*0.5) {
			omega1_24 = -pi;
			omega2_24 = pi; // sun never sets
			polarchecked = true;
		}
		if (delta - phi > pi*0.5) {
			omega1_24 = 0.0; // sun never rises
			omega2_24 = 0.0;
			polarchecked = true;
		}
	}
	else { // southern hemisphere
		if (delta + phi < -pi*0.5) {
			omega1_24 = -pi;
			omega2_24 = pi; // sun never sets
			polarchecked = true;
		}
		if (delta - phi < -pi*0.5) {
			omega1_24 = 0.0;
			omega2_24 = 0.0; // sun never rises
			polarchecked = true;
		}
	}
	
	if(!polarchecked) {
		float omega_s =  acos(-tan(delta) * tan(phi)); // horizontal surface, Eq. 8
		if (s == 0.0) {
			omega1_24 = -omega_s;
			omega2_24 =  omega_s;
		}
		else { // tilted and rotated surface, Eq. 10, 11, 12
			float a = sin(delta) * cos(phi) * sin(s) * cos(gamma) - sin(delta) * sin(phi) * cos(s); // Eq. 11a
			float b = cos(delta) * cos(phi) * cos(s) + cos(delta) * sin(phi) * sin(s) * cos(gamma); // Eq. 11b
			float c = cos(delta) * sin(s) * sin(gamma); // Eq. 11c
			
			float rootexp = sqrt(b*b + c*c - a*a); // Eq. 13, 
			if (std::isnan(rootexp)) rootexp = 0.0001; // D1
			float sin_omega1_24 =  (a*c - b*rootexp)/(b*b + c*c); // Eq. 13a
			float sin_omega2_24 =  (a*c + b*rootexp)/(b*b + c*c); // Eq. 13b
			
			omega1_24 = asin(sin_omega1_24); // Eq. 13
			omega2_24 = asin(sin_omega2_24);
			
			float sin_omega1_24_lim = max(min(sin_omega1_24, 1.0F), -1.0F); //step A2Bi
			float sin_omega2_24_lim = max(min(sin_omega2_24, 1.0F), -1.0F); //step A2Bi
			float omega1_24_lim = asin(sin_omega1_24_lim); // Eq. 13
			float omega2_24_lim = asin(sin_omega2_24_lim);
			
			//---- step B ----
			// cosine of potential solar incidence angles at sunrise
			float cos_theta_omega1 = calculate_cos_theta(omega1_24_lim); // B3 solar height above cell surface at sunrise for a sloping cell
			float cos_theta_omega_sminus = calculate_cos_theta(-omega_s); // B3 solar height above cell surface when sunrise for a horizontal cell
			
			if ((cos_theta_omega_sminus <= cos_theta_omega1) & (cos_theta_omega1 < 0.001)) {
				omega1_24 = omega1_24_lim; // OK as is, B4
			}
			else { // B5
				float omega1x = -pi - omega1_24_lim;
				float cos_theta_omega_1x = calculate_cos_theta(omega1x);
				
				if (cos_theta_omega_1x > 0.001) //B5a
					omega1_24 = -omega_s; // OK as is
				else if (omega1x <= -omega_s) // B5b
					omega1_24 = -omega_s; // OK as is
				else // B5c
					omega1_24 = omega1x;
				
				if (omega1_24 < -omega_s) // B6
					omega1_24 = -omega_s;
			}
			
			//---- step C ----
			// cosine of potential solar incidence angles at sunset
			float cos_theta_omega2 = calculate_cos_theta(omega2_24_lim); // C3
			float cos_theta_omega_s = calculate_cos_theta(omega_s); // C3
			
			if ((cos_theta_omega_s <= cos_theta_omega2) & (cos_theta_omega2 < 0.001)) {
				omega2_24 = omega2_24_lim; // OK as is, C4
			}
			else { // C5
				float omega2x = pi - omega2_24_lim;
				float cos_theta_omega_2x = calculate_cos_theta(omega2x);
				
				if (cos_theta_omega_2x > 0.001) //C5a
					omega2_24 = omega_s; // OK as is
				else if (omega2x >= omega_s) // C5b
					omega2_24 = omega_s; // OK as is
				else // C5c
					omega2_24 =  omega2x;
				
				if (omega2_24 > omega_s) // C6
					omega2_24 = omega_s;
			} // end else C5
			
			
			//---- step D ---- steep poleward facing surface shaded around noon
			// step D1 not implemented because sqrt always returns a number >= 0
			
			if (omega2_24 < omega1_24) {// D2, always shade
				omega1_24 = 0.0;
				omega2_24 = 0.0;
			}
			
			else if(sin(s) > sin(phi) * cos(delta) + cos(phi) * sin(delta)) { // D4, two periods of insolation per day
				float omega2_24b = min(omega2_24_lim, omega1_24_lim); // Eq 44-46
				float omega1_24b = max(omega2_24_lim, omega1_24_lim);  
				
				float cos_theta_omega_2b = -a + b*cos(omega2_24b) + c*sin(omega2_24b); // Eq 47a
				float cos_theta_omega_1b = -a + b*cos(omega1_24b) + c*sin(omega1_24b); // Eq 47b
				
				if ((cos_theta_omega_2b < -0.001) | (cos_theta_omega_2b > 0.001)) // Eq 48a
					omega2_24b = -pi -omega2_24b;
				if ((cos_theta_omega_1b < -0.001) | (cos_theta_omega_1b > 0.001)) // Eq 48a
					omega1_24b = pi -omega1_24b;
				
				if (omega2_24b < omega1_24) // Dfg, Eq. 49a
					omega2_24b = omega1_24;
				if (omega1_24b > omega2_24) // Dfg, Eq. 49a
					omega1_24b = omega2_24;
				
				if (integrate_cos_theta_time(omega2_24b, omega1_24b) <= 0.0) {
					omega_set2  = omega2_24b;
					omega_rise2 = omega1_24b;
				}
			} // end two periods of insolation 
			
		} // end else tilted and rotated surface
		
		omega_rise = omega1_24;
		omega_set = omega2_24;
		
	} // end polarchecked
}

/**
Allen et al. 2006, Eq. 5
*/
float SOLAR_RADIATION::integrate_cos_theta_time(float omega1, float omega2)
{
	float integrated_cos_theta = sin(delta) * sin(phi) * cos(s) * (omega2 - omega1)
	- sin(delta) * cos(phi) * sin(s) * cos(gamma) * (omega2 - omega1)
	+ cos(delta) * cos(phi) * cos(s) * (sin(omega2) - sin(omega1))
	+ cos(delta) * sin(phi) * sin(s) * cos(gamma) * (sin(omega2) - sin(omega1))
	- cos(delta) * sin(s) * sin(gamma) * (cos(omega2) - cos(omega1));
	return integrated_cos_theta;
}
