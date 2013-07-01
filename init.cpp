#include "init.h"

extern long N_cells, S_cells, W_cells, P_cells;
extern int N_edge, S_edge, W_edge, P_edge;
extern int slope;

void initialize (WATER* W, float* cc)
{  
  for(int is = 0; is<NS; is++)
  {
	// Abzählen der Habitatgrenzen
		  if (!N_edge && W[is].habitat == Plateau) P_edge = is; // Plateau
		  if (W[is].habitat == North_f)	    	   N_edge = is; // Nordhang
		  if (W[is].habitat == Wadi)			   W_edge = is; // Wadi
		  if (W[is].habitat == South_f)            S_edge = is; // Südhang
  }
  
  P_cells = (P_edge + NS - S_edge) * NZ;
  N_cells = (N_edge - P_edge) * NZ;
  W_cells = (W_edge - N_edge) * NZ;
  S_cells = (S_edge - W_edge) * NZ;
  
  
  float contrib_cells_on_slopes = 0.0;
  float evapo_factor = 1.0;
  
  // changed 20070718: distribute foothill water only when slope > 0
  float water_addition_to_wadi = 0.0;
  if (slope > 0)
{
  for(int iz = 0; iz<NZ; iz++)
  {
	contrib_cells_on_slopes += cc[iz*NZ+N_edge+1]; // foot of N slope
	contrib_cells_on_slopes += cc[iz*NZ+W_edge]; // foot of S slope
  }
  
  water_addition_to_wadi = contrib_cells_on_slopes/float(W_cells);
  evapo_factor = 0.8;
}

  for(int iz = 0; iz<NZ; iz++)
	for(int is = 0; is<NS; is++)
	{
	  W[iz*NZ+is].avail_rel = 1.5-W[iz*NZ+is].elevation * 0.01 + cc[iz*NZ+is] * 0.004; 
	  /*relative Wasserverfügbarkeit von 0,5 - 1,5 nach Höhenlage + contrib_cells; contrib_cells/270, da contrib_cells zwischen 1000 u. 4000  - absolute betrachtung wird so gewichtet, um die Wasserverfügbarkeit nicht zu sehr zu fördern	*/  
	  
	  switch (W[iz*NZ+is].habitat) {
		case South_f : W[iz*NZ+is].avail_rel *= evapo_factor; break; // Suedhang(Verdunstung)  
		case North_f : break;
		case Wadi    : W[iz*NZ+is].avail_rel += water_addition_to_wadi; break;
		case Plateau : break; 
	  } // end switch
	} // end for is, iz		

}
