#include "init.h"

extern long N_cells, S_cells, W_cells, P_cells;
extern int NZ, NS;

void initialize (WATER* W)
{  
	long int contrib_cells_on_slopes = 0;
	for(int iz = 0; iz<NZ; iz++) {
		for(int is = 0; is<NS; is++)  {
			// AbzŠhlen der Habitatgrenzen
			if (W[iz*NZ+is].habitat == Plateau) P_cells++;
			else if (W[iz*NZ+is].habitat == North_f) {
				N_cells++; // Nordhang
				contrib_cells_on_slopes += W[iz*NZ+is].cc;
			}
			else if (W[iz*NZ+is].habitat == South_f) {
				S_cells++;
				contrib_cells_on_slopes += W[iz*NZ+is].cc;
			}
			else W_cells++;
		} // end is
  } // end iz
    
  
//  float water_addition_to_wadi = 0; // does not make sense for real landscapes within the scope of this program
  
  for(int iz = 0; iz<NZ; iz++)
	for(int is = 0; is<NS; is++)
	{
	  W[iz*NZ+is].avail_rel = 1.5-W[iz*NZ+is].elevation/100. + W[iz*NZ+is].cc/float(contrib_cells_on_slopes)*4000.0/250.; 
	  /*relative WasserverfŸgbarkeit von 0,5 - 1,5 nach Hšhenlage + contrib_cells; contrib_cells/270, 
	   da contrib_cells zwischen 1000 u. 4000  - absolute betrachtung wird so gewichtet, um die WasserverfŸgbarkeit 
	   nicht zu sehr zu fšrdern	*/  
	  
	  switch (W[iz*NZ+is].habitat) {
		case South_f : W[iz*NZ+is].avail_rel *= cos(W[iz*NZ+is].slope * q1_365 * pi2) * 0.8; break; // Projektion der geneigten FlŠche auf die Horizontale, Suedhang(Verdunstung)  
		case North_f : W[iz*NZ+is].avail_rel *= cos(W[iz*NZ+is].slope * q1_365 * pi2); break;
		case Wadi    : W[iz*NZ+is].avail_rel *= cos(W[iz*NZ+is].slope * q1_365 * pi2); break;
		case Plateau : W[iz*NZ+is].avail_rel *= cos(W[iz*NZ+is].slope * q1_365 * pi2); break; 
	  } // end switch
	} // end for is, iz		
}
