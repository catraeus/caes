

// $Id: CaesFFT.cpp 176 2019-06-08 22:49:01Z duncang $

//=================================================================================================
// Original File Name : CaesFFT.cpp
// Original Author    : duncang
// Creation Date      : Mar 6, 2007
// Copyright          : Copyright © 2007 - 2017 by Catraeus and Duncan Gray
//
//!< \brief FFT, due to Oppenheim and Schafer
//
//=================================================================================================


#include "CaesFFT.hpp"
#include "CaesMath.hpp"

       FFT::FFT          ( void    )  {
  inverse  = false;
  len  = 2048;
  inRe     = NULL;
  inIm     = NULL;
  outRe    = NULL;
  outIm    = NULL;
}
       FFT::~FFT         ( void    )  {
}
void   FFT::Calc         ( void    ) {
  ullong  radix;
  ullong  i;
  ullong  j;
  ullong  k;
  ullong  n;
  ullong  blkModu;
  ullong  blkEnd;
  double  twoPiNum;
  double  tmpRe;
  double  tmpIm;     /* temp real, temp imaginary */
  double  moduDbl;
  bool       err;

  if(inRe  == NULL)             err = true;
  if(inIm  == NULL)             err = true;
  if(outRe == NULL)             err = true;
  if(outIm == NULL)             err = true;
  if(!IsPowTwo(len))        err = true;
  if(err)    return;

  moduDbl = (double)len;
  twoPiNum = PIx2;
  if(inverse) twoPiNum = -twoPiNum;
  radix = Log2(len);

// Do simultaneous data copy and bit-reversal ordering into outputs...
  for(i=0; i < len; i++) {
    j = ReverseBits ( i, radix );
    outRe[j] = inRe[i];
    outIm[j] = inIm[i];
    }

// Do the FFT itself...
  blkEnd = 1;
  for(blkModu = 2; blkModu <= len; blkModu <<= 1) {
    double delta_angle = twoPiNum / (double)blkModu;
    double sm2 = sin(-2.0 * delta_angle );
    double sm1 = sin(      -delta_angle );
    double cm2 = cos(-2.0 * delta_angle );
    double cm1 = cos(      -delta_angle );
    double w   = 2 * cm1;
    double ar[3];
    double ai[3];

    for(i=0; i<len; i+=blkModu) {
      ar[2] = cm2;
      ar[1] = cm1;

      ai[2] = sm2;
      ai[1] = sm1;

      for(j=i, n=0; n<blkEnd; j++, n++ ) {
        ar[0] = w*ar[1] - ar[2];
        ar[2] = ar[1];
        ar[1] = ar[0];

        ai[0] = w*ai[1] - ai[2];
        ai[2] = ai[1];
        ai[1] = ai[0];

        k = j + blkEnd;
        tmpRe = ar[0]*outRe[k] - ai[0]*outIm[k];
        tmpIm = ar[0]*outIm[k] + ai[0]*outRe[k];

        outRe[k] = outRe[j] - tmpRe;
        outIm[k] = outIm[j] - tmpIm;

        outRe[j] += tmpRe;
        outIm[j] += tmpIm;
        }
      }

    blkEnd = blkModu;
    }

    //   Need to normalize if inverse transform...
  if(inverse) {
    for(i=0; i<len; i++) {
      outRe[i] /= moduDbl;
      outIm[i] /= moduDbl;
      }
    }
  else {
    for(i=0; i<len; i++) {
      outRe[i] /= moduDbl;
      outIm[i] /= moduDbl;
      }
    }
  }
double FFT::NormFreq     ( ullong x ) {
  double f;

  if     (x >= len)    f = 0.0;
  else if(x <= len/2)  f = (double) x          / (double)len;
  else                     f = (double)(x-len) / (double)len;
  return f;
  }
void   FFT::ToPower      ( void) {
  ullong i;
  ullong mid;
  double r;
  double j;

  mid = len/2;

  for(i=0; i<len; i++) {
    r = outRe[i];
    j = outIm[i];
    outRe[i] = sqrt(r*r+j*j);
    outIm[i] = 0.0;
    }
  for(i=1; i<len; i++) {
    r = outRe[i];
    j = outRe[len-i];
    if(i < mid) outRe[i] = sqrt(r*r+j*j);
    else        outRe[i] = 0.0;
    }

  return;
  }
ullong FFT::SetLen       ( ullong i_m ) {
  if(i_m < 4)
    i_m = 4;
  if(i_m > EC_MAX_MODU)
    i_m = EC_MAX_MODU;
  len = i_m;
  return len;
}
void   FFT::SetAry       ( double *i_r, double *i_i, double *o_r, double *o_i ) {
  inRe  = i_r;
  inIm  = i_i;
  outRe = o_r;
  outIm = o_i;
  return;
}
