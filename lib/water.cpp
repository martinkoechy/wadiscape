/*
 *  water.cpp
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */
#include "radiation.h"
#include "water.h"
#include "cell.h"
#include <cmath>

SOIL_PARAMS* WATER::soilP = NULL;
SOLAR_RADIATION* WATER::p_my_earth = NULL;

SOIL_PARAMS::SOIL_PARAMS(float diameter_mm) :
	grain_diam_mm(diameter_mm), density_water(998.2), density_quartz(2650) {
	//---------------------------------------------
	// soil parameters, physical constants, mean grain diameter, kritische Bodenschubspannung
	//---------------------------------------------
	kin_visc = 0.0010087 / density_water; // 20°C = 1.0087 kinetic viscosity

	pRatio = sqrtf(density_quartz / density_water - 1.0);

	dStar = calc_dStar(grain_diam_mm);

	ShieldKrit = calc_ShieldCrit(dStar);

	TKrit = (density_quartz - density_water) * _g * grain_diam_mm * ShieldKrit;

	// -- run time optimization ----
	inv_dStar = 1.0 / dStar;

	pow_dStar = dStar > 0.0 ? pow(dStar, -0.3) : 0.0;

	pow_grain_diam = pow(grain_diam_mm, 1.5);

	formulaUcr = ShieldKrit >= 0.0 ? 5.75 * sqrtf(ShieldKrit * (density_quartz
			/ density_water - 1.0) * _g * grain_diam_mm) : 0.0;
	// -- end  run time optimization -----
}// Klasse zu


/**
 d* = ((ρQuartz - ρwater) * g / (ρwater * kinetic viscosity²))^(1/3) * mean grain diameter
 kinetic viscosity ≈ 1E-6
 */
float SOIL_PARAMS::calc_dStar(float mean_grain_diameter = 0.02) {
	return pow((density_quartz - density_water) * _g / (density_water * 1E-12),
			q1_3) * mean_grain_diameter;
}

/**
 From Mathaj 2007
 */
float SOIL_PARAMS::calc_ShieldCrit(float d_star) {
	float myShieldCrit = 0.0;

	if (d_star > 0.0 && d_star <= 4.0)
		myShieldCrit = 0.24 * pow(d_star, -1.0);
	else if (d_star <= 10.0)
		myShieldCrit = 0.14 * pow(dStar, -0.64);
	else if (d_star <= 20.0)
		myShieldCrit = 0.04 * pow(d_star, -0.1);
	else if (d_star <= 150)
		myShieldCrit = 0.013 * pow(d_star, 0.29);
	else
		myShieldCrit = 0.055;

	return myShieldCrit;
}

float WATER::P_netSoilMovement[12];
float WATER::N_netSoilMovement[12];
float WATER::W_netSoilMovement[12];
float WATER::S_netSoilMovement[12];
float WATER::P_netWaterMovement[12];
float WATER::N_netWaterMovement[12];
float WATER::W_netWaterMovement[12];
float WATER::S_netWaterMovement[12];

WATER:: WATER (void) {
	elevation = 0.0;
	cell_slope_rad = 0.0;
	aspect_rad = 0.0;
	infiltration_rate_mmh = 0.0;
	kStr = 0.0;
	avail_rel = 0.0;
	for(short y=0; y<3; avail_abs[y++] = 0.0);
	aboveground_water_mm = 10.0;
	bedrock_water_mm = 150.0; // give seedlings something to grow from in the months till the first rain.
	monthly_infiltr_water_mm = 0.0;
	mRain_mm = 0.0;
	ann_rain_mm = 0.0;
	accumulated_runon_mm = 0.0;
	rain_mmh = 0.0;
	v_max_runoff = 0.0;
	sum_velocity = 0.0;
	is_sink = false;
	runon_l = 0.0;
	time_s = 0.0;
	flow_direction_id = 0;
	moved_soil_kg = 0.0;
	pET_mm = 0.0;
	for(short m = 0; m<12; monthly_cummWater[m++] = 0.0);
}

float WATER::get_avg_water_mm(void) {
	return (avail_abs[0] + avail_abs[1] + avail_abs[2]) * q1_3;
}

/** weighted average of available water across the past years
 */
float WATER::get_wavg_water_mm(void) {
	return avail_abs[0] * 0.6 + avail_abs[1] * 0.3 + avail_abs[2] * 0.1; //MM 02.2007, MK changed 2008-07-11;
}

void WATER::update(void) {
	bedrock_water_mm += (0.5 * avail_abs[0]); // long-term storage, groundwater, drainage; 

	avail_abs[2] = avail_abs[1];
	avail_abs[1] = avail_abs[0];
	avail_abs[0] = 0.0;
	
	for(short M=0; M <= 11; ++M)
		monthly_cummWater[M] = 0.0;
	
	ann_rain_mm = 0.0;
	moved_soil_kg = 0.0;
	accumulated_runon_mm = 0.0;
}

void WATER::add_daily_rain(float rain_mm) {
	mRain_mm += rain_mm;
	ann_rain_mm += rain_mm;
}

float WATER::ortho(float cell_length_or_area) {
	return cell_length_or_area * cos(cell_slope_rad); // rain in mm/m, therefore no adjustement for cell length
}

float WATER::get_mRain(void) {
	return mRain_mm;
}

float WATER::get_mWater(void) {
	return monthly_infiltr_water_mm;
}

void WATER::reset_mWater(void) {
	monthly_infiltr_water_mm = 0.0;
	mRain_mm = 0.0;
}

float WATER::get_rel_cummWater(month_names month) {
	return avail_abs[0]?monthly_cummWater[int(month)] / avail_abs[0]:0.0; // includes check for division by zero at start of year
}

/** The rainy season has ended in the month after the accumulated rain equals
 the annual rain.
 */
bool WATER::is_past_rainy_month(month_names m) {
	/* TODO: this seems to be a rather clumsy way to determine the end of the rainy season in each
	 cell, but I can't think of a more elegant way. MK 20080609.
	 */
	return monthly_cummWater[m] == avail_abs[0] 
	&& monthly_cummWater[m] == monthly_cummWater[m - 1] 
	&& m > Feb ? true : false; // the month after which at least some rain should have fallen
}

void WATER::accumulate_monthly_water(month_names m) {
	if(monthly_infiltr_water_mm > 0.0) {
		avail_abs[0] += monthly_infiltr_water_mm;
		
		for(month_names M = m; M <= Jul; ++M)
			monthly_cummWater[int(M)] += monthly_infiltr_water_mm;
	}
}

/**
 Infiltration after rain. Returns the volume of excess water.
 */
float WATER::infiltration(void) { 
	float infiltrated_water_mm = 0.0;
	aboveground_water_mm += runon_l * q1_cell_area_m2;
	accumulated_runon_mm += runon_l * q1_cell_area_m2; // for spatial statistics
	runon_l = 0.0;
	
	if (aboveground_water_mm > 0.0) {
		float infiltrating_capacity_mm = aboveground_water_mm;
		// if the cell has no lower cells, all water infiltrates immediately
		if(is_sink) {
			infiltrated_water_mm = aboveground_water_mm;
			monthly_infiltr_water_mm += infiltrated_water_mm;
			aboveground_water_mm = 0.0;
		}
		else {
			infiltrating_capacity_mm = infiltration_rate_mmh * time_s * q1_3600; ///< infiltration capacity
			if (infiltrating_capacity_mm > aboveground_water_mm) { // if the capacity is large enough
				infiltrated_water_mm = aboveground_water_mm;
				monthly_infiltr_water_mm += infiltrated_water_mm; // all water can infiltrate
				aboveground_water_mm = 0.0; // and nothing is left
			} else {
				infiltrated_water_mm = infiltrating_capacity_mm;
				monthly_infiltr_water_mm += infiltrated_water_mm; // water can infiltrate
				aboveground_water_mm -= infiltrated_water_mm;
			}
		}
	} 	
	return infiltrated_water_mm; // for error checking
}

/**
Infiltration during rain. Returns the volume of excess water.
 */
float WATER::infiltration(float time_h) {
	float infiltrated_water_mm = 0.0;
	aboveground_water_mm += runon_l * q1_cell_area_m2;
	accumulated_runon_mm  += runon_l * q1_cell_area_m2; // for spatial statistics
	runon_l = 0.0;

	if (aboveground_water_mm > 0.0) {
		float infiltrating_capacity_mm = aboveground_water_mm;
		// if the cell has no lower cells, all water infiltrates immediately
		if(is_sink) {
			infiltrated_water_mm = aboveground_water_mm;
			monthly_infiltr_water_mm += infiltrated_water_mm;
			aboveground_water_mm = 0.0;
		}
		else {
			infiltrating_capacity_mm = infiltration_rate_mmh * time_h; ///< infiltration capacity
			if (infiltrating_capacity_mm > aboveground_water_mm) { // if the capacity is large enough
				infiltrated_water_mm = aboveground_water_mm;
				monthly_infiltr_water_mm += infiltrated_water_mm; // all water can infiltrate
				aboveground_water_mm = 0.0; // and nothing is left
			} else {
				infiltrated_water_mm = infiltrating_capacity_mm;
				monthly_infiltr_water_mm += infiltrated_water_mm; // water can infiltrate
				aboveground_water_mm -= infiltrated_water_mm;
			}
		}
	} 
	return infiltrated_water_mm; // for error checking
}

/**
 Calculation of the maximum run-off velocity according to Manning-Strickler.
 Returns the velocity. The neighbourhood index is set to the cell to which the
 water flows fastest.
 */

float WATER::calculate_runoff_velocity(uint32 position,
		short& flow_direction_id) {
	// calculate variables for run-off velocity

	// 1. wet perimeter = 2 * octogonal cell length + 2 * height of water column (m)
	float perimeter_m = 2 * (cell_octogon_edge_m + mm_to_m(
			aboveground_water_mm));

	// 2. area, through which the water flows = octogonal cell length * height of water column (m)
	float cross_section_m = cell_octogon_edge_m * mm_to_m(aboveground_water_mm);

	// 3. hydraulic radius = cross section / wetted perimeter
	float hydraulic_radius_m = cross_section_m / perimeter_m;

	// search for the direction of fastest run-off
	v_max_runoff = 0.0;

	// check all neighbours;
	// kStr can vary, therefore runoff is not necessarily down the steepest slope

	// determine the current position of the cell
	unsigned short iz = position / NZ;
	unsigned short is = position % NZ;
	int nb_id = -1; ///< position of neighbour cells
	WATER* pNb; ///< pointer to neighbour cell


	for (short d_iz = -1; d_iz < 2; d_iz++) {
		for (short d_is = -1; d_is < 2; d_is++) {
			if (d_iz == 0 && d_is == 0)
				continue;
			nb_id++; // keep track of position
			pNb = this + d_iz * NZ + d_is; // pointer arithmetic

			float kStr_nb = 0.0; ///< Strickler number of neighbour cell

			// beyond grid
			if (iz + d_iz < 0 || iz + d_iz > NZ - 1 || is + d_is < 0 || is
					+ d_is > NS - 1) {
				kStr_nb = kStr; // assume same value as in this cell
			} else { // within grid
				kStr_nb = pNb->kStr;
			}

			// velocity of run-off according to Manning/Strickler
			// v = mean Strickler-value * hydraulic radius^2/3 * sqrt(slope, ‰)
			v_runoff[nb_id] = angle_nb[nb_id] < 0.0 ? ((kStr + kStr_nb) * 0.5) * pow(
					hydraulic_radius_m, q2_3) * sqrtf(-angle_nb[nb_id]) : 0.0; // no run-off if the slope is upwards

			// store the position to where water runs if it is "schießend"
			if (v_runoff[nb_id] > v_max_runoff) {
				v_max_runoff = v_runoff[nb_id];
				flow_direction_id = nb_id;
			}
			if(nb_id>-1) sum_velocity += v_runoff[nb_id]; // Mathaj 2007 used angles for weighting runoff
		} // for d_is
	} // for d_iz

	is_sink = v_max_runoff < 1E-6 ? true : false; // the conditions for sink may have changed due to vegetation cover

	return v_max_runoff;
}

/*
 Water that did not infiltrate is distributed to lower cells. If the conditions
 indicate slow, laminar flow, water is distributed to all lower cells, else the
 conditions indicate rapid, turbulent flow and the water runs along the steepest
 gradient. The amount of runoff is stored in the variable "runon" of the neighbour
 cell. The conditions for laminar and turbulent flow are decided according to
 Froude and Reynolds number. According to Mathaj 2007.

 */
float WATER::distribute_runoff(uint32 position) {
	flow_direction_id = -1; ///< direction of steepest slope
	float v_max_runoff = calculate_runoff_velocity(position, flow_direction_id);
	float my_total_runoff_mm = 0.0;
	
	if(v_max_runoff && aboveground_water_mm) // prevent division by zero
	{
	// calculate Froude number F, F = v / √(g * h)
	// and Reynolds number R, R = v * h / kv
	// kv: kinematic viscosity (m²/s), kv = 0.0010087/density of water(kg/m³) ≈ 1E-6, 1/kv ≈ 1E6

	/// Froude number, F > 1 turbulent flow , F > 1.2 rill erosion
	float F = v_max_runoff / sqrtf(_g * mm_to_m(aboveground_water_mm));

	/// Reynolds number, R > 1500 turbulent flow, R < 500 laminar flow
	float R = v_max_runoff * mm_to_m(aboveground_water_mm) * 1E6;

	// conditions for turbulent run-off, water flows along the steepest direction
	bool is_turbulent = (F > 1.0 || R > 1500.0) ? true : false;

	// determine the current position of the cell
	short iz = position / NZ;
	short is = position % NZ;
	short nb_id = -1; ///< id of neighbour cells
	WATER* pNb; ///< pointer to neighbour cell

	float my_relative_runoff = 0.0, my_runoff_surface = 0.0, my_infiltration_time_s = 0.0;
	float my_runoff_mm = 0.0;

	for (short d_iz = -1; d_iz < 2; d_iz++)
		for (short d_is = -1; d_is < 2; d_is++) {
			if (d_iz == 0 && d_is == 0) continue;
			my_runoff_mm = 0.0;
			nb_id++; // keep track of position
			pNb = this + d_iz * NZ + d_is; // pointer arithmetic

			if (is_turbulent)
				my_relative_runoff = (nb_id == flow_direction_id ? 1.0 : 0.0); // automatic unidirectional flow
			else
				my_relative_runoff = v_runoff[nb_id] / sum_velocity; // all directions

			if(my_relative_runoff > 0.0) {
				my_runoff_mm = aboveground_water_mm * my_relative_runoff; // runoff height (mm)
				my_runoff_surface = cell_area_m2 * my_relative_runoff; // inclined run-off surface (m²)
				if(v_runoff[nb_id] > 0.0)
					my_infiltration_time_s = my_runoff_surface / (cell_octogon_edge_m
						* v_runoff[nb_id]); // t = A/(s * s/t)

				// pass on runoff and sediment to neighbour cells
				if (!(iz + d_iz < 0 || iz + d_iz > NZ - 1 || is + d_is < 0 || is
						+ d_is > NS - 1)) { // only inside grid
					pNb->runon_l += my_runoff_mm * my_runoff_surface;
					pNb->time_s += my_infiltration_time_s;
					pNb->moved_soil_kg += SediTransport(v_runoff[nb_id],
							my_infiltration_time_s, mm_to_m(my_runoff_mm), (kStr
									+ pNb->kStr) * 0.5);
				}
			}
			my_total_runoff_mm += my_runoff_mm * my_relative_runoff;
		}
	}
	accumulated_runon_mm -= my_total_runoff_mm; // for spatial output
	return my_total_runoff_mm;
}

/**
 Calculation of potential daily evaporation according to Oudin et al. 2005.
 if T > -5, pET = R/(lambda * density of water) * (T+5)/100, else pET = 0
 R: extraterrestrial radiation (MJ/(m² d)), lambda: latent heat flux ≈ 2.45 MJ/kg, density of water = 1000 kg/m³
 latent heat flux adjusted to the formula in Xu and Singh 2002
 multiplication by 1000 to convert from m to mm, explained in Allen et al. 1998, wrong in Oudin et al. 2005
 */
float WATER::get_pot_evapo_mm(float radiation_MJm2d, float TdegC) {
	return radiation_MJm2d / (2.5 - 2.361E-3 * TdegC) * (TdegC + 5) * 0.01;
}

// executed daily
void WATER::evapotranspiration(CELL* cell, month_names m) {
	float aET_mm = pET_mm;
	float dwarfshrubs_herbs_LAIM = cell->dwarfshrubs.get_LAIM_rel(m) + cell->herbs.get_LAIM_rel(m);
	
	// effect of herb and shrub transpiration, based on FAO algorithm for 0.5 m high vegetation, trees have their own algorithm
	// http://www.fao.org/docrep/X0490E/x0490e0f.htm#chapter%209%20%20%20etc%20for%20natural,%20non%20typical%20and%20non%20pristine%20vegetation
	if(dwarfshrubs_herbs_LAIM > 0.0)
		aET_mm *= (0.15 + (1 + 0.01 * cell->get_heightM_m(m) - 0.15) * (1 - exp(
			-0.7 * dwarfshrubs_herbs_LAIM)));
	//	std::cout << aET_mm << std::endl;

	// trees have their own transpiration function
	
	if (monthly_infiltr_water_mm < aET_mm)
		aET_mm = monthly_infiltr_water_mm;
	monthly_infiltr_water_mm -= aET_mm;
}

/**
 Shear stress is calculated according to Strickler and Nikuradse. From Mathaj 2007.
 */
float WATER::SediTransport(float velocity_ms, float time_s, float height_m,
		float m_kStr)
{
	// calculate shear stress according to Strickler and Nikuradse
	float shear_stress = 0.0;
	if (height_m > 0.0 && m_kStr > 0.0 && height_m > 0)
		shear_stress = soilP->density_water * _g * velocity_ms * velocity_ms
				/ (m_kStr * m_kStr * pow(height_m, q1_3));

	// dimensionslose Bodenschubspannung nach van Rijn, und rollender und hüpfender Transport durch empirische Modelle
	float shear_stress_rel = (shear_stress - soilP->TKrit) / soilP->TKrit;

	float qsn1; ///< transportation rate of rolling and skipping particles
	float qsn2; ///< transportation rate of suspended particles

	if (shear_stress_rel < 1E-6)
		qsn1 = 0.0;
	else if (shear_stress_rel < 3.0)
		qsn1 = 0.053 * soilP->pRatio * gROOT * soilP->pow_grain_diam
				* soilP->pow_dStar * pow(shear_stress_rel, 2.1);
	else
		qsn1 = 0.1 * soilP->pRatio * gROOT * soilP->pow_grain_diam
				* soilP->pow_dStar * pow(shear_stress_rel, 1.5);

	float loga = (12.0 * height_m) / (3.0 * soilP->grain_diam_mm);
	float Ucr = loga > 0.0 ? soilP->formulaUcr * log10(loga) : 0.0;

	if (Ucr < velocity_ms) {
		float UU = (velocity_ms - Ucr) / soilP->pRatio; // Transport in Schwebe nach van Rijn
		qsn2 = (UU >= 0.0 && height_m > 0.0) ? 
				0.012 * velocity_ms * height_m * pow(UU, 2.4)
				* (soilP->grain_diam_mm / height_m) * soilP->inv_dStar :
				0.0; // error, changed 2010-05-07
	} else
		qsn2 = 0.0;

	//  erosion in kg per cell
	float my_erosion_kg = (qsn1 + qsn2) * time_s * cell_octogon_edge_m;

	moved_soil_kg -= my_erosion_kg;
	return my_erosion_kg;
}
// Ende Seditransport

