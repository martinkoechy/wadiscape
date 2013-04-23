/*
 *  read_landscape.cpp
 *  Landschaft
 *
 *  Created by Forschung on Tue Jun 20 2006.
 *  Copyright (c) 2006 Martin Koechy. All rights reserved.
 *
 */

#include "read_landscape.h"
#include <sstream>

extern int NZ, NS;

void read_landscape(WATER* &W, int ll, int slope)
{
  std::ifstream landscape_file;
  std::stringstream LFN;
  LFN << "Wadiscape_slope_" << slope;
	
  switch (ll)
  {
	case 4: LFN<<"_EY.txt"; landscape_file.open(LFN.str().c_str()); break;
	case 3: LFN<<"_MA.txt"; landscape_file.open(LFN.str().c_str()); break;
	case 1: LFN<<"_LA.txt"; landscape_file.open(LFN.str().c_str()); break;
	case 0: LFN<<"_SB.txt"; landscape_file.open(LFN.str().c_str()); break;
	  default: std::cerr << "found no file for this slope" << std::endl;
  }  
	if(!landscape_file) std::cerr<< "file '" << LFN.str().c_str() << "' not found\n";
  int Z = 0; int S = 0; float slope_deg = 0.0; char h = 'a';
  
	landscape_file >> NS;
	landscape_file >> NZ;
	
	W = new WATER[NZ*NS];
	
  for (int iz = 0; iz < NZ; iz++)
	for (int is = 0; is < NS; is++)
	{
	  landscape_file >> Z;
	  landscape_file >> S;
	  landscape_file >> W[iz*NZ+is].elevation;
	  landscape_file >> h;
	  switch (h)
	  {
		case 'P': W[iz*NZ+is].habitat = Plateau; break;
		case 'N': W[iz*NZ+is].habitat = North_f; break;
		case 'W': W[iz*NZ+is].habitat = Wadi; break;
		case 'S': W[iz*NZ+is].habitat = South_f; break;
		default: std::cerr << "wrong habitat code input. It was '" << h << "'" << std::endl;
	  }
	  landscape_file >> W[iz*NZ+is].cc;
	  landscape_file >> W[iz*NZ+is].slope;
	}
  landscape_file.close();
}
