
// $Id: CaesFFT.hpp 204 2022-11-21 04:29:00Z duncang $

//=================================================================================================
// Original File Name : CaesFFT.hpp
// Original Author    : duncang
// Creation Date      : Mar 6, 2007
// Copyright          : Copyright © 2007 - 2017 by Catraeus and Duncan Gray
//
//!< \brief FFT
//
//=================================================================================================


#ifndef __CAES_FFT_HPP_
#define __CAES_FFT_HPP_

#include <stdio.h>
#include <math.h>

#include <caes/CaesTypes.hpp>

class FFT {
  private:
    enum eConst {
      EC_MAX_MODU = 256*1024
    };
  public:
                      FFT          ( void        );
    virtual          ~FFT          ( void        );
            void      Calc         ( void        );
            void      ToPower      ( void        );
            ullong    SetLen       ( ullong  i_m );
            void      SetAry       ( double *i_r, double *i_i, double *o_r, double *o_i );
            void      SetDirFwd    ( void        );
            void      SetDirRev    ( void        );
  private:
            double    NormFreq     ( ullong  i_x );
  public:
  private:
    ullong     len;
    bool       inverse;
    double    *inRe;
    double    *inIm;
    double    *outRe;
    double    *outIm;
  };
#endif // __FFT_HPP_
