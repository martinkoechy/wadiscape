/*
 *  read_landscape.cpp
 *  Landschaft
 *
 *  Created by Forschung on Tue Jun 20 2006.
 *  Copyright (c) 2006 Martin Köchy. All rights reserved.
 *
 */
#include <sstream>
#include <fstream>

#include "read_landscape.h"


void read_landscape(WATER* W, int ll, float* cc, int slope, uint32* elevation_array)
{
	std::stringstream LFN;
	LFN << "../topographies/Wadiscape_slope_" << slope << "_" << ll << ".txt" << std::ends;

	std::ifstream landscape_file;
	landscape_file.open(LFN.str().c_str());

	int Z = 0; int S = 0; float slope_deg = 0.0; char h = 'a';
	
	for (int iz = 0; iz < NZ; iz++)
		for (int is = 0; is < NS; is++)
		{	uint32 c = iz*NZ+is;
			
			landscape_file >> Z;
			landscape_file >> S;
			landscape_file >> W[c].elevation; elevation_array[c] = uint32 (W[c].elevation);

			landscape_file >> h;
			switch (h)
			{
				case 'P': W[c].habitat = Plateau; break;
				case 'N': W[c].habitat = North_f; break;
				case 'W': W[c].habitat = Wadi; break;
				case 'S': W[c].habitat = South_f; break;
				default: std::cerr << "wrong habitat code input from " << LFN.str() << std::endl;
			}
			landscape_file >> cc[c]; // cumulative runoff
			landscape_file >> slope_deg;
			
		}// for

	landscape_file.close();

	// simple calculation of cell slope and aspect (de Smith et al. 2007)
	// aspect calculation converted to radians
	for (ushort iz = 0; iz < NZ; iz++)
		for (ushort is = 0; is < NS; is++) {
			// dzdy is half the difference of elevation of the northern and southern neighbour
			// beyond grid use the same elevation as target cell
			float dzdy = (W[iz*NZ+(is<NS-1?is+1:is)].elevation - W[iz*NZ+(is>0?is-1:is)].elevation);
			// dzdx is half the difference of elevation of the western and eastern neighbour
			float dzdx = (W[(iz<NZ-1?iz+1:iz)*NZ+is].elevation - W[(iz>0?iz-1:iz)*NZ+is].elevation);
			
			W[iz*NZ+is].cell_slope_rad = atan(sqrtf(dzdx/cell_length_m * dzdx/cell_length_m + dzdy/cell_length_m * dzdy/cell_length_m)); // Smith et al. 2007
			W[iz*NZ+is].aspect_rad = atan2(dzdx, dzdy); // Smith et al. 2007, see Wikipedia for explanation of atan2

			short nb_id = 0;
			WATER* thisCell = &W[iz*NZ+is];
			WATER* pNb; 
			for (short d_iz = -1; d_iz < 2; d_iz++) {
				for (short d_is = -1; d_is < 2; d_is++) {
					if (d_iz == 0 && d_is == 0)
						continue;
					pNb = thisCell + d_iz * NZ + d_is; // pointer arithmetic
					
					W[iz*NZ+is].angle_nb[nb_id] = 0.0; ///< angle to neighbour cell
					
					// beyond grid
					if (iz + d_iz < 0 || iz + d_iz > NZ - 1 || is + d_is < 0 || is
						+ d_is > NS - 1) {
						W[iz*NZ+is].angle_nb[nb_id] = 0.0; // assume neighbour cell has the same elevation
					} else { // within grid
						W[iz*NZ+is].angle_nb[nb_id] = (pNb->elevation - W[iz*NZ+is].elevation) / sqrtf(d_iz * d_iz
																		+ d_is * d_is) * 1000.0; //downwards is negative
					}
					nb_id++; // keep track of position
				}
			}
		}
}
