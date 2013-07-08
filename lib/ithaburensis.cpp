/*
 *  ithaburensis.cpp
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include "ithaburensis.h"
#include "water.h"
#include "cell.h"


extern RNG* zZzo;

float ITHABURENSIS::desiccation_sensitivity = 0.5;

ITHABURENSIS::ITHABURENSIS(void) {
	height_m = 0.0;
	radius_base_m = 0.0;
	radius_top_m = 0.0;
	age = 0;
	acorns[1]=0; // MK 20080307
	acorns[0]=0; // MK 20080313
	leaf_layer_depth = 0.05;
	individuals = 0;		
	for(short m = 0; m < 11; m++) {
		feedM_kgcell[m] = 0.0;
		coverM_rel[m] = 0.0;
		heightM_m[m] = 0.0;
		LAIM_rel[m] = 0.0;
	}
	
    top_shoot_mass_g = 0.0;
    base_shoot_mass_g = 0.0;
	edible_mass_g = 0.0;
	yearsAfterSevereBurn = 100; 

}

ITHABURENSIS::ITHABURENSIS(CELL* pContaining_cell): TREE(pContaining_cell) {
	height_m = 0.0;
	radius_base_m = 0.0;
	radius_top_m = 0.0;
	age = 0;
	acorns[1]=0; // MK 20080307
	acorns[0]=0; // MK 20080313
	leaf_layer_depth = 0.05;
	individuals = 0;	
	for(short m = 0; m < 11; m++) {
		feedM_kgcell[m] = 0.0;
		coverM_rel[m] = 0.0;
		heightM_m[m] = 0.0;
		LAIM_rel[m] = 0.0;
	}
	
    top_shoot_mass_g = 0.0;
    base_shoot_mass_g = 0.0;
	edible_mass_g = 0.0;
	yearsAfterSevereBurn = 100; 
}

ITHABURENSIS::ITHABURENSIS(float height, float radius_base, float radius_top, int newAge, CELL* p_containing_cell, int n=1)
: TREE(p_containing_cell) 
{
	height_m = height;
	radius_base_m = radius_base;
	radius_top_m = radius_top;
	age = newAge;
	acorns[1]=0; // MK 20080307
	acorns[0]=0; // MK 20080313
	leaf_layer_depth = 0.05;
	individuals = n;

    top_shoot_mass_g = 0.0;
    base_shoot_mass_g = 0.0;
	edible_mass_g = 0.0;
	yearsAfterSevereBurn = 100; 
}

ITHABURENSIS* ITHABURENSIS::initialize(climate_names c, CELL* pContaining_cell) { 
	//initial distribution of ithaburensis trees
	float randomIthaburensis, randomStatus;
	ITHABURENSIS* ithaburensis= NULL;
	
	float occupancy = 0.0;
	switch (c) {
		case arid: occupancy = perCell; break;
		case semiarid: occupancy = perCell; break;
		case drymed: occupancy = 0.05; break;
		case typmed: occupancy = 0.6; break;
		case mesicmed: occupancy = 0.5; break;
	}
	
	
	randomIthaburensis = z01();
	float randomHeight;
	int randomAge;
	
	if (randomIthaburensis <= occupancy) { 
		randomStatus = z01();
		
		if (randomStatus<=0.2) { // probability for being a seedling
			randomHeight = zZzo->uniform(0.05, 0.3);
			
			radius_base_m = 0.07; // estimate of M. Koechy // set static radius_base_m
			
			randomAge = int(round(zZzo->uniform(0, 3))); // independent of height
			
			ithaburensis = new ITHABURENSIS(randomHeight, radius_base_m, 0.0, randomAge, pContaining_cell, int(z01()*20+1));
			
		}
		else if (randomStatus<=0.5) { // probability for being a sapling
			randomHeight = zZzo->uniform(0.3, 1.5);
			
			radius_base_m = 0.5*randomHeight; // estimate of M. Koechy // set static radius_base_m
			
			randomAge = int(round(zZzo->uniform(1, 7))); // Kaplan 2005, independent of height
			
			ithaburensis = new ITHABURENSIS(randomHeight, radius_base_m, 0.0, randomAge, pContaining_cell);
			
		}
	
		else { // probability for being an adult
			randomHeight = 1.5+Weibull(z01(), 2.0, 0.95);

			float random_base = 0.0; // typical tree growth form: stem without leaves
			float random_top = 0.44*randomHeight; // calculated from Schiller et al. 2007
			
			randomAge = int(round(8.0+Weibull(z01(), 50, 0.95))); // guess, shape from http://www.stat.sfu.ca/~dean/forestry/talks/BIRSHe.pdf, scale to fit max age of 300	
			
			ithaburensis = new ITHABURENSIS(randomHeight, random_base, random_top, randomAge, pContaining_cell);
			
		}
	}
	return ithaburensis;
}

/** LAI is related to crown area for consistency with herbs and shrubs
 */
float ITHABURENSIS::get_LAI_rel(month_names m) {
	float max_canopy_radius = radius_base_m > radius_top_m ? radius_base_m : radius_top_m;
	float canopy_area = 3.1416 * max_canopy_radius * max_canopy_radius;
	float LAI = canopy_area ? get_leaf_area_m2(m)/canopy_area : 0.0;
	return LAI;
}

/** Conversion from leaf mass (g) to leaf area (m<sup>2</sup>) including seasonal variation of deciduousness 
 (Kaplan & Gutman 1999) but ignoring seasonal variation of specific leaf mass (Liakoura et al. 2001) and effects 
 of watering (Gruenzweig et al. 2008)
*/
float ITHABURENSIS::get_leaf_area_m2(month_names month) {
	float leafiness = 1.0; // percentage of trees times percentage of leaves shed (Kaplan & Gutman 1999) avergaed across years;
	switch (month) {
		case Aug:
			break;
		case Sep:
			break;
		case Oct:
			leafiness = 0.75;
			break; // leaf shedding
		case Nov:
			leafiness = 0.63;
			break;
		case Dec:
			leafiness = 0.72;
			break; // start of budding
		case Jan:
			leafiness = 0.90;
			break;
		case Feb:
			leafiness = 0.95;
			break;
		case Mar:
			break;
		case Apr:
			break;
		case May:
			break;
		case Jun:
			break;
		case Jul:
			break;
	}
	return leafiness * 0.0083 * leaf_mass_g; // 83 cm2/g, Grünzweig et al. 2008; SLA of Q. douglasii is 0.006 m2/g, Karlik & McKay
}

bool ITHABURENSIS::grow_month(WATER *Wc, month_names month) {
	/** Seedlings grow in height from February to September (Gruenzweig et al. 2008, Fotelli et al. 2000). Height growth 
	 rates in relation to water availability are estimated from Gruenzweig et al. (2008). The growth increment of the 
	 canopy radius (0.135 cm/mo) is assumed to be independent of water supply and the same as for <i>Q. coccifera</i> 
	 (Bellot et al. 2002). Seedling roots (Q. cal. & Q ith.) attain a depth of 60 cm within two months after germination
	 (Oppenheimer 1940).
	*/
	bool desiccation=false;
	float t = get_transpiration_lm2mo(month);
	float w = Wc->get_mWater();
	float brw=0.0;
	
	if (height_m<=0.3) { // ************** SEEDLING GROWTH				
		brw = 0.0*Wc->bedrock_water_mm; // 10% of bed rock water is accessible to seedlings (my assumption)
		if (month>Jan || month<Oct) {
//			bool output=false;
//			if(height_m>0.28) {output=true; std::cout << w << "\t" << height_m << "\t" << individuals << "\t";}
			radius_top_m += 0.00135; // 1.35 mm = 1.1 cm from Feb to Sep
			if (w+brw-t>160)
				height_m += 0.06; // 6.0
			else if (w+brw-t>80)
				height_m += 0.03; // 3.0 cm
			else if (w+brw-t>40)
				height_m += 0.015; // 1.5 cm
			else if (w+brw<t*desiccation_sensitivity)
				desiccation=true;
//			if (output) std::cout << height_m << std::endl;
		}
		transpiration(Wc, month, t, 0.1);
	}
	/** Saplings are assumed to grow similar to seedlings, but growth is reduced by competition from adult trees 
	(Kaplan 2005). Kaplan assumed the oaks compete for water, but Bragg et al. (1993) found no evidence that oak 
	saplings in tallgrass prairie have lower xylem pressure than adult trees. Bragg et al. concluded nonetheless that 
	trees and saplings compete for water and that stress is lower with fewer neighbours. Here I am assuming competition 
	for water based on the demand by trees and seedlings (water is reduced via the function transpiration). I assume 
	that saplings have access to 60% of the  water in the soil profile based on their rooting depth compared to adult 
	trees.
	*/
	if (height_m > 0.3 && height_m <=1.5) { // *************** SAPLING GROWTH
		brw = 0.6*Wc->bedrock_water_mm; // 60% of bed rock water is accessible to saplings (my assumption)
		if (month>Jan || month<Oct) {
			radius_top_m += 0.00135; // 1.35 mm = 1.1 cm from Feb to Sep
			if (brw+w-t>160)
				height_m += 0.06; // 6.0
			else if (brw+w-t>80)
				height_m += 0.03; // 3.0 cm
			else if (brw+w-t>40)
				height_m += 0.015; // 1.5 cm
			else if (brw+w<t*desiccation_sensitivity)
				desiccation=true;
		}
		transpiration(Wc, month, t, 0.6);
	}
	/** Adult trees have ca. 4 cm twig growth per year (Kaplan &amp; Gutman 1999) without relation to rain 
	 amount. Since twigs grow at an angle, I assume that canopy radius increases only by 3 cm/yr.
	*/
	if (height_m>1.50) { // **************** ADULT GROWTH
		float z = z01();
		brw = 0.9 * Wc->bedrock_water_mm; // 90% of bed rock water is accessible to trees)
		
		switch (month) {
			case Feb:
				if (brw + w > t) {
					if (z>0.7) {
						radius_top_m += 0.03;
						height_m += 0.03;
					}
				} else if (brw+w<t*desiccation_sensitivity)
					desiccation=true;
				acorn_production(Wc->get_mRain());
				break;
			case Mar:
				if (brw + w > t) {
					if (z>0.2) {
						radius_top_m += 0.03;
						height_m += 0.03;
					}
				} else if (brw+w<t*desiccation_sensitivity)
					desiccation=true;
				break;
			case Apr:
				if (brw + w> t) {
					if (z>0.1) {
						radius_top_m += 0.03;
						height_m += 0.03;
					}
				} else if (brw+w<t*desiccation_sensitivity)
					desiccation=true;
				break;
			default:
				if (brw+w<t*desiccation_sensitivity)
					desiccation=true; break;
		}
		
		if (height_m > 10)
			height_m = 10; /* max height of Q. ithaburensis ssp. ithaburensis (Kaplan 2005),
							Q. ithaburensis ssp. macrolepis growing in Turkey and Greece may attain up to 15 m (WWW, 
							Papanastasis 2002)*/

		/* the average crown diameter observed by Schiller was 6.2 m (3.1 m radius), 
		 average crown projection area 34.4±13.6 m² (3.9 m radius)
		 */
		if (radius_top_m > 3.90)
			radius_top_m = 3.90;
		if (radius_base_m > 3.90)
			radius_base_m = 3.90;

		// copied from calliprinos:
		// set reasonable limits to tree size; Sternberg & Shoshany: raw data, Köchy: raw data		
		if (radius_top_m > 1.5 * height_m) radius_top_m = 1.5 * height_m;
		if (radius_base_m > 1.5 * height_m) radius_base_m = 1.5 * height_m;
		if (radius_top_m > 3.5) radius_top_m = 3.5;
		if (radius_base_m > 3.5) radius_base_m = 3.5;
		
		transpiration(Wc, month, t, 0.1); // steady-state
}
	
	update_shoot_mass();
	coverM_rel[int(month)] = 3.1416 * radius_top_m*radius_top_m*0.04; // this allows cover>1 when the tree is 
																	  // greater than one cell - not unlikely!
	heightM_m[int(month)] = height_m;
	feedM_kgcell[int(month)] = g_to_kg(get_feed_mass_g()*individuals);
	if(feedM_kgcell[int(month)] < 0) std::cerr << "ITHABURENSIS::get_feed_mass_g = " << get_feed_mass_g() << std::endl;
	LAIM_rel[int(month)] = get_LAI_rel(month);
	
	return desiccation;
}

void ITHABURENSIS::update_shoot_mass(void) {
	float DBH = 0.0;
	/** Conversion from height (m) to shoot dry mass (g) based on only two points (Fotelli et al. 2000) and 
	 regression through the origin.
	*/
	if (height_m < 0.3) // seedlings
		top_shoot_mass_g = 10.0 * height_m;
	else {
		/** Shoot mass of saplings and adult trees is calculated by first inversing a regression of tree height on DBH for 
		 <i>Quercus ithaburensis</i> and then 
		 converting to mass using a regression for <Q. conferta</i>.
		 */
		DBH = exp((log(height_m/1.004)-0.3)*2.0); // Schiller et al. 2007 for Q. ithaburensis, incl. 1 value from Papanastasis & Liakos 2002 (64;11)
		if (DBH * 0.5 < leaf_layer_depth)
			top_shoot_mass_g = 10.0 * height_m;
		else {
			top_shoot_mass_g = kg_to_g(0.145*pow(DBH, 2.282)); // from Q. macrocarpa: Ter-Mikaelian & Korzukhin 1997, no data on Q. ith. or Q. douglasii
		}
	}
	update_leaf_mass(DBH);
}

void ITHABURENSIS::update_leaf_mass(float DBH) {
	/** Conversion from total shoot dry mass (g) to leaf dry mass (g) (Gruenzweig et al. 2008).
	*/
	if (height_m < 0.3) {// seedlings
		leaf_mass_g = 0.4 * top_shoot_mass_g;
		edible_mass_g = top_shoot_mass_g; 
	}
	else {
		if (height_m < 1.5) {// saplings
			leaf_mass_g = 0.4 * top_shoot_mass_g;
			/** Leaf mass is 3% of total aboveground mass based on allometric regressions of aboveground mass 
			 on DBH for <i>Quercus macrocarpa</i> (Ter-Mikaelian &amp; Korzukhin 1997), and regressions of dry 
			 leaf mass on DBH for <i>Quercus douglasii</i> (Karlik &amp; McKay 2002). The leaf mass ratio of 3% 
			 is the same as for <i>Q. pyrenaica</i> (Santa Regina 2000).
			*/
			edible_mass_g = top_shoot_mass_g; 
		}
		else {// adults, no browsing because leaves are above browsing height
			leaf_mass_g = exp(2.0+2.2*log(DBH))*1.017; // Karlik & MacKay 2002: Q. douglasii; no data on Q. ith. or Q. conferta
			/* no data on Q. ith. or Q. conferta, Q. macrocarpa has similar stature as Q. ithaburensis and leaf mass 
			 equation has exponent <1 */
			edible_mass_g = 0.0; 
		}
	} // end else
}
	
/** Calculation of transpiration (litres per square metre per day) per individual depending on leaf mass (g) per tree. 
Data from Schiller et al. 2007, Fig. 6a.
*/

float ITHABURENSIS::get_transpiration_lm2mo(month_names m) {
	float t = 0.0;
	switch (m) {
		case Aug: t=0.30; break;
		case Sep: t=0.25; break;
		case Oct: t=0.18; break;
		case Nov: t=0.13; break;
		case Dec: t=0.10; break;
		case Jan: t=0.09; break;
		case Feb: t=0.09; break;
		case Mar: t=0.13; break;
		case Apr: t=0.26; break;
		case May: t=0.34; break;
		case Jun: t=0.36; break;
		case Jul: t=0.35; break;
	}
	return cell_to_m2(t * leaf_mass_g);
}

/** Reduction of water stored in soil profile by the amount of water used. This is an underestimate because the tree 
takes up more water than it transpires. (The rest is used in chemical reactions.) 
*/
void ITHABURENSIS::transpiration(WATER * Wc, month_names m, float t, float bw_ratio) {
	t*=q1_cell_area_m2;
	
	float transpired = min(t*(1-bw_ratio), Wc->monthly_infiltr_water_mm); // remove water from upper soil layer first
	Wc->monthly_infiltr_water_mm -= transpired;
	
	t-=transpired;
	transpired = min(t, Wc->bedrock_water_mm);
	Wc->bedrock_water_mm -= transpired;

	//std::cout << "transpiration ITH\n";
}

/* Seed number depends on seed mass and leaf mass (Greene &amp; Johnson 1994). Since acorns of <i>Q. ithaburensis</i> 
ripen for 20-22 months (Feb - Oct, Kaplan &amp; Gutman 1999), the pollination success of the previous year is decisive 
(Knops et al. 2007). Seed mass of <i>Q. ithaburensis</i> is 8.13 g (Gruenzweig et al. 2008) to 10 g (Rababah et al. 2008)
*/
void ITHABURENSIS::acorn_production(float mrain) {
	if(age > 25) { // average maturity of oaks
	float pollination_factor = 1.0; 
		/* an average February has about 100 mm of rain in Yehudia Nature Reserve (500 mm MAP); 
		   pollination probability is higher in dry years (Knops et al. 2007).
		 */
	
	if (mrain < 50)
		pollination_factor = 1.5; // my estimate
	else if (mrain > 150)
		pollination_factor = 0.5; // my estimate
	
	acorns[0] = (unsigned long int) (pollination_factor * 0.6 * pow(12.0, -0.58) * pow(leaf_mass_g, 1.08)); 
		/* the second term adapted to fit approximately seed production of Q. ithaburensis (Kaplan 2005) 
		 1000 acorns/tree and average tree hight (5-6 m, Kaplan 1984). The factor is larger than the one (0.011) in 
		 Greene & Johnson 1994. */
	}
}

/** Access function for biomass that could be eaten by sheep or goats. 
*/
float ITHABURENSIS::get_feed_mass_g(void) {
	return edible_mass_g;
}

float ITHABURENSIS::removeMassByForaging(month_names month, float demand) {
	float consumed_g = 0.0;
	/** Browsing and grazing have the same effect on seedlings (Dufour-Dror 2007). Canopy diameter is 
	 assumed to be reduced to zero because all leaves are eaten. The function returns the amount of 
	 biomass eaten. This might be less than the demand when the plant is small.
		*/
	if (height_m <= 0.3) {
		if (demand > edible_mass_g * individuals)
			demand = edible_mass_g * individuals;
		browse_history[0] += 1.0;
		consumed_g = demand; // MK: changed 200110503 to increase effect of grazing
		
		/// the eaten volume
		float volume = get_browse_layer_volume_m3() * consumed_g/float(edible_mass_g * individuals);
		float a = 1.0 - volume/get_browse_layer_volume_m3();
		a = a>0.0 ? a : 0.0; // don't let 'a' be negative
		radius_base_m = pow(a, q1_3) * radius_base_m;
		height_m *= a;
	}
	/** Saplings are browsed as much as seedlings but they have stronger stems, so there is more remaining.
		*/
	if (height_m >0.3 && height_m <= 1.5) {
		
		if (demand > edible_mass_g)
			demand = edible_mass_g;
		browse_history[0] += 1.0;
		consumed_g = demand;
		
		// the eaten volume
		float volume = get_browse_layer_volume_m3() * consumed_g/edible_mass_g;
		float a = 1.0 - volume/get_browse_layer_volume_m3();
		a = a>0.0 ? a : 0.0; // don't let 'a' be negative
		radius_base_m = pow(a, q1_3) * radius_base_m;
		height_m *= a;
	} else
		/** Adult trees have stems &gt;150 cm so that goats and sheep can't reach the leaves. 
		 Therefore, the function always returns 0.0.*/
		consumed_g = 0.0; // if height_m > 1.5m no browsing!
	return consumed_g;
}

bool ITHABURENSIS::mortality(float herb_mass_gm2, int NumberOfAdultTreeIndividualsInCell, int NumberOfSaplingsInCell) {
	bool erase = false;
	int browseSum = browse_history.sum();
	
	/** Stochastic determination of seedling mortality. Primary cause of mortality is competition by herbaceous plants 
	for water, increased by higher probability of fire kill and browsing when herbaceous neighbours are around. As a 
	consequence, overall mortality is high in grassland but low under adult oaks (Gruenzweig et al. 2004, 2008, 
	Kaplan 2005). Competitive effects of herbeaceous vegetation on oak seedlings (Q. douglasii) occur at 400 g/sqm 
	(Gordon & Rice 1993). This mass corresponds to the herbaceous productivity in burned and grazed Q. ithaburensis 
	parkland (Noy-Meir 1995). These conditions are sufficient to increase the survival rate of Q. ithaburensis 
	(Kaplan 2005). Repeated clipping (every 2 weeks May-September for four years) kills a Q. calliprinos tree 
	(Tsiouvaras 19988).
	*/
	//TODO: check consistency with fire and grazing module
	if (height_m<=0.3) {		
		if (browseSum > 10) // lower value for seedlings
			individuals = 0; // 2/month * 5 months * 4 years, Tsiouvaras (1988)
		else
			individuals = individuals * (10-browseSum)/10.0;
		
		for (int n = 1; n < individuals; n++) {
			float z = z01();
			
			if (herb_mass_gm2 < 225) { // biomass data estimated from Fig. 4, Wadiscape paper
				if (z>pow(0.99,200*NumberOfAdultTreeIndividualsInCell+10*NumberOfSaplingsInCell+individuals))
					individuals--;
			} else if (herb_mass_gm2 < 250) { // 0.015<LSU<0.275
				if (z>pow(0.95,200*NumberOfAdultTreeIndividualsInCell+10*NumberOfSaplingsInCell+individuals))
					individuals--;
			} else if (z>pow(0.9,200*NumberOfAdultTreeIndividualsInCell+10*NumberOfSaplingsInCell+individuals)) // >0.275 LSU
				individuals--;
		}
		if (individuals < 1) erase = true;
	}
	/** Saplings no longer compete with herbs.
		*/
	if (height_m>0.3 && height_m<=1.5) {
		float z = z01();
		
		if (z > pow(0.99, 200*NumberOfAdultTreeIndividualsInCell+NumberOfSaplingsInCell)) {// MK 20110318: increased factor for number of adults from 0.5 to 200
			erase = true; // >0.275 LSU
			//std::cout << "ith sapling died" << "\t" << z << "\t" << NumberOfAdultTreeIndividualsInCell << "\n";
		}
		
		z=z01();
		if (z<square(browseSum/40.0)) // 2/month * 5 months * 4 years, Tsiouvaras (1988)
			erase = true;
	}
	/** Adult trees have deep roots and are unlikely to die from water stress 
		(Schiller et al. 2007, Bragg et al. 1993). The only cause of death 
		considered here is random mortality and, in function adult_burn(), fire.
		*/
	if (height_m>1.5) {
		float z = z01();
		if (age < 300) {
			if (z > pow(0.999, 200*(NumberOfAdultTreeIndividualsInCell-1))) // MK20110318: increased tree competition
				erase = true; // random mortality, assumption
		} else if (z>pow(0.995, double(age)))
			erase = true; // age-dependent mortality, my guess
	}
	return erase;
}

// no longer used as of 6.4.1
/*
 ITHABURENSIS* ITHABURENSIS::initialize_new_seedling(CELL* pContaining_cell, int n) {
	ITHABURENSIS* pIthaburensis= NULL;
	pIthaburensis = new ITHABURENSIS(0.05, 0.07, 0.0, 0, pContaining_cell, n);
	return pIthaburensis;
}
 */

// no longer used as of 6.4.1
/*
 ITHABURENSIS* ITHABURENSIS::initialize_new_sapling(CELL* pContaining_cell, TREE* pITH) {
	ITHABURENSIS* pIthaburensis= NULL;
	pIthaburensis = new ITHABURENSIS(pITH->get_height_m(), pITH->radius_base_m, 0, 
	pITH->get_age(), pContaining_cell, 1);
	return pIthaburensis;
}
*/

bool ITHABURENSIS::burnTree(float herb_mass_kgm2) {
	bool erase = false;
	if (height_m < 0.3) { /** Seedlings are less affected by fire when they grow in sparse vegetation like on cairns (Kaplan 2005, Tab. 2), 
		for which I assume a productivity of 175 g/m2, approximately the productivity in the dry Mediterranean region. */
		for (int n = 0; n < individuals; n++) {
		float z = z01();
		if (herb_mass_kgm2 < 0.175) {
			if (z<0.8) // mortality by fire in sparse vegetation, as on cairn tops, Kaplan 2005, Tab.7; increased from 0.3
					individuals--;
			else {
				height_m *= 0.7; // smaller effect on surviving plants, my estimate
				//				canopy_radius_m = 0.01; // 1 cm
			}
		} else {
			if (z<0.9) // mortality by fire in dense vegetation, Kaplan 2005, Tab.7; increased from 0.7
					individuals--;
			else {
				height_m *= 0.1; // stronger effect on surviving plants, my estimate
				//				canopy_radius_m = 0.01; // 1 cm
			}
			
		}
			if(individuals < 1) erase = true;
		}
	}
	else if (height_m < 1.5) { /** Saplings are less affected by fire when they grow in sparse vegetation like on cairns (Kaplan 2005). 
		The effect of fire is smaller than on seedlings.*/
		float Z = z01();
		if (Z < 0.5) // estimate, between seedlings (50%) and adults (7 %)
			erase = true;
		else {
		float z = (z01()-0.5)/5.0; // [-0.1,+0.1)
		if (herb_mass_kgm2 < 0.175)// in sparse vegetation, as on cairns
		{
			height_m *= 0.9+z;
			radius_base_m *= 0.9+z;
		} else {
			height_m *= 0.5+z;
			radius_base_m *= 0.5+z;
		}
		}
	}
	else { /** Fire affects trees only in so far as all or only the lower leaves are burned (Kaplan 2005, Kaplan &amp; Gutman 1999). 
		Fire damage is less in grazed plots because of less fuel.  Leaves resprout within one month. */
		float z = z01();
		if (z > 0.993) // 0.7% of trees killed by fire (Kaplan 2005)
			erase = true;
		else {
			if (herb_mass_kgm2 < 0.175) // in sparse vegetation
			{
				if (z<0.4)
					leaf_mass_g *= 0.5;
				else if (z<0.42)
					leaf_mass_g = 0.0;
			} else // in dense vegetation
			{
				if (z<0.4)
					leaf_mass_g *= 0.5;
				else if (z<0.8)
					leaf_mass_g = 0.0;
			}
		}
	}
	update_shoot_mass();
	months_since_fire = 0;
	return erase;
}

bool ITHABURENSIS::cutTree(float cut_height_m){
	
	if(height_m > 1.2* cut_height_m) {
		height_m = cut_height_m;
		radius_top_m = 0.0;
		age = 5;

		update_shoot_mass();
	}
	return true;
}


species_names ITHABURENSIS::getTreeType(void) {
	return Q_ithaburensis;
}

