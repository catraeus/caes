
// $Id: CaesRemez.hpp 234 2023-01-23 22:27:00Z duncang $

//=================================================================================================
// Original File Name : CaesRemez.hpp
// Original Author    : duncang
// Creation Date      : Jan 14, 2005
// Copyright          : Copyright © 2015 - 2019 by Catraeus and Duncan Gray
//
// Description        :
//    Make the Remez vector using that brilliant algorithm
//
//=================================================================================================

#ifndef __CAES_REMEZ_HPP_
#define __CAES_REMEZ_HPP_

#include <caes/CaesTypes.hpp>
#include <caes/CaesMath.hpp>

class Remez {
  private:
    enum eSymm {SY_POS, SY_NEG};
    enum eConst {
      EC_MAX_N      = 2999, // pretty conservative for now.
      EC_MAX_B      =    4, // In bands, so there are 8 edges.
      EC_GRID_DENS  =  128, // wildly larger than in 1975, but then RAM was billions of dollars per gig.
      EC_MAX_ITER   = 1587
    };
  public:
    enum eFiltType {FT_BAND, FT_DIFF, FT_HILB};
  public:
                    Remez(void);
    virtual        ~Remez(void);
            void    SetTypeBandPass ( void                    ) { iType   = FT_BAND                   ;};
            void    SetTypeDiff     ( void                    ) { iType   = FT_DIFF                   ;};
            void    SetTypeHilbert  ( void                    ) { iType   = FT_HILB                   ;};
            void    SetEdge         ( llong   i_b, ldouble i_f );
            ldouble  GetEdge         ( llong   i_b             ) {                    return iE[i_b]   ;};
            void    SetAry          ( ldouble *i_a             );
            void    SetNumB         ( llong   i_b             ) { iNumB   = i_b                       ;};
            llong   GetNumB         ( void                    ) {                    return iNumB     ;};
            void    SetG            ( llong   i_b, ldouble i_g ) { iG[i_b] = i_g                       ;};
            ldouble  GetG            ( llong   i_b             ) {                    return iG[i_b]   ;};
            void    SetW            ( llong   i_b, ldouble i_w ) { iW[i_b] = i_w                       ;};
            ldouble  GetW            ( llong   i_b             ) {                    return iW[i_b]   ;};
            llong   GetNmax         ( void                    ) {                    return EC_MAX_N  ;};
            void    SetN            ( llong   i_n             ) { iN      = i_n                       ;};
            ldouble  GetN            ( void                    ) {                    return iN        ;};

            bool    IsValid         ( void        );
            void    Calculate       ( void        );
            void    NormDC          ( void        );
            void    NormRMS         ( void        );
            void    NormSF          ( ldouble i_fs );
            void    NormPeak        ( void        );

  private:
            void    Setup           ( void        );
            void    Teardown        ( void        );

            void    CreateDenseGrid ( void        );
            void    InitialGuess    ( void        );
            void    CalcParms       ( void        );
            ldouble  ComputeA        ( ldouble  i_a );
            void    CalcError       ( void        );
            void    Search          ( void        );
            bool    IsDone          ( void        );
            void    FtoT            ( void        );

  public:
  private:
    llong     iN;    // Number of taps in the resulting filter
    llong     iNumB; // Number of bands in user specification
    ldouble   *taps;  // The point of the whole thing
    ldouble   *iE;    // User-specified band edges [2*iNumB]
    ldouble   *iG;    // Desired response per band [iNumB]
    ldouble   *iW;    // Approximation weighting of each band
    eFiltType iType;

  private:
    eSymm   symm;   // Symmetry of filter - used for grid creation

    // gdd means "gridded" as in located onto the dense array for maximals loci
    llong   numGdd;      // Number of elements in the dense frequency grid
    ldouble *gddF;        // Frequencies (0 to 0.5) on the dense grid [numGdd]
    ldouble *gddG;        // Desired response on the dense grid [numGdd]
    ldouble *gddW;        // Weight function on the dense grid [numGdd]
    ldouble *gddXerrVal;
    llong  *gddXerrDex;
    llong  *gddXlocDex;  // Extremal indexes to dense frequency grid [numCos+1]

    llong   numCos;      // 1/2 the number of filter coefficients
    ldouble *aryCos;
    ldouble *ad;          // Originally 'b' in Oppenheim & Schafer [numCos+1]
    ldouble *x;           // [numCos+1]
    ldouble *y;           // Originally 'C' in Oppenheim & Schafer [numCos+1]

    ldouble  convergeTest;
};
#endif // __REMEZ_HPP_
