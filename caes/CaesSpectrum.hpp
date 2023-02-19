// $Id: CaesSpectrum.hpp 176 2019-06-08 22:49:01Z duncang $

//=================================================================================================
// Original File Name : caes_freq.hpp
// Original Author    : duncang
// Creation Date      : Aug 23, 2013
// Copyright          : Copyright © 2013 by Catraeus and Duncan Gray
//
// Description        :
//    Spectrum Transform.  Takes in time, outputs frequency.
/*

  Hard stuck rule:
    There are Nt input points.  This fixes the finest deltaW tha twe can know (after Gabor)
  Tough rule with a little wiggle room:
    The locations that can have spqctrum calc should be at integer multiples of length / Nt.
    The farther we are away from DC, the more windowing saves us here.

  time and frequency arrays are up to callers.  We don't new or delete these.

  What the customer wants:
    Given a time array of size Nt.
    Given a freq array of size Nf.
    Given a freq span from freqT to freqP (normalized to (0.0 to 0.5 of sample rate)
      FIXME For now that span will always start at 0.0 and stop at 0.5

    Make a sine/cosine array on the deltaF points from freqT to freqP inclusive for later multiply
      This array has to be Nt long.
      Nt/Nf tells whether the low freq end will be wonky.  But just let fly.
      For Nt != k * Nf Gibbs will introduce bias ... I'll figure that out later.
*/
//
//=================================================================================================

#ifndef __CAEV_SPECTRAN_HPP_
#define __CAEV_SPECTRAN_HPP_
#include <caes/CaesMath.hpp>
#include <caes/CaesFFT.hpp>

class SpecTran {
  private:
    enum eConst {
      EK_MIN =         4,
      EK_DFT =      2048, //!< LE will get DFT, GT will get FFT
      EK_MAX =  256*1024
    };
  public:
                  SpecTran    ( void                         );
            void  SetTimeAry  ( double *i_tRe, double *i_tIm );
            void  SetFreqAry  ( double *i_fRe, double *i_fIm );
    virtual      ~SpecTran    ( void                         );
            void  Resize      (ullong   i_Nt,  ullong  i_Nf  );
            void  FFTr        ( void                         );
            void  FFTc        ( void                         );
            void  DFTrPS      ( void                         );
            void  DFTc        ( void                         );
            void  DFTlinPhase ( void                         );
            bool  GetFFTvDFT  ( void                         ) {return FFTvDFT;};
  private:
    double  *tRe;   //!< Real time input -- deltaT always 1.0
    double  *tIm;   //!< Imaginary time input -- deltaT always 1.0
    double  *fRe;   //!< Real frequency output -- Min freq -0.5, max 0.5
    double  *fIm;   //!< Imaginary frequency output -- Min freq -0.5, max 0.5
    double **c;     //!< Cosine array for DFT
    double **s;     //!< Sine   array for DFT
    double   fT;    //!< start frequency in FS=1 normalized
    double   fP;    //!< stop frequency in FSA=1 normalized
    ullong   Nf;    //!< Size of the frequency output array
    ullong   Nt;    //!< Size of the time input array
    bool     isOdd;
    bool     FFTvDFT;

    FFT     *tFFT;
  };

#endif /* __CAEV_SPECTRAN_HPP_ */
