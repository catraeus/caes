
// $Id: CaesMath.hpp 231 2022-12-29 02:47:03Z duncang $

//=================================================================================================
// Original File Name : caes.h
// Original Author    : duncang
// Created on         : Dec 31, 2007
// Copyright          : Copyright © 2007 - 2017 by Catraeus and Duncan Gray
// Description        : Many functions to do many things.
//=================================================================================================


#ifndef __CAES_MATH_H_
#define __CAES_MATH_H_

#include <math.h>
#include <caes/CaesTypes.hpp>

//static inline double dabs(double x) {return (double)(((ullong)x) & 0x7fffffffffffffff);}
//==  Conversion, min and max functions are to some extent a conversion function
void    caes_Conv_f64v_i32i_s ( llong   N,  double  *vA,  double *vB, void   *dst          );
void    caes_Conv_f64v_i16i_s ( llong   N,  double  *vA,  double *vB, void   *dst          );
void    caes_Conv_i32i_f64v_s ( llong   N,  void    *src, double *vA, double *vB           );
void    caes_Conv_i16i_f64v_s ( llong   N,  void    *src, double *vA, double *vB           );
int     caes_double2int       ( double  f            );
int     caes_float2int        ( float   f            );
double  dmin                  ( double  x, double  y );
double  dmax                  ( double  x, double  y );
ullong  ullmin                ( ullong  x, ullong  y );
 llong  llmin                 (  llong  x,  llong  y );
ullong  ullmax                ( ullong  x, ullong  y );
 llong  llmax                 (  llong  x,  llong  y );
double  dabs                  ( double  x            );
//==  Signals
void    ConvolveExt           ( llong  Na, double  *a,   llong  Nb, double *b, double *o, void (*Report)(double *, void *), void *ClasPtr ); //!< Exterior convolution
void    ConvolveInt           ( llong  Na, double  *a,   llong  Nb, double *b, double *o, void (*Report)(double *, void *), void *ClasPtr ); //!< Exterior convolution
//==  Numbers, Integer
ullong  GCF                   ( ullong  a,  ullong   b                                     );
void    caes_factorize        ( ullong  a,  ullong  *N,   ullong *f                        );
ullong  ReverseBits           ( ullong  x,  ullong   N );
//==  Numbers, Real
ullong  Log2                  ( ullong  N            );
ullong  NextPowTwo            ( ullong  i            );
ldouble pow10                 ( ldouble x            );
bool    IsPowTwo              ( ullong  x            );
ldouble floorld               ( ldouble x            );
//==  Stats
double  caes_RandRect         ( void                 );
void    caes_RandGaussVec     ( ullong N, double **v );
double  caes_RandGauss        ( void                 );

#endif // __CAEV_MATH_H_

