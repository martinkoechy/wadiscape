/*
 *  dwarfshrub.cpp
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include "dwarfshrub.h"
#include "water.h"
#include "cell.h"
#include "main.h"
#include <math.h>

DWARFSHRUB::DWARFSHRUB (void) {
	palatability = 0.0;
	cover = 0.0;
	delta_cover = 0.0;
	colonized_cov = 0.0;
	leaf_mass_g = 0.0;
	height_m = 0.0;
}

DWARFSHRUB::DWARFSHRUB(CELL* p_containing_cell)
: WOODY(p_containing_cell)
{
	for(short m = 0; m < 11; m++) {
		feedM_kgcell[m] = 0.0;
		coverM_rel[m] = 0.0;
		heightM_m[m++] = 0.0;
		LAIM_rel[m++] = 0.0;
	}
	
	float zuf1 =z01();
	float zuf2 = z01();
	
	palatability = 0.5; ///< refers to current year's growth!
	cover = zuf1<0.7 ? zuf2*0.25+0.1 : 0.0;
	delta_cover = 0.0;
	colonized_cov = 0.0;
	
	leaf_mass_g = convert_cover_to_leaf_mass_gCell(cover);
	height_m = calc_height_m(cover);
	months_since_fire = 0;
}

DWARFSHRUB* DWARFSHRUB::initialize(climate_names c, CELL* pContaining_cell) {
	float zuf1 =z01();
	float zuf2 = z01();
	
	cover = zuf1<0.7 ? zuf2*float(c)/10.0+0.3 : 0.0;
	delta_cover = 0.0;
	colonized_cov = 0.0;
	
	leaf_mass_g = convert_cover_to_leaf_mass_gCell(cover);
	height_m = calc_height_m(cover);
	
	DWARFSHRUB* d = NULL;
	return d;
}

/**
The mean height of Sarcopoterium shrubs is 47 cm, SD = 12 cm (Sternberg and
Shoshany 2001b). Height increases with crown area asymptotically to 70 cm
 till 3 m² ,  <i>n</i>=24 (raw data of Sternberg and Shoshany 2001b).
*/
float DWARFSHRUB::calc_height_m(float my_cover) {

	return height_m = cover?0.7072-1.0/(4.2292*my_cover + 1.0/0.7072):0.0;
}

/** <p>
Leaf mass calculated from cover. Leaf area calculated using specific leaf area of Phrygana (Margaris 1976).
Adjustment for monthly variation in specific leaf mass (Liakoura et al. 2001) based on the mean of <i>Sarcopoterium spinosum</i>, 
<i>Pistacia lentiscus</i>, and <i>Thymelaea hirsuta</i>. An alternative would be to concentrate on <i>Sarcopoterium</i>.
It has a higher specific leaf area (0.001 m2/g) and stronger seasonal variation (Liakoura et al. 2001). 
 </p> */
float DWARFSHRUB::calc_LAI_rel(month_names m) {
	float leaf_mass_g = convert_cover_to_leaf_mass_gCell(coverM_rel[short(m)]); // measured in April
	float leaf_area_m2 = leaf_mass_g * 0.008534; // average value of Phrygana, Oct-May, Margaris 1976

	/* adjust for monthly variation in specific leaf area, Liakoura et al., 2001, standardized on April
	*/
	switch (m) {
		case Aug:
			leaf_area_m2 *= 0.9499186;
			break;
		case Sep:
			leaf_area_m2 *= 0.8188140;
			break;
		case Oct:
			leaf_area_m2 *= 0.9283906;
			break;
		case Nov:
			leaf_area_m2 *= 0.8894672;
			break;
		case Dec:
			leaf_area_m2 *= 0.8525386;
			break;
		case Jan:
			leaf_area_m2 *= 0.9308908;
			break;
		case Feb:
			leaf_area_m2 *= 0.8328316;
			break;
		case Mar:
			leaf_area_m2 *= 1.0154293;
			break; // Liakoura: May
		case Apr:
			leaf_area_m2 *= 1.0000000;
			break;
		case May:
			leaf_area_m2 *= 1.6404755;
			break;
		case Jun:
			leaf_area_m2 *= 1.3895913;
			break;
		case Jul:
			leaf_area_m2 *= 1.0358490;
			break;
	}
	return leaf_area_m2 * q1_cell_area_m2;
}

/**
The Factor of 3748.53 is based on a regression of leaf mass on crown area with the
original data of Sternberg &amp; Shoshany (2002a), R²=0.98, n = 44.
(24 original data sets, 2 excluded, remainder sorted, cumulatively summed and
 added to data set. This resulted in a straight line showing that it doesn't
matter whether the cover consists of one big or several small shrubs.)
 "regression shrub mass on cover.R"
  */

float DWARFSHRUB::convert_cover_to_leaf_mass_gCell(float my_cover) {
	return max(0.0, my_cover * 3748.53); // per cell
}

/** see note for convert_cover_to_mass_g */
float DWARFSHRUB::convert_leaf_mass_gCell_to_cover(float leaf_mass_g) {
	return leaf_mass_g / 3748.53; // per cell
}

/** This function calculates the change in cover for a whole year based on the
cover in July (after grazing) plus additional cover by colonization. The
change in cover is assumed to be in the form of edible biomass and feed mass
is updated accordingly. Since the growth of dolichoblasts is strongest in
May (Orshan 1989), the biomass is added from May on and carried over into
the next vegetation season (via a class-global variable).
*/
void DWARFSHRUB::grow_year(WATER* W) {
		
	// add the cover of colonized shrubs
	cover += colonized_cov;
	if (cover > 1.0) // make sure cover does not get greater than 100%
		cover = 1.0;
	// reset colonized cover
	colonized_cov = 0.0;
	
	
		
	// at start of simulation year, set the potential production in the absence of grazing;
	// growth is fairly independent of rain (Orshan, 1989), therefore it is
	// distributed evenly accoss the months, starting in May when the dolichoblasts
	// have their peak growth
	// I use delta_cover to calculate feedM because I assume that goats and sheep will eat only the fresh leaves
	// and avoid the woody, thorny older parts.
	// NOTE: Last year's accumulated soil water is used!
	int n = 0;
	for (month_names m = Aug; m<=Apr; ++m) {
		if (cover>0 || delta_cover>0 ) {
			n = int(m)+(m<Apr?5:4); // months after April, May counts twice, no growth in April
			feedM_kgcell[int(m)] = g_to_kg(convert_cover_to_leaf_mass_gCell(cover+n*delta_cover/12.0)); // conversion checked
		}
		else
			feedM_kgcell[int(m)] = 0.0;
	}
}

/** This function updates the cover, height, and LAI of the dwarf shrubs. Cover
is calculated as the previous year's cover plus the converted leaf mass of
the current month. Feed mass is updated in grow_year and may be reduced by
foraging via VEGETATION::removeMassByForaging.
*/
bool DWARFSHRUB::grow_month(WATER *W, month_names m) {
	short M = short(m);
	cover = convert_leaf_mass_gCell_to_cover(kg_to_g(feedM_kgcell[M==0?11:(M-1)]));
	if(m==May) {
	// remember last year's value because shrubs start new growth in May
	
	// get soil moisture information
	float mean_w_avail = W->get_wavg_water_mm();
	float w_avail = W->monthly_cummWater[int(May)];
	//std::cout<< mean_w_avail << "\t" << w_avail << std::endl;
	if (mean_w_avail == 0.0)
		mean_w_avail =  1.0; // prevent division by zero further down
	
	/* =======================================================================
	 calculate the change in cover for the new season, starting in May   */
	delta_cover = 0.0;
	
	//last modified 20.3.04;
	// regression from Dan's shrub model;
	// Note: per cent converted to relative units (MK 2006-06-23)
	
	float zuf = z01();
	if (cover) {
		/* mortality, Malkinson (2007) eq 2, but too low to reproduce 60%
		 shrub cover in mesic site (Henkin et al. 1998). This is due to the
		 larger cell size used in this model. Therefore, the mortality rate
		 was adjusted to fit 60% shrub cover on S-facing slopes in 'En Ya'aqov.
		 */
/*		if (w_avail > 200.0 && zuf < 0.04*cover)
			delta_cover = -cover; // shrub mortality due to chance
		else if (w_avail <= 200.0 && zuf > mean_w_avail/50.0 * (1-0.05*cover))
			delta_cover = -cover; // shrub mortality due to drought
		else // MK 20060711, adapted from Dan's transition matrix
*/			// Dan's regressions didn't reflect the growth well
		{
			if (w_avail < 175.0) {
				if (zuf < 0.10)
					delta_cover = 0.02;
				else if (zuf > 0.04) // 20110907: changed from 0.41 to achieve stability in semiarid climate
					delta_cover = -0.02;
			} else if (w_avail < 225.0) {
				if (zuf < 0.25)
					delta_cover = 0.02;
				else if (zuf > 0.84)
					delta_cover = -0.02;
			} else if (w_avail<400.0) {
				if (zuf < 0.55)
					delta_cover = 0.02;
				else if (zuf > 0.99)
					delta_cover = -0.02;
			} else //Daten Dan 7.3.04
			{
				if (zuf < 0.58)
					delta_cover = 0.1;
				else if (zuf > 0.92)
					delta_cover = -0.1;
			}
		}
	}
	
	if (cover + delta_cover > 1.0)
		delta_cover = 1.0 - cover; // add only up to 100 % cover
	
	if (cover + delta_cover < 0.0) 
		delta_cover = -cover; // only subtract what's there
	//std::cout << "\t" << cover << ", delta_cover: " << delta_cover << std::endl;
	
	// at start of simulation year, set the potential production in the absence of grazing;
	// growth is fairly independent of rain (Orshan, 1989), therefore it is
	// distributed evenly accoss the months, starting in May when the dolichoblasts
	// have their peak growth, therefore double growth in May.
	// I use delta_cover to calculate feedM because I assume that goats and sheep will eat only the fresh leaves
	// and avoid the woody, thorny older parts.
	// NOTE: Last year's accumulated soil water is used!
	int n = 2;
	for (month_names m = May; m<=Jul; ++m) {
		if (cover>0 || delta_cover>0) {
			feedM_kgcell[int(m)] = g_to_kg(convert_cover_to_leaf_mass_gCell(cover+n*delta_cover/12.0)); // conversion checked
			n++;
		}
		else
			feedM_kgcell[int(m)] = 0.0;
	}
	
	} // end if May
	// add the cover of the previous month to include grazing effects
	coverM_rel[M] = convert_leaf_mass_gCell_to_cover(kg_to_g(feedM_kgcell[M]));
	cover=coverM_rel[M];
	heightM_m[short(m)] = calc_height_m(cover);
	LAIM_rel[short(m)] = calc_LAI_rel(m);
	
	return false;
}


void DWARFSHRUB::colonize(short iz, short is, float w_avail, CELL* C) {
	/* ++++++++++++ Colonization by shrubs +++++++++ */
	float zuf;
	month_names m = Apr; // 20040324 MK
						 // no need to use "old_value" because only neighbour shrubs with cover > 1%
						 // are considered and the new shrub cover is 0.2% (MK 20060623)
						 // 20060710 MK
						 // I have summerized Dan's categories for survival after colonization so that
						 // the probabilities don't bounce up and down
	if (cover < 0.002) /* no shrub */
	{
		short jza, jze, jsa, jse;
		float dwarfshrub_cover = 0.0;


//		jza = max(iz-1s, 0s);
//				jze = min(iz+1s, NZ);
//				jsa = max(is-1s, 0s);
//				jse = min(is+1s, NS);
		jza = std::max(iz-1, 0);
		jze = min(iz+1, 300);
		jsa = std::max(is-1, 0);
		jse = min(is+1, 300);
		for (short jz = jza; jz<jze; jz++)
			for (short js = jsa; js<jse; js++) {
				if (jz == iz && js == is)
					break;
				zuf = z01();
				dwarfshrub_cover = C[jz*NZ+js].dwarfshrubs.get_coverM_rel(m);

				if (dwarfshrub_cover > 0.020) {
					if (w_avail<400) //Daten Dan
					{
						if (dwarfshrub_cover < 0.060) //shrub density source cell < 4%
						{
							if (zuf<0.3875)
								colonized_cov += 0.002; //input needed from Dan, 21.8.03
						} else if (dwarfshrub_cover < 0.100) //shrub density source cell < 10%
						{
							if (zuf<0.71)
								colonized_cov += 0.002; //input needed from Dan, 21.8.03
						} else if (dwarfshrub_cover < 0.140) //shrub density source cell < 14%
						{
							if (zuf<0.73)
								colonized_cov += 0.002; //input needed from Dan, 21.8.03
						} else if (dwarfshrub_cover >= 0.140) {
							if (zuf<0.89)
								colonized_cov += 0.002; //input needed from Dan, 21.8.03
						}
					} //end if <400mm
					else // Daten von Dan, 1.3.04
					{
						if (dwarfshrub_cover < 0.060) //shrub density source cell < 6%
						{
							if (zuf<0.8)
								colonized_cov += 0.002; //input needed from Dan, 21.8.03
						} else if (dwarfshrub_cover < 0.140) //shrub density source cell < 14%
						{
							if (zuf<0.955)
								colonized_cov += 0.002; //input needed from Dan, 21.8.03
						} else if (dwarfshrub_cover >= 0.140) {
							if (zuf<0.8)
								colonized_cov += 0.002; //input needed from Dan, 21.8.03
						}

					} /* end >400mm*/
				} // end if nb cover > 0.02
			} //end neighbour quest
		//std::cout << "colonized shrub cover: "<< colonized_cov << std::endl;
	} /* end if target cell free of shrub */
}
