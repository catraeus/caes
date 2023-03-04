
// $Id: CaesRemez.cpp 234 2023-01-23 22:27:00Z duncang $

//============================================================================
// Name        : Remez.cpp
// Author      : Catraeus & Duncan Gray
// Version     :
// Copyright   : Copyright © 2014 - 2019 Catraeus & Duncan Gray
// Description :
/*
   The famous Remez Exchange Algorithm for
     equi-ripple zero group delay destortion filters.
*/
//============================================================================

#include <math.h>
#include <stdio.h>

#include <caes/CaesTypes.hpp>

#include <caes/CaesMath.hpp>

#include <caes/CaesRemez.hpp>


       Remez::Remez          ( void        ) {
  //
  iN         = 0;
  taps       = NULL;
  iNumB      = 0;
  iE         = new ldouble[EC_MAX_B * 2];
  iG         = new ldouble[EC_MAX_B    ];
  iW         = new ldouble[EC_MAX_B    ];
  iType      = FT_BAND;

  symm       = SY_POS;

  numGdd     = 0;
  gddXerrDex = NULL;
  gddXlocDex = NULL;
  gddXerrVal = NULL;
  gddF       = NULL;
  gddG       = NULL;
  gddW       = NULL;

  numCos     =    1;
  aryCos     = NULL;
  x          = NULL;
  y          = NULL;
  ad         = NULL;

  convergeTest = 1.0e-12;

}
       Remez::~Remez         ( void        ) {
}

void   Remez::SetEdge        ( llong   i_b, ldouble i_f ) {
  if(i_b > EC_MAX_B * 2)    i_b = EC_MAX_B * 2;
  if(i_b <  0          )    i_b = 0   ;
  if(i_f >  0.5        )    i_f = 0.5 ;
  if(i_f <  0.0        )    i_f = 0.0 ;
  iE[i_b] = i_f;
}

void   Remez::Calculate      ( void        ) {
  llong     i;
  llong     iter;
  ldouble    correctionFactor;
  ldouble    iOverLen;


  //fprintf(stderr, "Ca\n"); fflush(stderr);
  Setup();
  CreateDenseGrid();
  InitialGuess();



// Here is the meat of it, Thank you Mr. Remez
  for(iter=0; iter<EC_MAX_ITER; iter++) {
    fprintf(stderr, "\nIter %5lld", iter);fflush(stderr);
    CalcParms();
    CalcError();
    Search();
    if(IsDone())
      break;
  }
  if(iter >= EC_MAX_ITER){ fprintf(stderr, "Reached maximum iteration count.\nResults may be bad.\n");fflush(stderr);}
  CalcParms();

//  Find the 'aryCos' of the filter for use with Frequency Sampling.
//  If odd or Negative symm, fix the aryCos according to Parks & McClellan
  for(i=0; i<=iN/2; i++) {
    iOverLen = (ldouble)i / iN;
    if(symm == Remez::SY_POS) {
      if(iN % 2)  correctionFactor = 1;
      else            correctionFactor = cos(PI  * iOverLen);
      }
    else {
      if(iN % 2)  correctionFactor = sin(PIx2 * iOverLen);
      else            correctionFactor = sin(PI  * iOverLen);
      }
    aryCos[i] = ComputeA(iOverLen) * correctionFactor;
    }
   FtoT();
   Teardown();
   return;
}

void   Remez::NormDC         ( void        ) {
  llong  i;
  ldouble dc;

  dc = 0.0;
  for(i=0; i<iN; i++)
    dc += taps[i];
  dc /= (ldouble)iN;
  for(i=0; i<iN; i++)
    taps[i] /= dc;
  return;
}
void   Remez::NormRMS        ( void        ) {
  llong  i;
  ldouble rms;

  rms = 0.0;
  for(i=0; i<iN; i++)
    rms += taps[i] * taps[i];
  rms /= iN;
  rms = sqrt(rms);
  for(i=0; i<iN; i++)
    taps[i] /= rms;
  return;
}
void   Remez::NormSF         ( ldouble i_sf   ) {
  llong    i;
  ldouble  convo1;
  ldouble  dPh;
  ldouble  ph;
  ldouble  phOff;
  ldouble  t;
  ldouble  c;


  if(i_sf >= 0.5) i_sf = 0.0;
  if(i_sf <  0.0) i_sf = 0.0;
  // IMPORTANT This whole normalization thing is only valid because of a bunch of symmetry assumptions.
  phOff = ((iN ^ 2) == 1) ? 0.0 : - PI;
  dPh = i_sf * PIx2;

  convo1 = 0.0;
  ph  = phOff;
  for(i = 0; i < iN; i++) {  // test frequency is 1.0 = peak
    ph  += dPh;
    if(phOff > PIx2) phOff -= PIx2; // IMPORTANT, assumes the i_sf is Nyquist-compliant
    t    = taps[i];
    c    = cos(ph);
    convo1 += t*c;
  }
  convo1 = 1.0 / convo1;
  for(i=0; i<iN; i++)
    taps[i] *= convo1;
  return;
}
void   Remez::NormPeak       ( void        ) {
  llong  i;
  ldouble peak;

  peak = 0.0;
  for(i=0; i<iN; i++)
    if(dabs(peak)<taps[i])
      peak = dabs(taps[i]);
  for(i=0; i<iN; i++)
    taps[i] /= peak;
  return;
}
void   Remez::SetAry         ( ldouble *i_a ) {
  taps              = i_a;
  return;
}

void   Remez::Setup          ( void        ) {
  llong i;

  if(iType == FT_BAND)  symm = SY_POS;
  else                  symm = SY_NEG;

  numCos = iN / 2;                  // number of extrema
  if((iN % 2) && (symm == SY_POS)) numCos++;

//  Predict dense grid size in advance for memory allocation  .5 is so we round up, not truncate
  numGdd = 0;
  for(i=0; i<iNumB; i++) {
    numGdd +=(llong)(2*numCos*EC_GRID_DENS*(iE[2*i+1] - iE[2*i]) + .5);
    }
  if(symm == Remez::SY_NEG) numGdd--;


  gddF       = new ldouble[ numGdd      * EC_GRID_DENS + 3];
  gddG       = new ldouble[ numGdd      * EC_GRID_DENS + 3];
  gddW       = new ldouble[ numGdd      * EC_GRID_DENS + 3];
  gddXerrVal = new ldouble[ numGdd      * EC_GRID_DENS + 3];
  gddXlocDex = new llong [(numCos + 1) * EC_GRID_DENS + 3];
  gddXerrDex = new llong [(numCos + 1) * EC_GRID_DENS + 3];
  aryCos     = new ldouble[(numCos + 1) * EC_GRID_DENS + 3];
  x          = new ldouble[(numCos + 1) * EC_GRID_DENS + 3];
  y          = new ldouble[(numCos + 1) * EC_GRID_DENS + 3];
  ad         = new ldouble[(numCos + 1) * EC_GRID_DENS + 3];

  for(i=0; i<(numGdd * EC_GRID_DENS + 3); i++) {
    gddF[i]         = 0.0;
    gddG[i]         = 0.0;
    gddW[i]         = 0.0;
    gddXerrVal[i]   = 0.0;
  }
  for(i=0; i<(numCos + 1) * EC_GRID_DENS + 3; i++) {
    gddXlocDex[i]   = 0;
    aryCos[i]       = 0.0;
    x[i]            = 0.0;
    y[i]            = 0.0;
    ad[i]           = 0.0;
    gddXerrDex[i]   = 0;
  }
  return;
}
void   Remez::Teardown       ( void        ) {
  delete gddF;
  delete gddW;
  delete gddG;
  delete gddXerrVal;
  delete gddXlocDex;
  delete gddXerrDex;
  delete aryCos;
  delete x;
  delete y;
  delete ad;
  numCos = 1;
  numGdd = 0;
}







void   Remez::CreateDenseGrid( void        ) {
/* Creates the dense grid of frequencies from the specified iE.
     Also creates the Desired Frequency Response function
     (gddG[]) and the Weight function
     (gddW[]) on that dense grid
*/
  llong    i;
  llong    j;
  llong    k;
  llong    band;
  ldouble   delf;
  ldouble   lowf;
  ldouble   highf;
  ldouble   correctionFactor;


//   For FT_DIFF & hilbert, symm is odd and gddF[0] = max(delf, band[0])
  delf = 0.5 / ((ldouble)EC_GRID_DENS * (ldouble)numCos);
  if ((symm == Remez::SY_NEG) && (delf > iE[0])) iE[0] = delf;

  j=0;
  for (band=0; band < iNumB; band++) {
    gddF[j] = iE[2*band];
    lowf = iE[2*band];
    highf = iE[2*band + 1];
    k = (llong)((highf - lowf)/delf + 0.5);   // .5 for rounding - Convert FS-relative frequency into grid-domain integers
    for (i=0; i<k; i++) {
      gddG[j] = iG[band];
      gddW[j] = iW[band];
      gddF[j] = lowf;
      lowf += delf;
      j++;
      }
    gddF[j-1] = highf;
    }

/*
  Similar to above, if odd symm, last grid point can't be .5
    - but, if there are even taps, leave the last grid point at .5
*/
  if((symm == Remez::SY_NEG) && (gddF[numGdd-1] > (0.5 - delf)) && (iN % 2)) {
     gddF[numGdd-1] = 0.5-delf;
     }
// For Differentiator:(fix grid)
  if(iType == Remez::FT_DIFF) {
    for(i=0; i<numGdd; i++) {
// gddG[i] = gddG[i]*gddF[i]; // Code fix by commenting
      if(gddG[i] > DBL_EPS) gddW[i] = gddW[i] / gddF[i];
      }
    }

/*
 * For odd or Negative symm filters, alter the
 * gddG[] and gddW[] according to Parks & McClellan
 */
  if(symm == Remez::SY_POS) {
    if(iN % 2 == 0) {
      for(i=0; i<numGdd; i++) {
        correctionFactor             = cos(PI * gddF[i]);
        gddG[i] /= correctionFactor;
        gddW[i]   *= correctionFactor;
        }
      }
    }
  else {
    if(iN % 2) {
      for(i=0; i<numGdd; i++) {
        correctionFactor             = sin(PIx2 * gddF[i]);
        gddG[i] /= correctionFactor;
        gddW[i]   *= correctionFactor;
        }
      }
    else {
      for(i=0; i<numGdd; i++) {
        correctionFactor             = sin(PI * gddF[i]);
        gddG[i] /= correctionFactor;
        gddW[i]   *= correctionFactor;
        }
      }
    }
  return;
  } // End of function Remez::CreteDenseGrid()
void   Remez::InitialGuess   ( void        ) {
//Places Extremal Frequencies evenly throughout the dense grid.
  llong i;

  ldouble ge;
  ldouble id;
  ldouble gs = (ldouble)numGdd - 1.0;
  ldouble nc = (ldouble)numCos;
  for (i=0; i<=numCos; i++) {
    id = (ldouble)i;
    ge = id * gs / nc;
    if(i == 0) gddXlocDex[i] = 0;
    else       gddXlocDex[i] = (llong)(ge - DBL_EPS);
    }
  } // End of Function Remez::InitialGuess()
void   Remez::CalcParms      ( void        ) {
  llong    i;
  llong    j;
  llong    k;
  llong    ld;
  ldouble   sign;
  ldouble   xi;
  ldouble   delta;
  ldouble   denom;
  ldouble   numer;

// Find x[]
  for(i=0; i<=numCos; i++) {
    x[i] = cos(PIx2 * gddF[gddXlocDex[i]]);
    }


// Calculate ad[]  - Oppenheim & Schafer eq 7.132
  ld =(numCos - 1) / 15 + 1;         // Skips around to avoid round errors
  ld *= 3245983;
  ld += 1219874;
  ld = ld % numCos;
  ld--;
  for(i=0; i<=numCos; i++) {
    denom = 1.0;
    xi = x[i];
    for(j=0; j<ld; j++) {
      for(k=j; k<=numCos; k+=ld)
        if(k != i)
          denom *= 2.0*(xi - x[k]);
      }
    if(fabs(denom)<DBL_EPS) denom = DBL_EPS;
    ad[i] = 1.0/denom;
    }


// Calculate delta  - Oppenheim & Schafer eq 7.131
  numer = denom = 0.0;
  sign = 1.0;
  for(i=0; i<=numCos; i++) {
    numer += ad[i] * gddG[gddXlocDex[i]];
    denom += sign * ad[i]/gddW[gddXlocDex[i]];
    sign = -sign;
    }
  delta = numer/denom;
  sign = 1;


// Calculate y[]  - Oppenheim & Schafer eq 7.133b
  for(i=0; i<=numCos; i++) {
    y[i] = gddG[gddXlocDex[i]] - sign * delta/gddW[gddXlocDex[i]];
    sign = -sign;
    }
  return;
  } // End if Function Remez::CalcParms()
ldouble Remez::ComputeA       ( ldouble  i_f ) {
/* Using values calculated in CalcParms, ComputeA calculates the
     actual filter response at a given frequency(freq).  Uses
     eq 7.133a from Oppenheim & Schafer.
*/
  llong  i;
  ldouble xc;
  ldouble c;
  ldouble denom;
  ldouble numer;

  denom = numer = 0;
  xc = cos(PIx2 * i_f);
  for(i=0; i<=numCos; i++) {
    c = xc - x[i];
    if(dabs(c) < DBL_EPS) {
      numer = y[i];
      denom = 1;
      break;
      }
    c = ad[i]/c;
    denom += c;
    numer += c*y[i];
    }
    return numer/denom;
  }
void   Remez::CalcError      ( void        ) {
/* Calculates the Error function from the desired frequency response
    on the dense grid(gddG[]), the iW
    function on the dense grid(gddW[]),
    and the present response calculation(A[])
*/
  llong  i;
  ldouble A;

  for(i=0; i<numGdd; i++) {
    A = ComputeA(gddF[i]);
    gddXerrVal[i] = gddW[i] *(gddG[i] - A);
    }
  } // End if Function Remez::CalcError()
void   Remez::Search         ( void        ) {
/* Searches for the maxima/minima of the error curve.  If more than
    numCos+1 extrema are found, it uses the following heuristic(thanks
    Chris Hanson):
     1) Adjacent non-alternating extrema deleted first.
     2) If there are more than one excess extrema, delete the
        one with the smallest error.  This will create a non-alternation
        condition that is fixed by 1).
     3) If there is exactly one excess extremum, delete the smaller
        of the first/last extremum
*/
  llong  i;
  llong  j;
  llong  numExtrema;
  llong  l;
  llong  extra;
  bool   up;
  bool   alt;

  numExtrema = 0;

// Check for extremum at 0.0
  if(((gddXerrVal[0]>0.0) && (gddXerrVal[0] > gddXerrVal[1])) ||
     ((gddXerrVal[0]<0.0) && (gddXerrVal[0] < gddXerrVal[1])))
    gddXerrDex[numExtrema++] = 0;

// Check for extrema inside dense grid, stay off of end points, 0.0 & 0.5 checked that.
  for(i=1; i<numGdd - 1; i++) {
      if( ( (  gddXerrVal[i] >= gddXerrVal[i-1] )  &&       // upward or flat
            (  gddXerrVal[i] >  gddXerrVal[i+1] )  &&       // next is downward
            (  gddXerrVal[i] >             0.0  )      ) || // but is a positive error
          ( (  gddXerrVal[i] <= gddXerrVal[i-1] )  &&       // downward or flat
            (  gddXerrVal[i] <  gddXerrVal[i+1] )  &&       // next is upward
            (  gddXerrVal[i] <             0.0  )      ) )  // but is a negative error
        gddXerrDex[numExtrema++] = i;                       // THIS is an extremum
   }

// Check for extremum at 0.5
  j = numGdd-1;
  if(((gddXerrVal[j]>0.0) &&(gddXerrVal[j]>gddXerrVal[j-1])) ||
     ((gddXerrVal[j]<0.0) &&(gddXerrVal[j]<gddXerrVal[j-1])))
    gddXerrDex[numExtrema++] = j;


// Remove extra extrema
  fprintf(stderr, "      extrema: %5lld", numExtrema); fflush(stderr);
  extra = numExtrema - (numCos + 1);
  if(extra > 0) {
    fprintf(stderr, "      excess: %5lld", extra); fflush(stderr);
  }
  else {
    fprintf(stderr, "                   "); fflush(stderr);
  }
  while(extra > 0) {
    if(gddXerrVal[gddXerrDex[0]] > 0.0) up = true; // first one is a maximum
    else                                up = false; // first one is a minimum

    l   = 0;
    alt = true;
    for(j=1; j<numExtrema; j++) {
      if(fabs(gddXerrVal[gddXerrDex[j]]) < fabs(gddXerrVal[gddXerrDex[l]]))  l = j;               // new smallest error.
           if(( up) &&(gddXerrVal[gddXerrDex[j]] < 0.0))                     up = false;              // switch to a minimum
      else if((!up) &&(gddXerrVal[gddXerrDex[j]] > 0.0))                     up = true;              // switch to a maximum
      else {
        alt = false;
        break;                                                                                                    // Ooops, found two non-alternating
        } // extrema.  Delete smallest of them
      }  // if the loop finishes, all extrema are alternating


//  If there's only one extremum and all are alternating, delete the smallest of the first/last extremals.
  if((alt) &&(extra == 1)) {
    if(fabs(gddXerrVal[gddXerrDex[numExtrema-1]]) < fabs(gddXerrVal[gddXerrDex[0]]))  l = gddXerrDex[numExtrema-1];   // Delete last extremum
    else                                                                     l = gddXerrDex[0];     // Delete first extremum
    }

  for(j=l; j<numExtrema; j++)        // Loop that does the deletion
    gddXerrDex[j] = gddXerrDex[j+1];
  numExtrema--;
  extra--;
  }

  for(i=0; i<=numCos; i++)
    gddXlocDex[i] = gddXerrDex[i];       // Copy found extremals to gddXlocDex[]

  return;
}
bool   Remez::IsDone         ( void        ) {
/*
  Checks to see if the error function is small
  enough to consider the result to have converged.
*/
  llong  i;
  ldouble min;
  ldouble max;
  ldouble current;

  min = max = fabs(gddXerrVal[gddXlocDex[0]]);
  for(i=1; i<=numCos; i++) {
    current = fabs(gddXerrVal[gddXlocDex[i]]);
    if(current < min) min = current;
    if(current > max) max = current;
    }
  bool result;
  ldouble test;
  test = (max-min)/max;
  fprintf(stderr, "      residue: %25.21Lf", test); fflush(stderr);
  if(dabs(test) < convergeTest)
    result = true;
  else
    result = false;
  return result;
  } // End of Function Remez::IsDone()
void   Remez::FtoT           ( void        ) {
/*
Since the Remez Exchange Algorithm finds a minimax solution using cosine
approximators spaced at regular frequencies, we can transform these back into
an impulse function.
*/
  llong  n;
  llong  k;
  ldouble x;
  ldouble val;
  ldouble M;

  M =(iN-1.0)/2.0;
  if(symm == SY_POS) {
    if(iN % 2) {
      for(n=0; n<iN; n++) {
        val = aryCos[0];
        x = PIx2 *((ldouble)(n - M))/(ldouble)iN;
        for(k=1; k<=M; k++)
          val += 2.0 * aryCos[k] * cos(x*k);
        taps[n] = val/(ldouble)iN;
        }
      }
    else {
      for(n=0; n<iN; n++) {
        val = aryCos[0];
        x = PIx2 *(n - M)/iN;
        for(k=1; k<=(iN/2-1); k++) val += 2.0 * aryCos[k] * cos(x*k);
        taps[n] = val/iN;
        }
      }
    }
  else {
    if(iN % 2) {
      for(n=0; n<iN; n++) {
        val = 0;
        x = PIx2 *(n - M)/iN;
        for(k=1; k<=M; k++) val += 2.0 * aryCos[k] * sin(x*k);
        taps[n] = val/iN;
        }
      }
    else {
      for(n=0; n<iN; n++) {
        val = aryCos[iN/2] * sin(PI *(n - M));
        x = PIx2 *(n - M)/iN;
        for(k=1; k<=(iN/2-1); k++) val += 2.0 * aryCos[k] * sin(x*k);
        taps[n] = val/iN;
        }
      }
    }
  } // End of Function Remez::FtoT()





