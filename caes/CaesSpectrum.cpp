// $Id: CaesSpectrum.cpp 176 2019-06-08 22:49:01Z duncang $

//=================================================================================================
// Original File Name : caes_freq.cpp
// Original Author    : duncang
// Creation Date      : Aug 23, 2013
// Copyright          : Copyright © 2013 by Catraeus and Duncan Gray
//
// Description        :
/*

    The time      domain has Nt points.
    The frequency domain has Nf points.
    Time is straight, from zero with Nt points.
    Frequency is single, from start to stop with Nf points.
    If start frequency is 0.0, then size is actually doubled.

    Use Cases:
      Analyze signal content
        Usually only want power spectra, running, of a live signal
        Needs apodization setting and execution interface.
        Wants to narrow in on small spectra.
        Wants either linear or log view mode.
      Analyze noise floor
        Needs a multiple capture averager mode control.
        Needs apodization setting and execution interface.
        Wants to narrow in on small spectra.
        Wants either linear or log view mode.
      Analyze impulse responses
        Needs a zero data = zero time vs. mid data = zero time mode control.
        Needs phase info for arb filters
        Nothing special for LinearPhase FIR responses
        Turns off apodization
        Wants to narrow in on small spectra.
        Wants either linear or log view mode.

    Where to put zero-time?
      LinearPhase FIR is the only one that will put it mid-space

    For real-only time data:
      start-to-stop
    For imag time data:

*/
//=================================================================================================

#include "CaesSpectrum.hpp"
#include <math.h>
#include <stdio.h>

     SpecTran::SpecTran     ( void                          ) { //!< Build without any info, pointers to zero, lengths to benign values
  tRe     = NULL;
  tIm     = NULL;
  fRe     = NULL;
  fIm     = NULL;

  fT      = 0.0;
  fP      = 0.5;


  c = new double*[EK_DFT];  for(uint ii=0; ii<EK_DFT; ii++)    c[ii] = new double[EK_DFT];
  s = new double*[EK_DFT];  for(uint ii=0; ii<EK_DFT; ii++)    s[ii] = new double[EK_DFT];

  Nt      = 1;
  Nf      = 1;

  isOdd     = false;
  doFFTvDFT = true;

  tFFT      = new FFT();

  return;
}
     SpecTran::~SpecTran    ( void                          ) {
  for(uint ii=0; ii<EK_DFT; ii++)    delete c[ii];
  delete c;
  for(uint ii=0; ii<EK_DFT; ii++)    delete s[ii];
  delete s;
}
void SpecTran::SetTimeAry   ( double *i_tRe, double *i_tIm  ) {
  tRe   = i_tRe;
  tIm   = i_tIm;
  tFFT->SetAry(tRe, tIm, fRe, fIm);
  return;
  }
void SpecTran::SetFreqAry   ( double *i_fRe, double *i_fIm  ) {
  fRe   = i_fRe;
  fIm   = i_fIm;
  tFFT->SetAry(tRe, tIm, fRe, fIm);
  return;
  }
void SpecTran::Resize       ( ullong  i_Nt,  ullong  i_Nf   ) {
  double wT;  //!< start radian frequency
  double dW;  //!< radian frequency increment
  double tT;  //!< start time, will be x.5 for even size time arrays.
  double corr;

  if(i_Nt < EK_MIN )
    return;
  if(i_Nt > EK_MAX)
    return;
  if(i_Nf < EK_MIN)
    return;
  if(i_Nf > EK_MAX)
    return;
  if((i_Nt == Nt) && (i_Nf == Nf))
    return;
  Nt = i_Nt;
  Nf = i_Nf;
  isOdd   = ((int)((uint)Nf & 0x00000001) == 1);
  tFFT->SetLen(Nt);
  doFFTvDFT = false;
  if((Nt > EK_DFT) || (Nf > EK_DFT)) {
    doFFTvDFT = true;
    return;
  }
  tT      = -0.5 * ((double)(Nt - 1)); // in N domain, not w.  This makes the time sample symmetrical about its center.
  dW      = PIx2 * (fP - fT) /  (double)(Nf); // So that the last point is the folding frequency (stop frequency) not penultimate to it.
  wT      = PIx2 * fT;

  corr    = 2.0 / (double)Nt;
  for(ullong fDex = 0; fDex < Nf; fDex++) {
    double w = wT + (double)fDex * dW;
    for(ullong tDex = 0; tDex < Nt; tDex++) {
      double t = tT + (double)tDex;
      c[fDex][tDex] = cos(w*t) * corr; // pre-compensate for the integration and half-side spectrum
      s[fDex][tDex] = sin(w*t) * corr; // pre-compensate for the integration and half-side spectrum
      }
    }
  return;
  }

void SpecTran::Calc         ( void                          ) {
  if(tRe == NULL)
    return;
  if(Nt <=  EK_DFT) {
    doFFTvDFT = false;
    DFTr();
  }
  else {
    doFFTvDFT = true;
    FFTr();
  }
  return;
}

void SpecTran::FFTr         ( void                          ) {
  tFFT->Calc();
  return;
  }
void SpecTran::FFTc         ( void                          ) {
  fprintf(stderr, "We just did nothing in SpecTran::FFTc\n"); fflush(stderr);
  return;
  }
void SpecTran::DFTr         ( void                          ) {
  for(ullong fDex = 0; fDex < Nf; fDex++) { // HOORAY This is IT, man ... do the transform cha-cha ... cha-cha
    fRe[fDex] = 0.0;
    fIm[fDex] = 0.0;
    for(ullong tDex=0; tDex < Nt; tDex++) {
      fRe[fDex] += c[fDex][tDex] * tRe[tDex];
      fIm[fDex] += s[fDex][tDex] * tRe[tDex];
    }
  }
  return;
  }
void SpecTran::DFTc         ( void                          ) {
  return;
  }
void SpecTran::DFTlinPhase  ( void                          ) { // Assumes symmetry about middle.
  ullong     Ncos;
  ullong     Mo;
  double  df;
  double *pCos;

  df = 1.0 / ((double)(Nf - 1));

  Ncos  = Nf >> 1;
  Mo    = Ncos;
  if(isOdd)
    Mo++;
  pCos = &tRe[Mo - 1]; // to make the 1-based pointer math work in the loop below
  // If it's odd, the frequencies will be at cos(k*2*PI) and include a "DC" term. (k 1 based)
  // else         the frequencies will be at cos((k*2-1)*PI) and have no "DC" term. (k 1 based)
  // FIXME This is a hack for zero-spanning spectra.  It needs to be double-sided when off of zero.
  // FIXME This needs to be grossed up to make an even spectrum have a DC in the middle, like the FFT.
  if(isOdd) { // The zero-in-time point is an impulse that needs spreading out across the spectrum
    for(ullong i=0; i<Nf; i++)
      fRe[i] = tRe[Mo - 1] * 0.5;
    }
  else { // else, there is no middle.
    for(ullong i=0; i<Nf; i++)
      fRe[i] = 0.0;
    }
  if(isOdd) {
    df *= PIx2;
    for(ullong k=1; k<=Ncos; k++)
      for(ullong i=0; i<Nf; i++)
        fRe[i] += pCos[k] * cos((double)(k * i) * df);
    }
  else {
    df *= PI;
    for(ullong k=1; k<=Ncos; k++) {
      double td = pCos[k];
      double tk = (double)(2*k - 1);
      for(ullong i=0; i<Nf; i++) {
        fRe[i] += td * cos(tk * (double)i * df);
        }
      }
    }
  for(ullong i=0; i<Nf; i++)
    fRe[i] /= (double)Ncos;
  return;
  }
void SpecTran::ToPS         ( void                          ) {
  for(ullong ii = 0; ii < Nt; ii++)    fRe[ii] *= fRe[ii];
  for(ullong ii = 0; ii < Nt; ii++)    fIm[ii] *= fIm[ii];
  for(ullong ii = 0; ii < Nt; ii++)    fRe[ii] += fIm[ii];
  for(ullong ii = 0; ii < Nt; ii++)    fRe[ii]  = sqrt(fRe[ii]);
  return;
}
