/*
 *  herbs.cpp
 *  Wadiscape5
 *
 *  Created by Forschung on 2008-06-06.
 *  Copyright 2008 Martin Köchy. All rights reserved.
 *
 */

#include "herbs.h"
#include "cell.h"

extern climate_names VEGETATION::our_climate;

HERB::HERB(CELL* p_containing_cell)
: VEGETATION(p_containing_cell)
{
	for(short m = 0; m < 11; m++) {
		feedM_kgcell[m] = 0.0;
		coverM_rel[m] = 0.0;
		heightM_m[m++] = 0.0;
		LAIM_rel[m++] = 0.0;
	}
	palatability = 1.0;
}

HERB* HERB::initialize(climate_names c, CELL* pContaining_cell) {
	float zuf1 = z01();
	seedbank_m2 = zuf1<0.8 ? 20000 : 0; // random number of seeds
	seeds_produced_m2 = 0;
	mass_kgm2 = 0.0;
	cover = 0.0;
	height_m = 0.0;
	
	HERB* h=NULL;
	return h;
}

unsigned long int HERB::sum_seeds_prod_grid = 0;

// the number of individuals per square meter in categories of 25 g/m2 mass, decentiles
float HERB::q10Inds [13] = {280.0, 724.0, 1236.0, 1732.0, 1968.0, 2188.0, 2396.0, 2580.0, 2784.0, 3036.0, 3392.0, 3616.0, 3985.2};
float HERB::q30Inds [13] = {456.4, 844.0, 1368.0, 1896.0, 2140.0, 2372.0, 2588.0, 2756.0, 2968.0, 3288.0, 3680.0, 3916.0, 4144.0};
float HERB::q50Inds [13] = {564,  1036,   1556,   2020,   2276,   2560,   2752,   2912,   3144,   3532,   3888,   4092,   4256};
float HERB::q70Inds [13] = {644.0,1176.0, 1752.0, 2176.0, 2464.0, 2764.0, 2968.0, 3120.0, 3376.0, 3768.0, 4084.0, 4254.8, 4360.4};
float HERB::q90Inds [13] = {732.0,1416.0, 2044.0, 2516.8, 2797.6, 3108.0, 3312.8, 3456.0, 3700.0, 4052.0, 4308.0, 4412.0, 4502.8};

/**
Assign number of individuals to mass categories
*/ 
unsigned long int HERB::calc_n_annual_individuals_m2(void)
{	
	float z = z01();
	float mass_gm2 = mass_kgm2*1000.0;
	float n = 0.0;
	
	if(z<0.2)
		n=q10Inds[int(min(mass_gm2/25,12.0f))]; // 25 is the size of the class
	else if(z<0.4)
		n=q30Inds[int(min(mass_gm2/25,12.0f))];
	else if(z<0.6)
		n=q50Inds[int(min(mass_gm2/25,12.0f))];
	else if(z<0.8)
		n=q70Inds[int(min(mass_gm2/25,12.0f))];
	else
		n=q90Inds[int(min(mass_gm2/25,12.0f))];
	
	return (unsigned long int)n;
}

unsigned long int HERB::get_seeds_produced_m2(void) {
	return seeds_produced_m2;
}

inline unsigned long int HERB::get_seedbank_m2(void) {
	return seedbank_m2;
}

void HERB::add_to_seedbank(unsigned long int number_cell) {
	seedbank_m2 += cell_to_m2(number_cell);
}

void HERB::reduce_seedbank (void) {
	if(seedbank_m2 > max_individuals_m2)
		seedbank_m2 -= max_individuals_m2;
	else
		seedbank_m2 = 0;
}


float HERB::calc_cover_rel(void)
{
	// Data from Pierre 20060704
	// values from Pierre were for peak biomass but here applied to monthly biomass
	float cover = 0.0;
	
	if (mass_kgm2 > 0.0) {
		if (mass_kgm2 < 0.1) {
			if (zc < 0.2)
				cover = 0.02;
			else if (zc < 0.4)
				cover = 0.04;
			else if (zc < 0.6)
				cover = 0.08;
			else if (zc < 0.8)
				cover = 0.12;
			else
				cover = 0.22;
		} else if (mass_kgm2 < 0.2) {
			if (zc < 0.2)
				cover = 0.15;
			else if (zc < 0.4)
				cover = 0.2;
			else if (zc < 0.6)
				cover = 0.25;
			else if (zc < 0.8)
				cover = 0.28;
			else
				cover = 0.4;
		} else if (mass_kgm2 < 0.3) {
			if (zc < 0.2)
				cover = 0.22;
			else if (zc < 0.4)
				cover = 0.335;
			else if (zc < 0.6)
				cover = 0.415;
			else if (zc < 0.8)
				cover = 0.53;
			else
				cover = 0.653;
		} else if (mass_kgm2 < 0.4) {
			if (zc < 0.2)
				cover = 0.22;
			else if (zc < 0.4)
				cover = 0.35;
			else if (zc < 0.6)
				cover = 0.4;
			else if (zc < 0.8)
				cover = 0.6;
			else
				cover = 0.78;
		} else if (mass_kgm2 >= 0.4) {
			if (zc < 0.2)
				cover = 0.552;
			else if (zc < 0.4)
				cover = 0.708;
			else if (zc < 0.6)
				cover = 0.75;
			else if (zc < 0.8)
				cover = 0.892;
			else
				cover = 0.944;
		}
	}
	return cover;
}

void HERB::mortality_seedbank(void) {
	seedbank_m2 = long (0.8 * double(seedbank_m2));
}

void HERB::grow_year(void) { // changed W6.5.0
	zq = z01(); // for growth
	zc = z01(); // for cover
	max_individuals_m2 = 0;
	for(month_names m=Aug; m<Dec; ++m) 
		feedM_kgcell[int(m)] *= 0.8; // represents decomposition of standing litter
	for(month_names m=Jan; m<Jul; ++m)
		feedM_kgcell[int(m)] = 0.0; // assume that any old litter has disappeared by January
}

void HERB::grow( float w_avail, float woody_cover) {
	float mass_gm2 = 0.0;
		
	if (seedbank_m2 && w_avail>50) // bei w_avail < 50 bleibt sb == 0
	{ //threshold seeds + moisture 20.3.04
		
		if (VEGETATION::our_climate==arid) // arid
						   // Results from annuals model, version 2.7.0, 2009-07-20, Martin Koechy
						   // community: arid, ca 100 mm mean annual precipitation
		{
			if (seedbank_m2 <1500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(191.6169/w_avail -3.32465);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(145.114/w_avail -3.350919);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(122.8771/w_avail -3.390793);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(108.2803/w_avail -3.439857);
				else
					mass_gm2 = 1.0/exp(91.54892/w_avail -3.509932);
			}
			else if (seedbank_m2 <3500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(266.8603/w_avail -4.13893);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(196.3706/w_avail -4.123343);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(171.5111/w_avail -4.148971);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(149.4997/w_avail -4.178211);
				else
					mass_gm2 = 1.0/exp(130.8207/w_avail -4.24073);
			}
			else if (seedbank_m2 <7500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(394.4767/w_avail -5.090505);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(327.9539/w_avail -5.092926);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(286.0197/w_avail -5.099705);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(258.264/w_avail -5.132206);
				else
					mass_gm2 = 1.0/exp(217.3858/w_avail -5.159731);
			}
			else if (seedbank_m2 <12500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(371.627/w_avail -5.489586);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(306.1046/w_avail -5.477345);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(268.0926/w_avail -5.480291);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(238.6632/w_avail -5.493238);
				else
					mass_gm2 = 1.0/exp(205.7989/w_avail -5.521512);
			}
			else if (seedbank_m2 <17500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(254.8498/w_avail -5.585908);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(221.9957/w_avail -5.602534);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(199.6928/w_avail -5.615413);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(185.0877/w_avail -5.637514);
				else
					mass_gm2 = 1.0/exp(165.6736/w_avail -5.665024);
			}
			else if (seedbank_m2 <25000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(212.1577/w_avail -5.645208);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(183.0971/w_avail -5.659304);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(167.0625/w_avail -5.67341);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(154.478/w_avail -5.693735);
				else
					mass_gm2 = 1.0/exp(141.8611/w_avail -5.729235);
			}
			else if (seedbank_m2 <35000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(166.1127/w_avail -5.678247);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(141.7236/w_avail -5.692837);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(130.0858/w_avail -5.709081);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(123.2888/w_avail -5.732781);
				else
					mass_gm2 = 1.0/exp(113.6380/w_avail -5.76102);
			}
			else if (seedbank_m2 <45000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(132.9206/w_avail -5.67738);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(116.2485/w_avail -5.695314);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(107.0925/w_avail -5.714178);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(100.2424/w_avail -5.735411);
				else
					mass_gm2 = 1.0/exp(90.2465/w_avail -5.760072);
			}
			else if (seedbank_m2 <55000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(119.8701/w_avail -5.696786);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(100.0709/w_avail -5.704614);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(93.02126/w_avail -5.722065);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(85.39995/w_avail -5.741486);
				else
					mass_gm2 = 1.0/exp(75.61927/w_avail -5.760713);
			}
			else {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(109.7571/w_avail -5.707309);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(90.25482/w_avail -5.715785);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(82.65886/w_avail -5.731903);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(76.00115/w_avail -5.748595);
				else
					mass_gm2 = 1.0/exp(69.15582/w_avail -5.77349);
			}
		}// if w_availchoice1
		
		if (VEGETATION::our_climate==semiarid) //semi-ari
		{ 						   
		  // Results from annuals model, version 2.7.0, 2009-07-20, Martin Koechy
		  // community: semi-arid, ca 300 mm mean annual precipitation
			if (seedbank_m2 <1500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(254.3175/w_avail -3.505299);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(188.4727/w_avail -3.509713);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(159.9442/w_avail -3.534740);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(137.7319/w_avail -3.570266);
				else
					mass_gm2 = 1.0/exp(119.1123/w_avail -3.638301);
			}
			else if (seedbank_m2 <3500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(301.5907/w_avail -4.274214);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(235.9753/w_avail -4.288511);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(207.1281/w_avail -4.316637);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(185.0714/w_avail -4.347091);
				else
					mass_gm2 = 1.0/exp(163.6526/w_avail -4.422833);
			}
			else if (seedbank_m2 <7500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(406.997/w_avail -5.208006);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(339.7474/w_avail -5.220227);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(304.6154/w_avail -5.23831);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(271.1144/w_avail -5.251715);
				else
					mass_gm2 = 1.0/exp(243.1708/w_avail -5.308157);
			}
			else if (seedbank_m2 <12500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(385.5569/w_avail -5.55495);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(322.5906/w_avail -5.544871);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(291.8318/w_avail -5.564147);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(268.3433/w_avail -5.585463);
				else
					mass_gm2 = 1.0/exp(242.6934/w_avail -5.624228);
			}
			else if (seedbank_m2 <17500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(288.0517/w_avail -5.588502);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(247.9747/w_avail -5.601121);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(227.2674/w_avail -5.619762);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(209.3245/w_avail -5.638251);
				else
					mass_gm2 = 1.0/exp(188.6301/w_avail -5.668185);
			}
			else if (seedbank_m2 <25000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(254.1499/w_avail -5.628591);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(212.8284/w_avail -5.632862);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(196.4382/w_avail -5.652784);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(179.0741/w_avail -5.667016);
				else
					mass_gm2 = 1.0/exp(160.9990/w_avail -5.693923);
			}
			else if (seedbank_m2 <35000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(211.034/w_avail -5.656934);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(176.2576/w_avail -5.662366);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(159.9322/w_avail -5.673339);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(145.0364/w_avail -5.685297);
				else
					mass_gm2 = 1.0/exp(129.7751/w_avail -5.706582);
			}
			else if (seedbank_m2 <45000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(185.0453/w_avail -5.667275);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(153.1912/w_avail -5.669952);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(137.7223/w_avail -5.683523);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(125.8623/w_avail -5.696776);
				else
					mass_gm2 = 1.0/exp(114.7678/w_avail -5.723191);
			}
			else if (seedbank_m2 <55000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(168.0678/w_avail -5.681825);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(138.7515/w_avail -5.690318);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(123.7767/w_avail -5.697749);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(112.0456/w_avail -5.708407);
				else
					mass_gm2 = 1.0/exp(98.87124/w_avail -5.726635);
			}
			else {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(158.0588/w_avail -5.704395);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(129.3616/w_avail -5.705875);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(116.2054/w_avail -5.712704);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(104.8395/w_avail -5.72142);
				else
					mass_gm2 = 1.0/exp(91.44522/w_avail -5.738872);
			}
		}// if VEGETATION::our_climate semi-arid
		
		if (VEGETATION::our_climate==typmed) {
			// Results from annuals model, version 2.7.0, 2009-07-20, Martin Koechy
			// community: Mediterranean, ca 600 mm mean annual precipitation
			if (seedbank_m2 <1500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(287.3743/w_avail -3.6285);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(211.1925/w_avail -3.646547);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(174.7639/w_avail -3.672549);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(153.2476/w_avail -3.713557);
				else
					mass_gm2 = 1.0/exp(130.3831/w_avail -3.787695);
			}
			else if (seedbank_m2 <3500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(353.4076/w_avail -4.516534);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(276.0234/w_avail -4.523076);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(236.5286/w_avail -4.539668);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(209.5085/w_avail -4.575529);
				else
					mass_gm2 = 1.0/exp(170.6114/w_avail -4.607138);
			}
			else if (seedbank_m2 <7500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(427.7605/w_avail -5.497097);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(349.3374/w_avail -5.483953);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(310.1391/w_avail -5.490225);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(277.4844/w_avail -5.507514);
				else
					mass_gm2 = 1.0/exp(244.8126/w_avail -5.543985);
			}
			else if (seedbank_m2 <12500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(404.097/w_avail -5.81247);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(335.8254/w_avail -5.796016);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(296.6346/w_avail -5.786268);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(267.7041/w_avail -5.794337);
				else
					mass_gm2 = 1.0/exp(233.4390/w_avail -5.80405);
			}
			else if (seedbank_m2 <17500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(295.8686/w_avail -5.757762);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(250.3475/w_avail -5.75394);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(226.7753/w_avail -5.762662);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(210.0527/w_avail -5.776754);
				else
					mass_gm2 = 1.0/exp(188.22/w_avail -5.797821);
			}
			else if (seedbank_m2 <25000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(248.8868/w_avail -5.736251);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(211.2017/w_avail -5.742966);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(194.0939/w_avail -5.754408);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(178.9341/w_avail -5.771454);
				else
					mass_gm2 = 1.0/exp(162.0677/w_avail -5.79357);
			}
			else if (seedbank_m2 <35000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(227.6511/w_avail -5.765345);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(189.008/w_avail -5.761782);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(169.3182/w_avail -5.767704);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(156.2528/w_avail -5.782889);
				else
					mass_gm2 = 1.0/exp(136.6929/w_avail -5.79276);
			}
			else if (seedbank_m2 <45000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(201.4805/w_avail -5.754397);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(167.0021/w_avail -5.754688);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(150.8516/w_avail -5.762062);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(138.1983/w_avail -5.775213);
				else
					mass_gm2 = 1.0/exp(121.1598/w_avail -5.788218);
			}
			else if (seedbank_m2 <55000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(193.8747/w_avail -5.770172);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(159.6642/w_avail -5.766214);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(145.1200/w_avail -5.775041);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(129.6643/w_avail -5.780804);
				else
					mass_gm2 = 1.0/exp(111.73/w_avail -5.792896);
			}
			else {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(185.6229/w_avail -5.789161);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(152.8758/w_avail -5.783021);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(134.9842/w_avail -5.7845);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(121.8278/w_avail -5.791428);
				else
					mass_gm2 = 1.0/exp(105.2170/w_avail -5.804336);
			}
		}// VEGETATION::our_climate 4
		
		if (VEGETATION::our_climate==mesicmed) {
			// Results from annuals model, version 2.7.0, 2009-07-20, Martin Koechy
			// community: mesic Mediterranean, ca 800 mm mean annual precipitation
			if (seedbank_m2 <1500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(262.2237/w_avail -3.592068);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(195.1669/w_avail -3.602196);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(164.7443/w_avail -3.630417);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(141.0412/w_avail -3.661949);
				else
					mass_gm2 = 1.0/exp(116.7655/w_avail -3.724169);
			}
			else if (seedbank_m2 <3500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(313.6762/w_avail -4.445324);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(251.3314/w_avail -4.464206);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(219.2565/w_avail -4.485153);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(194.4309/w_avail -4.514733);
				else
					mass_gm2 = 1.0/exp(171.4796/w_avail -4.578741);
			}
			else if (seedbank_m2 <7500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(399.546/w_avail -5.40103);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(334.4041/w_avail -5.400775);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(300.4299/w_avail -5.418063);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(273.4844/w_avail -5.437713);
				else
					mass_gm2 = 1.0/exp(233.2192/w_avail -5.4574);
			}
			else if (seedbank_m2 <12500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(362.0703/w_avail -5.663349);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(302.0471/w_avail -5.658243);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(271.8785/w_avail -5.672056);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(243.4097/w_avail -5.681186);
				else
					mass_gm2 = 1.0/exp(217.8239/w_avail -5.709675);
			}
			else if (seedbank_m2 <17500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(274.1115/w_avail -5.670672);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(233.4737/w_avail -5.676108);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(211.0295/w_avail -5.687677);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(195.1570/w_avail -5.70648);
				else
					mass_gm2 = 1.0/exp(176.0129/w_avail -5.733144);
			}
			else if (seedbank_m2 <25000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(238.4813/w_avail -5.680303);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(200.7975/w_avail -5.685371);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(183.5649/w_avail -5.699961);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(172.0338/w_avail -5.721494);
				else
					mass_gm2 = 1.0/exp(156.9378/w_avail -5.746961);
			}
			else if (seedbank_m2 <35000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(213.606/w_avail -5.708975);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(175.7074/w_avail -5.704138);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(157.1200/w_avail -5.710908);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(142.765/w_avail -5.722404);
				else
					mass_gm2 = 1.0/exp(128.3644/w_avail -5.740107);
			}
			else if (seedbank_m2 <45000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(194.1642/w_avail -5.71872);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(159.8664/w_avail -5.717553);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(141.7265/w_avail -5.72287);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(129.2531/w_avail -5.73377);
				else
					mass_gm2 = 1.0/exp(115.0113/w_avail -5.750225);
			}
			else if (seedbank_m2 <55000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(185.1094/w_avail -5.739063);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(152.1031/w_avail -5.7361);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(136.5170/w_avail -5.742472);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(121.3871/w_avail -5.745001);
				else
					mass_gm2 = 1.0/exp(105.7015/w_avail -5.754986);
			}
			else {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(181.2697/w_avail -5.753162);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(143.7905/w_avail -5.74539);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(127.8760/w_avail -5.749471);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(115.6257/w_avail -5.757145);
				else
					mass_gm2 = 1.0/exp(96.998/w_avail -5.765328);
			}
		}// VEGETATION::our_climate 5
		
		if (VEGETATION::our_climate==drymed) {
			// Results from annuals model, version 2.7.0, 2009-07-20, Martin Koechy
			// community: dry Mediterranean, ca 450 mm mean annual precipitation
			if (seedbank_m2 <1500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(239.9151/w_avail -3.553738);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(176.5858/w_avail -3.577187);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(154.3408/w_avail -3.617797);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(132.4774/w_avail -3.653649);
				else
					mass_gm2 = 1.0/exp(107.4594/w_avail -3.707376);
			}
			else if (seedbank_m2 <3500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(296.9045/w_avail -4.404352);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(231.0145/w_avail -4.409252);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(203.4884/w_avail -4.441089);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(179.8697/w_avail -4.475304);
				else
					mass_gm2 = 1.0/exp(154.3021/w_avail -4.53156);
			}
			else if (seedbank_m2 <7500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(380.5183/w_avail -5.360109);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(320.8400/w_avail -5.372622);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(290.7899/w_avail -5.401756);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(259.0481/w_avail -5.412456);
				else
					mass_gm2 = 1.0/exp(228.4180/w_avail -5.442402);
			}
			else if (seedbank_m2 <12500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(346.8861/w_avail -5.655754);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(291.1150/w_avail -5.65382);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(264.1856/w_avail -5.671762);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(239.3255/w_avail -5.690282);
				else
					mass_gm2 = 1.0/exp(211.0191/w_avail -5.712445);
			}
			else if (seedbank_m2 <17500) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(270.9604/w_avail -5.683611);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(226.9629/w_avail -5.680584);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(207.6802/w_avail -5.69729);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(189.639/w_avail -5.71078);
				else
					mass_gm2 = 1.0/exp(168.0967/w_avail -5.733581);
			}
			else if (seedbank_m2 <25000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(224.2813/w_avail -5.678544);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(191.0281/w_avail -5.688382);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(176.6565/w_avail -5.707631);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(164.6318/w_avail -5.725349);
				else
					mass_gm2 = 1.0/exp(147.5138/w_avail -5.747127);
			}
			else if (seedbank_m2 <35000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(190.2003/w_avail -5.686336);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(160.3877/w_avail -5.693401);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(143.7769/w_avail -5.701352);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(131.2410/w_avail -5.716495);
				else
					mass_gm2 = 1.0/exp(115.8982/w_avail -5.738132);
			}
			else if (seedbank_m2 <45000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(179.1732/w_avail -5.713855);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(150.2019/w_avail -5.719243);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(134.6273/w_avail -5.72574);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(121.5141/w_avail -5.738422);
				else
					mass_gm2 = 1.0/exp(107.2780/w_avail -5.754033);
			}
			else if (seedbank_m2 <55000) {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(170.6440/w_avail -5.735319);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(140.7995/w_avail -5.735717);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(124.7488/w_avail -5.741932);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(113.1602/w_avail -5.750205);
				else
					mass_gm2 = 1.0/exp(96.5101/w_avail -5.763152);
			}
			else {
				if (zq < 0.2)
					mass_gm2 = 1.0/exp(159.4627/w_avail -5.74553);
				else if (zq < 0.4)
					mass_gm2 = 1.0/exp(130.7643/w_avail -5.742755);
				else if (zq < 0.6)
					mass_gm2 = 1.0/exp(117.5413/w_avail -5.751987);
				else if (zq < 0.8)
					mass_gm2 = 1.0/exp(105.3285/w_avail -5.757863);
				else
					mass_gm2 = 1.0/exp(91.81214/w_avail -5.771959);
			}
		} // VEGETATION::our_climate 3
		
	} // end if (n_ann_seeds>0 & w_avail > 50)
	
	// adjustment to using infiltrated water instead of the topographic moisture in the annuals model in Wadiscape 3
	mass_gm2 *= 1.174182; // factor derived by comparing the max value in unadjusted Wadiscape simulations with max from equation
	
	float sb = 1.0; // sb drueckt einen relativen Straucheffekt auf die Annuellen aus
	
	
	//seed effect evtl. weg, da indirekt schon ueber water effect - testen!!
	//  a_seed=a_seed*seedfact; //shrub effect seeds
	//  shrub_effect=rainfact/100.0;  // shrub effect moisture
	//preliminary: shrub cover effect on annuals; 2006-06-06: year_rain in w_avail abgeaendert
	if (w_avail > 50) {
		if (w_avail < 350)
			sb = 1.0 + 0.1 * woody_cover; // shrub effect on annual biomass
		else
			sb = 1.0 - 0.5 * woody_cover; // Holzapfel et al. 2006 25.3.04
	}
	
	mass_kgm2 = sb * g_to_kg(mass_gm2); // sb: shrub effect on biomass
	if (mass_kgm2 > max_ann_mass_kgm2) {
		std::cerr << "calculated mass, " << mass_kgm2 << " kg/m2 was > max herbaceous mass in HERB::grow\n\t with seedbank = " << seedbank_m2 << ", zq = " << zq << ", wavail = " << w_avail << std::endl;
		mass_kgm2 = max_ann_mass_kgm2; // max_ann_mass_kg 0.6 kg
	}
	if (mass_kgm2 < 0.0)
		mass_kgm2 = 0.0;
	
	//prepare for seed bank reduction by germination: 
	float tempMax = min(max_individuals_m2, calc_n_annual_individuals_m2()); max_individuals_m2 = tempMax;
}


bool HERB::grow_month(WATER* W, month_names m) {
	float woody_cover = the_pCell->get_woody_coverM_rel(m-1);
	// distribute production across months according to water availability
	float cumRain = W->monthly_cummWater[m];
	cumRain = cumRain>20? cumRain: 0; // at least 15 mm of rain are necessary to start germination - Loria & Noy-Meir 1979

	if (cumRain > 20.0 || mass_kgm2 > 0.0) {
		grow(cumRain, woody_cover);
		cumulated_forage_loss_kgcell = 0.0; // changed W6.5.0
	}
	
	feedM_kgcell[int(m)] = max(mass_kgm2 * cell_area_m2 - cumulated_forage_loss_kgcell, 0.0f)*(m>Mar?pow(0.8,m-int(Apr)):1.0); // last month's cumulated_forage_loss 
	heightM_m[int(m)] = calc_height_m();
	coverM_rel[int(m)] = calc_cover_rel();
	LAIM_rel[int(m)] = calc_LAI_rel(W, m);
	
	return false;
}

/** height (cm) = 2.8591475 + 0.083821407 dry mass [g/m²] (<i>R</i>² = 0.81, <i>n</i> = 256; Liancourt et al., 2006, unpubl. data)
*/
float HERB::calc_height_m(void) {
	return mass_kgm2? 0.028591475 + 0.83821407E-6 * mass_kgm2: 0.0; // R² = 0.81, n = 256; Liancourt et al., unpubl. data
}

/** leaf + stem area [cm²] = 0.146948*mass [mg] (Andreas Mäck, 2008, unpubl. data)
 Mass and hence leaf area represent the covered part of a cell.
*/
float HERB::calc_LAI_rel(WATER* W, month_names m) {
	return 
		W->is_past_rainy_month(m)? 
			0.0: // after the rainy season all plants die, no LAI
			14.6948 * mass_kgm2; // leaf area m²/m²
}

unsigned long int HERB::prod_seeds_cell(void) {
	seeds_produced_m2 = 0;
	float mass_g_m2 = kg_to_g(mass_kgm2);
	float z=z01();
	
	if (mass_g_m2>0.0) {
		// regressions from Martin K, RxS simulations, new v 6.0, 2009-07-20.
		if(z<0.2)
			seeds_produced_m2 = (long unsigned int) (-1917.43 + 66.34 * mass_g_m2);
		else if(z<0.4)
			seeds_produced_m2 = (long unsigned int) (-1496.94 + 70.68 * mass_g_m2);
		else if(z<0.6)
			seeds_produced_m2 = (long unsigned int) (-1230.1  + 74.3  * mass_g_m2);
		else if(z<0.8)
			seeds_produced_m2 =  (long unsigned int) (-939.87 + 77.87 * mass_g_m2);
		else
			seeds_produced_m2 = (long unsigned int) (-484.68 + 82.71 * mass_g_m2);
		
		if (seeds_produced_m2 < 0)
			seeds_produced_m2 = 0;
		
		seedbank_m2 += long(seeds_produced_m2);
	}
	
	return (unsigned long int) (seeds_produced_m2 * cell_area_m2);
}