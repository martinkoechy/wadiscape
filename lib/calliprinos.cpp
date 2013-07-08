/*
 *  calliprinos.cpp
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include "calliprinos.h"
#include "water.h"
#include "cell.h"
#include "main.h"

extern RNG* zZzo;

float CALLIPRINOS::desiccation_sensitivity = 1.0;

CALLIPRINOS::CALLIPRINOS(void) {
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

CALLIPRINOS::CALLIPRINOS(CELL* pContaining_cell): TREE(pContaining_cell) {
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

CALLIPRINOS::CALLIPRINOS(float height, float radius_base, float radius_top, int newAge, CELL* p_containing_cell, int n=1)
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

CALLIPRINOS* CALLIPRINOS::initialize(climate_names c, CELL* pContaining_cell) { //initial distribution
	float randomCalliprinos, randomStatus;
	CALLIPRINOS* calliprinos= NULL;
	
	float occupancy = 0.0;
	switch (c) {
		case arid: occupancy = perCell; break;
		case semiarid: occupancy = perCell; break;
		case drymed: occupancy = 0.1; break;
		case typmed: occupancy = 0.5; break;
		case mesicmed: occupancy = 0.7; break;
	}
	
	randomCalliprinos = z01();
	float randomHeight;
	int randomAge;
	
	if (randomCalliprinos <= occupancy) {
		randomStatus = z01();
		if (randomStatus<=0.1) { // Wahrscheinlichkeit, dass es ein SEEDLING ist
			randomHeight = zZzo->uniform(0.05, 0.3);
			
			radius_base_m = max(0.02, -0.08 + 0.53*randomHeight); // based on aggregated data from Sternberg & Shoshany 2001, Engel & Frey 1996, Koechy measured 2007
			
			randomAge = int(round(zZzo->uniform(0, 3))); // independent of height
			
			calliprinos = new CALLIPRINOS(randomHeight, radius_base_m, 0.0,
										  randomAge, pContaining_cell, int(z01()*20+1));
		}
		else if (randomStatus<=0.3) { // probability for being a sapling
			randomHeight = zZzo->uniform(0.3, 1.5);
			radius_base_m = -0.08 + 0.53*randomHeight; // as for seedlings
			
			randomAge = int(round(zZzo->uniform(1, 7))); // Kaplan 2005, independent of height
			
			calliprinos = new CALLIPRINOS(randomHeight, radius_base_m, 0.0,
										  randomAge, pContaining_cell);
			
		}
		else { // probability for being an adult
			randomHeight = 1.5+Weibull(z01(), 2.0, 0.95);
			
			radius_base_m = -0.08 + 0.53*randomHeight; // as for seedlings
			radius_top_m = -0.08 + 0.53*randomHeight; // as for seedlings
			
			randomAge = int(round(8.0+Weibull(z01(), 50, 0.95))); // guess, shape from http://www.stat.sfu.ca/~dean/forestry/talks/BIRSHe.pdf, scale to fit max age of 300	
			
			calliprinos = new CALLIPRINOS(randomHeight, radius_base_m,
										  radius_top_m, randomAge, pContaining_cell);
			
		}
	}
	return calliprinos;
}

/** LAI is related to crown area for consistency with herbs and shrubs
 */
float CALLIPRINOS::get_LAI_rel(month_names m) {
	float max_canopy_radius = radius_base_m > radius_top_m ? radius_base_m : radius_top_m;
	float canopy_area = 3.1416 * max_canopy_radius * max_canopy_radius;
	float LAI = canopy_area ? get_leaf_area_m2(m)/canopy_area: 0.0;
	
	//std::cout << "calliprinos: h: " << height_m << "\tr:" << max_canopy_radius << "\tm:" <<base_shoot_mass_g << "\t" << "\tLA:" << get_leaf_area_m2(m)<< "\tCA:" << canopy_area << std::endl;

	return LAI;
}

/** Conversion from leaf mass (g) to leaf area (m<sup>2</sup>). Leaves stay on the tree for 13-14 months (Orshan 1989). 
 Seasonal variation of specific leaf mass and effects of watering (Gruenzweig et al. 2008, Baquedano &amp; Castillo 2006) 
 is ignored. The value used is the median of SLA reported by Gruenzweig et al. 2008, Baquedano &amp; Castillo 2006, 
 Antunez et al.2001, Bellot et al. 2001, Castro-Diez et al. 2006. Only Gruenzweig et al. refer directly to <i>Q. 
 calliprinos</i>, the other studies used the closely related <i>Q. coccifera</i>. Using eq. 5 and 6 of Schiller et al. 
 2003 for Q. calliprinos, the SLA is 0.0023!
*/
float CALLIPRINOS::get_leaf_area_m2(month_names month) {
	/* the tree is evergreen, therefore 'leafiness' is not considered here in contrast to Q. ithaburensis.*/
	
	return 0.006 * leaf_mass_g;
}

float CALLIPRINOS::get_aboveground_mass_g(void) {
	return (base_shoot_mass_g + top_shoot_mass_g);
}

/* The growth of calliprinos with values obtained from the literature produces a declining number of saplings and adults.
I have increased the growth of seedlings and saplings and the access to bedrock water of saplings.
 */
bool CALLIPRINOS::grow_month(WATER *Wc, month_names month) {
	months_since_fire++;
	bool desiccation = false;
	float t = get_transpiration_lm2mo(month);
	float w = Wc->get_mWater();
	float brw = 0.0;

	/** Seedlings grow in height from April to October (Gruenzweig et al. 2008, Orshan 1989). Height growth rates in 
	 relation to water availability are estimated from Gruenzweig et al. (2008). The growth rate of the canopy radius 
	 is according to allometric relationships based on data from Sternberg &amp; Shoshany 2001, Engel &amp; Frey 1998, 
	 and Koechy, unpubl. data, 1997. The growth increment of the canopy radius is according to allometric relations. 
	 Seedling roots (Q. cal. & Q ith.) attain a depth of 60 cm within two months after germination, Q. cal. reaches 80 cm 
	 after one year, and 1.6 m after 2 years (Oppenheim 1940).
	*/
	if (height_m < 0.30) { // *************** SEEDLING GROWTH
		brw = 0.1*Wc->bedrock_water_mm; // 10% of bed rock water is accessible to seedlings (my assumption)
		if (month>Mar || month<Nov) {
//			bool output=false;
//			if(height_m>0.29) {output=true; std::cout << w << "\t" << height_m << "\t" << individuals << "\t";}

			if (w+brw-t>160) {
				height_m += 0.04; // 4 cm
				radius_base_m += 0.042;
			} else if (w+brw-t>80) {
				height_m += 0.02; // 2 cm
				radius_base_m += 0.021;
			} else if (w+brw-t>40) {
				height_m += 0.01; // 1 cm
				radius_base_m += 0.0105;
			} else if (w+brw<t*desiccation_sensitivity) // if available water < transpiration
				desiccation=true;
//			if (output) std::cout << height_m << std::endl;
		}
		transpiration(Wc, month, t, 0.1);

	} else if (height_m < 1.50) { /** Saplings can grow 2 cm/mo in height (Gruenzweig et al. 2008), and probably more in 
		summer like trees (Orshan 1989). They produce long shoots after fire (Konstantinidis et al. 2005). They do not 
		produce longer shoots after browsing (Perevolotsky &amp; Haimov 1992). The growth rate of the canopy radius is 
		according to allometric relationships based on data from Sternberg &amp; Shoshany 2001, Engel &amp; Frey 1998, 
		and Koechy, unpubl. data, 1997. Competition is implemented as for <i>Q. ithaburensis</i>. CHECK.*/
		
		brw = 0.6*Wc->bedrock_water_mm; // 60% of bed rock water is accessible to saplings (my assumption)
		/* height growth perhaps too little, factor for tuning added 20110517, MK*/
		float gtf=1.0; // growth tuning factor
		float f = months_since_fire < 6 ? 10.0: 1.0; // factor for overcompensation
		if (month>Mar || month<Nov) // height growth greater than for seedlings
		{
			if (brw+w-t>160) {
				height_m += 0.04*f*gtf; // 4 cm
				radius_base_m += 0.042*gtf;
			} else if (brw+w-t>80) {
				height_m += 0.02*f*gtf; // 2 cm
				radius_base_m += 0.021*gtf;
			} else if (brw+w-t>40) {
				height_m += 0.01*f*gtf; // 1 cm
				radius_base_m += 0.0105*gtf;
			} else if (brw+w<t*desiccation_sensitivity)
				desiccation=true;
			
			if (month>Jun || month<Oct) /* summer height growth similar to mature trees, but less change in canopy width 
											and lower probability*/
			{
				float z = z01();
				if (z<0.06){
					height_m += 0.3; // 30 cm
					radius_base_m += 0.05;
				}
				else if (z<0.03) {
					height_m += 0.15; // 15 cm
					radius_base_m += 0.03;
				}
				else if (z<0.01) {
					height_m += 0.1; // 10 cm
				radius_base_m += 0.02;
				}
		}
		transpiration(Wc, month, t, 0.6);
		
		}
		if (height_m >= 1.5) // when a sapling grows into the adult class, the crown radius remains the same
			radius_top_m = radius_base_m;

	} else { 
		/** 
		Trees have two growth periods: April-May and July-September. 
		"Stem elongation is limited in spring but in summer the new branches may 
		exceed 1 m" (Orshan 1989). Growth in summer is restricted to a few 
		branches in some trees in the upper part of the canopy (Orshan 1989). 
		Stem/canopy elongation in spring is about 3-4 cm (M. Koechy, pers. obs., 
		Jordan 2007). The ratio of width/height growth is based on allometric 
		relationships according to data by Sternberg &amp; Shoshany 2001, Engel
		&amp; Frey 1998, and Koechy, unpubl. data, 2007. Shrub forms grow up to 
		4 m, tree forms up to 15 m in height (Orshan 1989, Konstantinidis et al.
		2005).*/
		float z = z01();
		brw = 0.9* Wc->bedrock_water_mm; // 90% of bed rock water is accessible to trees)
		switch (month) {
			case Apr:
				if (brw + w > t) {
					radius_top_m += 0.021;
					radius_base_m += 0.021;
					height_m += 0.02;
				}  else if (brw+w<t*desiccation_sensitivity)
					desiccation=true;
				break;
			case May:
				if (brw + w> t) {
					radius_top_m += 0.021;
					radius_base_m += 0.021;
					height_m += 0.02;
				} else if (brw+w<t*desiccation_sensitivity)
					desiccation=true;
				acorn_production(Wc->get_mRain());
				break;
			case Jul: // height growth in summer up to 1 m: Orshan 1989, here only 30 cm are assumed, otherwise too tall too fast
				if (brw + w > t && z < 0.05) {
					radius_top_m += 0.05;
					radius_base_m += 0.05;
					height_m += 0.1;
				} else if (brw+w<t*desiccation_sensitivity)
					desiccation=true;
				break;
			case Aug:
				if (brw + w > t && z < 0.05) {
					radius_top_m += 0.05;
					radius_base_m += 0.05;
					height_m += 0.1;
				} else if (brw+w<t*desiccation_sensitivity)
					desiccation=true;
				break;
			case Sep:
				if (brw + w > t && z < 0.05) {
					radius_top_m += 0.05;
					radius_base_m += 0.05;
					height_m += 0.1;
				} else if (brw+w<t*desiccation_sensitivity)
					desiccation=true;
				break;
			default:
				if (brw+w<t*desiccation_sensitivity)
					desiccation=true;
		}
	transpiration(Wc, month, t, 0.9);
// set reasonable limits to tree size; Sternberg & Shoshany: raw data, Köchy: raw data
		if (height_m > 15.0) height_m = 15.0; // limit to average max height
		if (radius_top_m > 1.5 * height_m) radius_top_m = 1.5 * height_m;
		if (radius_base_m > 1.5 * height_m) radius_base_m = 1.5 * height_m;
		if (radius_top_m > 3.5) radius_top_m = 3.5;
		if (radius_base_m > 3.5) radius_base_m = 3.5;
	}
	update_shoot_mass();
	
	float max_r = radius_base_m > radius_top_m ? radius_base_m : radius_top_m;
	coverM_rel[int(month)] = 3.1416 * max_r*max_r*0.04;
	heightM_m[int(month)] = height_m;
	feedM_kgcell[int(month)] = g_to_kg(get_feed_mass_g()*individuals);
	if(feedM_kgcell[int(month)] < 0) 
		std::cerr << "CALLIPRINOS::get_feed_mass_g = " << get_feed_mass_g() << ", height: " << height_m << 
		", radius base: " << radius_base_m << ", radius top: " << radius_top_m << std::endl;

	LAIM_rel[int(month)] = get_LAI_rel(month);	
	return desiccation;
}

void CALLIPRINOS::update_shoot_mass(void) {
	float Circum50 = 0.0;
	/** Conversion from height (m) to shoot dry mass (g) based on the ratio for 'water-stressed' seedlings in Baquedano
	&amp; Castillo (2006) maintained at 0.4 MPa soil water potential.
	*/
	if (height_m < 0.3) // seedling
		base_shoot_mass_g = 20.0 * height_m;
	/** Saplings do it either like seedlings or like trees.
	*/
	else if (height_m <= 1.5) { // sapling
		if (radius_base_m < leaf_layer_depth)
			base_shoot_mass_g = 20.0 * height_m;
		else
			base_shoot_mass_g = exp(8.0 + 0.9 * log(qpi_3 * square(radius_base_m) * height_m))*1.185;
	}
	/** Shoot mass of adult trees was calculated from the raw data used by Sternberg & Shoshany (2001a) 
		(Sternberg, pers. comm) and Engel & Frey 1996.
		
		*/
	else { // adult, shape of an inverted cone
		if (radius_top_m <= 0) // TODO: at some point the updating to adulthood does not seem to work
			radius_top_m = radius_base_m;
		
		top_shoot_mass_g = exp(8.0 + 0.9 * 
							   log(qpi_3 * square(radius_top_m ) * height_m)) * 1.185 -  // total_shoot_mass
						   exp(8.0 + 0.9 *
							   log(qpi_3 * square(radius_top_m/height_m*1.5) * 1.5)) * 1.185; // - base_shoot_mass

		base_shoot_mass_g = exp(8.0 + 0.9 * 
								log(qpi_3 * square(radius_top_m/height_m*1.5) * 1.5)) * 1.185;
	}

	//conversion from mass to D50 from Ogaya et al. 2003, Quercus ilex, Spain
	//conversion of D50 to C50 by multiplication with pi.
	Circum50 = pi * exp((log(top_shoot_mass_g+base_shoot_mass_g)-4.9)/2.277);
	update_leaf_mass(Circum50);
}

void CALLIPRINOS::update_leaf_mass(float Circum50) {
	
	/** Conversion from total shoot dry mass (g) to leaf dry mass (g) (Gruenzweig et al. 2008). The LMR of <i>Q. 
	 calliprinos</i>seems to be higher than that of <i>Q. coccifera</i> (Antunez et al. 2001, Bellot et al. 2001[Fig. 8], 
	 Baquedano &amp; Castillo 2006, Sanz Perez et al. 2007, Castro-Diez et al. 2006).
	*/
	if (height_m < 0.3) { // seedling
		leaf_mass_g = 0.56 * base_shoot_mass_g;
		edible_mass_g = base_shoot_mass_g; 
	}
	/** Saplings do it either like seedlings or like trees.
	*/
	else if (height_m < 1.5) { // sapling
		if ((radius_base_m < leaf_layer_depth) | (base_shoot_mass_g < 67.1)) // the latter criterion just prevents negative values in the next function
			leaf_mass_g = 0.56 * base_shoot_mass_g; // like seedlings
		else
			leaf_mass_g = -494.0 + 117.0*log(base_shoot_mass_g); // like branches <140 cm (Sternberg & Shoshany 2001a)
		edible_mass_g = base_shoot_mass_g; 
	}
	else {// adult
		leaf_mass_g = 0.4492*pow(Circum50,2.518); // Schiller et al. 2003, eq. 5, Q. calliprinos
		edible_mass_g = max(0.0, -494.0 + 117.0*log(base_shoot_mass_g));
	}
}

/** Calculation of transpiration (litres per square metre per day) per individual 
depending on leaf mass (g) per tree. Data from Schiller et al. 2003, eq.5, 7, Fig. 6-8.
*/
float CALLIPRINOS::get_transpiration_lm2mo(month_names month) {
	float f = 0.0; // monthly sum of daily factors of transpiration
	switch (month) {
		case Aug: f=0.062; break;
		case Sep: f=0.033; break;
		case Oct: f=0.013; break;
		case Nov: f=0.007; break;
		case Dec: f=0.021; break;
		case Jan: f=0.055; break;
		case Feb: f=0.117; break;
		case Mar: f=0.149; break;
		case Apr: f=0.158; break;
		case May: f=0.148; break;
		case Jun: f=0.125; break;
		case Jul: f=0.097; break;
	}
	return cell_to_m2(f * 0.840*pow(10.0, 0.0294 * pow(leaf_mass_g/0.4492, 1.0/2.518))); 
	// inverse of eq 5 (leaf mass to stem circumference) inserted into eq 7 (stem circumference to transpiration)
}

/** Reduction of water stored in soil profile by the amount of water used. This 
 is an underestimate because the tree takes up more water than it transpires. 
 The rest is used in chemical reactions. I assume based on Seyfried et al. that 
 there is no deep drainage. In contrast to ithaburensis, water is removed only from deeper layers.
*/
void CALLIPRINOS::transpiration(WATER * Wc, month_names m, float t, float bw_ratio) {
	t*=q1_cell_area_m2;
	float transpired = min(t*(1-bw_ratio), Wc->monthly_infiltr_water_mm); // remove water from upper soil layer first
	Wc->monthly_infiltr_water_mm -= transpired;
	
	t-=transpired;
	transpired = min(t, Wc->bedrock_water_mm);
	Wc->bedrock_water_mm -= transpired;
	// std::cout << "transpiration CAL\n";
}

/** Seed number depends on seed mass and leaf mass (Greene &amp; Johnson 1994). Since acorns of 
 <i>Q. calliprinos</i> ripen for 18 months (May - Oct, Orshan 1989), the pollination success of 
 the previous year is decisive (Knops et al. 2007). Seed mass of <i>Q. calliprinos</i> varies between 3.6 g 
 (Grünzweig et al. 2008) and 6 g (Rababah et al. 2008)
*/
void CALLIPRINOS::acorn_production(float mrain) {
	if(height_m > 2.5) { // Shmida 1980 in Kaplan 1984
		float pollination_factor = 1.0; // an average February has about 100 mm 
										// of rain in Yehudia Nature Reserve 
										// (500 mm MAP); pollination probability 
										// is higher in dry years (Knops et al. 2007).
		if (mrain < 50)
			pollination_factor = 1.5; // my estimate
		else if (mrain >150)
			pollination_factor = 0.5; // my estimate
		
		acorns[0] = (long unsigned int)(pollination_factor * 0.4 * pow(4.0, -0.58) * pow(leaf_mass_g, 1.08)); 
		/* second term (0.4) set to fit approximately the seed production of oaks in Jordan (10 kg/tree,  Rababah et al.
		 2008). Avg. height 3-5 m (Köchy, pers. obs. Jordan, 2007). 
		 The factor is larger than the one (0.011) in Greene & Johnson 1994. But it is the same order of magnitude as
		 that used for Q. ithaburensis.
		 Nowar et al. 1994: 2500 ha * 400 trees/ha * 20 kg/tree (Agricultural Statistical Yearbook 1980)
		 Rababah et al. 2008: 50000 ha * 300 trees/ha * 10 kg/tree
		 10 kg ≈ 10000/4 = 2500 acorns
		 */
	}
}

/** Access function for biomass that could be eaten by sheep or goats
 */
float CALLIPRINOS::get_feed_mass_g(void) {
	return edible_mass_g;
}

float CALLIPRINOS::removeMassByForaging(month_names month, float demand) {
	float consumed_g = 0.0;
	/** Browsing and grazing have the same effect on seedlings (Dufour-Dror 2007). 
	 Canopy diameter is assumed to be reduced to 0.2 cm because all leaves are eaten. The function returns the amount of 
	 biomass eaten. This may be less than the demand when the plant is small.
		*/
	if (height_m <=0.3) {
		if (demand > edible_mass_g * individuals)
			demand = edible_mass_g * individuals;
		consumed_g = demand;
		browse_history[0] += 1.0;
		
		/// the eaten volume
		float volume = get_browse_layer_volume_m3() * consumed_g/float(edible_mass_g * individuals);
		
		float a = 1.0 - volume/get_browse_layer_volume_m3();
		a = a>0.0 ? a : 0.0; // don't let 'a' be negative
		radius_base_m = pow(a, q1_3) * radius_base_m;
		height_m *= a;
	}
	/** Saplings are browsed as much as seedlings but they have stronger stems, so there is more remaining. In addition,
	 <i>Q. calliprinos</i> often grows like a shurb, so even low individuals may have a wide canopy. The canopy 
	 reduction of 2 cm was estimated by considering the bite depth of goats and the reduction of canopy diameter after 
	 four years (40 cm, Perevolotsky &amp; Haimov 1992).
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
	}
	/** Only the leaves in the lower 150 cm of a tree can be browsed.
		*/
	if (height_m > 1.5) {
		if (demand > edible_mass_g)
			demand = edible_mass_g;
		consumed_g = demand;
		
		/// the eaten volume
		float volume = get_browse_layer_volume_m3() * consumed_g/edible_mass_g;
		float a = 1.0 - volume/get_browse_layer_volume_m3();
		a = a>0.0 ? a : 0.0; // don't let 'a' be negative
		radius_base_m = pow(a, (float)(1.0/2.0)) * radius_base_m;
	}
	update_shoot_mass();
	return consumed_g;
}

bool CALLIPRINOS::mortality(float herb_mass_gm2, int NumberOfAdultTreeIndividualsInCell, int NumberOfSaplingsInCell) {
	bool erase = false;
	int browseSum = browse_history.sum();
	/** Stochastic determination of seedling mortality. Values of survival for well watered individuals are 
	 assumed to be the same as for <i>Q. ithaburensis</i> since reported values vary considerably 
	 (94-99.5%/mo, Sanchez-Gomez et al. 2006, Bellot et al. 2001, Valladares &amp; Sanchez-Gomez 2006) 
	 and include the values for <i>Q. ithaburensis</i>. Values for stressed plants are in the range 
	 of 80-85%, lower than those reported for <i>Q. ithaburensis</i>. Repeated clipping (every 2 weeks 
	 May-September for four years) kills a Q. calliprinos tree (Tsiouvaras 1988). 
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
				if (z>pow(0.99, 200*NumberOfAdultTreeIndividualsInCell+10*NumberOfSaplingsInCell+individuals))
					individuals--;
			} else if (herb_mass_gm2 < 250) {
				if (z>pow(0.95, 200*NumberOfAdultTreeIndividualsInCell+10*NumberOfSaplingsInCell+individuals))
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
		if (z>pow(0.99, 200*NumberOfAdultTreeIndividualsInCell+NumberOfSaplingsInCell)) {// MK 20110526: increased factor for number of adults from to 200
			erase = false;
			//std::cout << "cal sapling died\n";
		}
		z=z01();
		if (z<square(browseSum/40.0)) // 2/month * 5 months * 4 years, Tsiouvaras (1988)
			erase = true;
	}
	/** Adult trees have deep roots (CHECK!!) and are unlikely to die from water stress (Schiller et al. 2007,
	 Bragg et al. 1993). The only cause of death considered here is random mortality and, 
	 in function adult_burn(), fire.
		*/
	if (height_m>1.5) {
		float z = z01();
		if (age < 300) { // my guess
			if (z > pow(0.999, 200*(NumberOfAdultTreeIndividualsInCell-1))) // MK20110318: increased tree competition
				erase = true; // random mortality, assumption
		} else if (z>pow(0.995, double(age)))
			erase = true; // age-dependent mortality, my guess
	}
	return erase;
}

// no longer used as of 6.4.1
/*
 CALLIPRINOS* CALLIPRINOS::initialize_new_seedling(CELL* pContaining_cell, int n) {
	CALLIPRINOS* pCalliprinos = NULL;
	pCalliprinos = new CALLIPRINOS(0.05, 0.02, 0.0, 0, pContaining_cell, n);
	return pCalliprinos;
}
*/

// no longer used as of 6.4.1
/*
 CALLIPRINOS* CALLIPRINOS::initialize_new_sapling(CELL* pContaining_cell, TREE* pCALL) {
	CALLIPRINOS* pCalliprinos= NULL;
	pCalliprinos = new CALLIPRINOS(pCALL->get_height_m(), pCALL->radius_base_m, 0.0, pCALL->get_age(), pContaining_cell, 1);
	return pCalliprinos;
}
 */

bool CALLIPRINOS::burnTree(float herb_mass_kgm2) {
	bool erase = false;
	if (height_m < 0.3) { /** I have found no reports about fire effects on the survival of <i>Q. calliprinos</i> seedlings. 
		Therefore, I assume that <i>Q. calliprinos</i> is affected by fire more strongly by <i>Q. ithaburensis</i>.*/
		for (int n = 0; n < individuals; n++) {
			float z = z01();
			if (herb_mass_kgm2 < 0.175) {
				if (z < 0.8) // mortality by fire in sparse vegetation, as on cairns; increased from 0.6
					individuals--;
				else {
					height_m *= 0.7; // smaller effect on surviving plants, my estimate
					radius_base_m = 0.01;
				}
				
			} else {
				if (z < 0.95) // mortality by fire in dense vegetation
					individuals--;
				else {
					height_m *= 0.1; // stronger effect on surviving plants, my estimate
					radius_base_m = 0.01;
				}
			}
	}
		if (individuals < 1) erase=true;
	}
	else if (height_m < 1.5) { /** Fire effects on <i>Q. calliprinos</i> saplings similar but higher to <i>Q. ithaburenis</i>.*/
		float Z = z01();
		if (Z < 0.65+0.3/float(yearsAfterSevereBurn)) // estimate, between seedlings (80%) and adults (1 %); increased from 0.45
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
				yearsAfterSevereBurn=0; // Konstantinidis: it takes 5 yr to regain original forest structure
			}
		}
	} 
	else { /** Fire affects Q. ith. trees only in so far as all or only the lower leaves are burned (Kaplan 2005, Kaplan &amp; 
Gutman 1999). Fire damage is less in grazed plots because of less fuel.  Leaves resprout within one month. No info on Q call. See also 
			Konstantinidis et al. 2005 for Q. cocc.: good resprouter after fire */
		float z = z01();
		if (z > 0.7+0.29/float(yearsAfterSevereBurn)) // 0.7% of trees killed by fire (Kaplan 2005), slightly higher than Q.ith., my estimate
			erase = true;
		else {
			if (herb_mass_kgm2 < 0.175) // in sparse vegetation
			{
				if (z<0.4)
					leaf_mass_g *= 0.5;
				else if (z<0.42)
					leaf_mass_g = 0.0;
			}
			else // in dense vegetation
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

/** As a management to control shrub and tree cover, trees may be cut. We assume here that the trees are just cut down
 to a specified height but are not killed by the cutting. Info: Perevolotsky & Haimov 1992.
 */
bool CALLIPRINOS::cutTree(float cut_height_m){

	if(height_m > 1.2* cut_height_m) {
		height_m = cut_height_m;
		radius_top_m = 0.0;
		age = 5;
				
		update_shoot_mass();
	}	
	
	return true;
}

species_names CALLIPRINOS::getTreeType(void) {
	return Q_calliprinos;
}

