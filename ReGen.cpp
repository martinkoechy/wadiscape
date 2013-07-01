/*
 *  ReGen.cpp
 *  ReGen 1.0.1
 *
 *  Created by Martin Koechy on Thu Feb 06 2003.
 *
 */
#include "ReGen.h"
#include <valarray>
#include <vector>
#include <math.h>

//extern const unsigned short days_in_year;

RAINPARAMETERS::RAINPARAMETERS (int MAP, float dDMR)
{
  calcParameters (MAP, dDMR);
}

RAINPARAMETERS::RAINPARAMETERS (float A, float L, float W, float a, float l, float w, float dDMR)
{
  lA=A; lL=L; lW=W; vA=a; vL=l; vW=w; d=dDMR;
}

/** The parameters have been derived from 7 meteorological stations in interior Israel. See Köchy 2007 for details. */
void RAINPARAMETERS::calcParameters (float MAP, float d)
{

  // Parameter values based on monthly units

  // rain occurance:
  lA = 0.130034 + 0.000408 * MAP;
  lA = lA*(1-d +1.33*d*d-(0.61+1.57*lA)*d*d*d);
  lL = 5.735; // 5.235 + 0.5 months
  lW = 1.704746 + 0.000242 * MAP;

  // rain volume:
  vA = -13.7178 + 4.0508301 * log(MAP);
  vA = vA * (1+d);
  vL = 5.586; // 5.086 + 0.5 months
  vW = 11.336;
}


//------------------------------------------------------------------
ReGen::ReGen (RAINPARAMETERS* p_RainP)
{
  pP = p_RainP;
  theAnnualRain = 0.0;

  float days[days_in_year];
  for (int d = 0; d < days_in_year; days[d] = d++);
  std::valarray <float> localDays (days, days_in_year);
  theDays.resize(days_in_year);
  theDays = localDays;

  rainProb.resize(days_in_year);

  meanRainVolume.resize(days_in_year);
  meanRainVolume = calcGauss(pP->vA, pP->vL, pP->vW, 4);

  rainyDay.resize(days_in_year);

  rainVolume.resize(days_in_year);
}


void ReGen::drawYear (void)
{
	calcRain();
	theAnnualRain = rainVolume.sum(); // calculate right away, reuse later
}

void ReGen::drawYear (unsigned short y)
{
	drawYear ();
}

void ReGen::drawYear(unsigned short yearID, int windowSize) {
	drawYear ();
}
	

//------------------------------------------------------------------
std::valarray <float> ReGen::calcGauss (float A, float L, float W, int s)
{
  // A: amplitude, L: location, W: width, s: shape
	std::valarray <float> G (days_in_year);
  G = theDays;

  G *= (12.0F*q1_365);
  G += (6.0F - L);
  for (int i = 0; i < days_in_year; i++) //  .apply does not work
	G[i] = fmod(G[i],12.0F);
  G += (L - 6.0F);

  G = A * exp(-pow(G - L, float(s))/float(2.0*W*W));

  return G;
}

//------------------------------------------------------------------
void ReGen::calcRain (void)
{
  // calculate rainy days
  rainProb = calcGauss(pP->lA, pP->lL, pP->lW);

  rainyDay=0.0;
  for (int i = 0; i < days_in_year; i++)
	if (Zzo.rand_halfclosed01() < rainProb[i] && rainProb[i] > 0.05)
	  rainyDay[i] = 1.0;


  // calculate rain volume on a specific day
  for (int i = 0; i < days_in_year; i++)
	rainVolume[i] = Zzo.rand_halfclosed01();
  rainVolume *= -1.0;
  rainVolume += 1.0;
  rainVolume = -log(rainVolume);

  rainVolume *= meanRainVolume;
  rainVolume *= rainyDay;
}

// the following funtions should be inherited from RAINSERIES
/*
 float ReGen::getRain (int day) const
{
	return rainVolume[day];
}

float ReGen::getAnnualRain (void) const
{
	return theAnnualRain;
}


float ReGen::getPeriodRain (int start, int end) const
{
	float sum = 0.0;
	for (int day = start; day <= end; day++)
		sum+=rainVolume[day];
	return sum;
}
*/

/**
Returns the accumulated rain for each month of the year.
The days per month deviate from that of calendar months in that they are more
equally distributed.
*/
/*
float ReGen::getPeriodRain (month_names month) const
{ //from:  0  31  61  92 122 152 183 213 244 274 305 335
  //  to: 30  60  91 121 151 182 212 243 273 304 334 364
  float mrain = 0.0;
	switch (month) {
		case Aug:
			mrain = getPeriodRain(0, 30); break; // 31
		case Sep:
			mrain = getPeriodRain(31, 60); break; // 30
		case Oct:
			mrain = getPeriodRain(61, 91); break; // 31
		case Nov:
			mrain = getPeriodRain(92, 121); break; // 30
		case Dec:
			mrain = getPeriodRain(122, 151); break; // 30
		case Jan:
			mrain = getPeriodRain(152, 182); break; // 31
		case Feb:
			mrain = getPeriodRain(183, 212); break; // 30
		case Mar:
			mrain = getPeriodRain(213, 243); break; // 31
		case Apr:
			mrain = getPeriodRain(244, 273); break; // 30
		case May:
			mrain = getPeriodRain(274, 304); break; // 31
		case Jun:
			mrain = getPeriodRain(305, 334); break; // 30
		case Jul:
			mrain = getPeriodRain(335, 364); break; // 30
		default:
			std::cerr << "wrong month in year.cpp, line 24ff" << std::endl;
	}
	return mrain;
}
*/
// end inherited
