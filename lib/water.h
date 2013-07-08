/*
 *  water.h
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include "main.h"
#include "radiation.h"

#ifndef WATER_H
#define WATER_H

struct CELL;

/**
Definitions of physical constants, mean grain size, critical shear stress. From Mathaj 2007.
 */
struct SOIL_PARAMS
{
	SOIL_PARAMS(float mean_grain_diameter);
	~SOIL_PARAMS(){};

	const float grain_diam_mm; ///< mean grain diameter, mm
	const float density_water; ///< density of water, 998.2 kg/m³ at 20°C
	const float density_quartz; ///< density of quartz, 2650.0 kg/m³

	float kin_visc; ///< kinematic viscosity at 20°C
	float dStar; ///< dimensionless grain diameter

	float calc_dStar(float mean_diam_mm);
	float calc_ShieldCrit(float d_star);
	float TKrit;     ///< critical shear stress, kritische Bodenschubspannung N/m2
	float ShieldKrit;///< Shields parameter

	float pRatio;      //für Laufzeitoptiemirung; pow(soilP.density_quartz/soilP.density_water-1.0,0.5)
	float inv_dStar;   //für Laufzeitoptiemirung; 1/dStar (1/dim. loser Korndurchmesser)
	float pow_grain_diam;    //für Laufzeitoptiemirung; pow(soilP.d50[i],1.5)
	float pow_dStar;        //für Laufzeitoptiemirung; pow(soilP.dStar[i],-0.3)
	float formulaUcr;  //für Laufzeitoptiemirung; = 5.75*(soilP.ShieldKrit[i],0.5)*pow((soilP.density_quartz/soilP.density_water-1.0)*g*soilP.d50[i],0.5)*log10((12.0*h)/(3*soilP.d50[i]));
};


struct WATER
{	static SOIL_PARAMS* soilP;
	static SOLAR_RADIATION* p_my_earth;
	
	static float P_netSoilMovement[12];
	static float N_netSoilMovement[12];
	static float W_netSoilMovement[12];
	static float S_netSoilMovement[12];
	static float P_netWaterMovement[12];
	static float N_netWaterMovement[12];
	static float W_netWaterMovement[12];
	static float S_netWaterMovement[12];
	
	float elevation; // 0-100 m
	float cell_slope_rad;	///< local slope [m/m] of the cell with respect to horizontal, in radians, i.e. 0...π/2
	float aspect_rad;		///< aspect [radians], S = 0, E = -π/2, W = +π/2, N = +π
	habitat_names habitat;

	float infiltration_rate_mmh;  ///< maximum infiltration rate, contingent on vegetation
	float kStr;		///< Strickler number for roughness of soil (m^1/3 / s)
	float angle_nb[8]; /// angle to neighbour cell in permill


	float avail_rel;
  float avail_abs[3];
  float monthly_cummWater[12];
	float netSoilMovement[12];
	float netWaterMovement[12];
  float aboveground_water_mm;
  float bedrock_water_mm;
  float monthly_infiltr_water_mm;
  float mRain_mm; ///< monthly rain (m)
	float ann_rain_mm; ///< annual rain (mm)

  float accumulated_runon_mm;
  
	float rain_mmh;		///< rain intensity
	float v_runoff[8];	///< velocity of runoff to each neighbour cell
	float v_max_runoff;	///< greatest speed of runoff to neighbour cells
	float sum_velocity;	///< sum of runoff velocities for calcualting a weighted multi-diretional runoff with laminar flow
	bool is_sink;			///< is the elevation of the cell a local minimum
	float runon_l;		///< accumulated amount of runon in litres
	float time_s;			///< accumulated time (s) for runon infiltration
	short flow_direction_id; ///< direction of flow if runoff is turbulent
	float moved_soil_kg;    ///< amount (kg) of eroded or deposited material per cell
	float pET_mm;		///< daily potential evapotranspiration of the 15th of a month
	
  void  update (void);
  void add_daily_rain (float);
  float get_mRain (void);
  float get_mWater (void); ///< get the available water of this month
  float get_wavg_water_mm (void);///< weighted average of available water across the past three years, used to calculate alpha in fire routine
  float get_avg_water_mm (void);///< weighted average of available water across the past three years, used to calculate alpha in fire routine
  float get_rel_cummWater (month_names m); ///< get the cummulative water
  void reset_mWater(void); ///< reset the monthly available water to zero
  void accumulate_monthly_water (month_names m);
  bool  is_past_rainy_month (month_names month); ///< is it past the end of the rainy season?
  float get_pot_evapo_mm(float radiation_MJm2d, float TdegC);


	WATER (void);
	~WATER (){};
	  
  /// Infiltration (rain or run on) into the soil
  float infiltration(void); // runon
  float infiltration(float time_h); // rain, no evaporation

  /// Calculate the amount (kg) of sediment transported according to van Rijn
  float SediTransport(float velocity_ms, float time_s, float height_m, float mean_kStr);

  /// Adjust rain amount, cell edge length or cell area for inclination of surface
  float ortho(float);

  /// Calculate the runoff velocity and direction of fastest flow according to Manning and Strickler.
  float calculate_runoff_velocity(uint32 position, short& flow_direction_id);

  /// Distribute the runoff to adjacent cells, return value: the amount of water run off
  float distribute_runoff(uint32 position);

  /// Remove transpired and evaporated water
  void evapotranspiration(CELL* cell, month_names m);
};

#endif
