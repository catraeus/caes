
// $Id: CaesStat.cpp 227 2022-12-25 00:44:45Z duncang $

//=================================================================================================
// Original File Name : caes_stat.cpp
// Original Author    : duncang
// Creation Date      : Oct 4, 2013
// Copyright          : Copyright © 2013 by Catraeus and Duncan Gray
//
// Description        :
//    Do statistical things.
//
//=================================================================================================

#include "CaesStat.hpp"
#include <math.h>
#include <thread>
#include <sigc++/sigc++.h>
#include <stdio.h>

     Stat::Stat         ( void        ) {
  SetVec(0, NULL);
}
     Stat::~Stat        ( void        ) {
  SetVec(0, NULL);
}
void Stat::CalcNormal   ( void        ) {
  CalcM    ();
  CalcS    ();
  CalcMin  ();
  CalcMax  ();
  return;
}
bool Stat::CalcTaylor3  ( double *o_t0, double *o_t1, double *o_t2) {
  bool result;

  double dN; // Double of N
  double dW; // The ((N-1)/2) number that is so magical.
  double dE; // Double of End Integer for Unit Width (-1.0 duh)
  double dI; // Double of delta for Unit Width

  double x;
  double ttt;
  double sumX2;
  double sumX4;
  double sumY;
  double sumXY;
  double sumX2Y;

  double S20; // One of the terms from equation
  double S40; // One of the terms from equation
  double S11; // One of the terms from equation
  double S21; // One of the terms from equation

  dN = (double) N;
  dE = -1.0D;
  dW = (dN - 1.0D) / 2.0D;
  dI =  2.0D / (dN - 1.0D);
  sumX2 = (dW / 3.0D + 0.5D + 1.0D / dW / 6.0D                               ) * 2.0D;
  sumX4 = (dW / 5.0D + 0.5D + 1.0D / dW / 3.0D - 1.0D /pow(dW, 3.0D) / 30.0D ) * 2.0D;

  sumY   = 0.0D;
  sumXY  = 0.0D;
  sumX2Y = 0.0D;
  for(llong i=0; i<N; i++) {
    x       = dE + (double)i * dI;
    ttt     = x * vec[i];
    sumY   += vec[i];
    sumXY  += ttt;
    ttt    *= x;
    sumX2Y += ttt;
  }

  S20 = sumX2;
  S40 = sumX4 - sumX2*sumX2/dN;
  S11 = sumXY;
  S21 = sumX2Y - sumX2*sumY/dN;

  *o_t2 = S21/S40;
  *o_t1 = S11/S20;
  *o_t0 = m - (*o_t2)*sumX2/dN;

  result = true;
  return result;
}
void Stat::CalcM        ( void        ) {

  if(!valid)
    return;
  m = 0.0;
  for(int i=0; i<N; i++)
    m += vec[i];
  m /= (double)N;
  dirty = false;
  return;
  }
void Stat::CalcS        ( void        ) {
  double   xx;

  if(!valid)
    return;
  s = 0.0;
  for(int i=0; i<N; i++) {
    xx  = vec[i] - m;
    xx *= xx;
    s  += xx;
    }
  s /= (double)N;
  s = sqrt(s);
  return;
  }
void Stat::CalcMin      ( void        ) {

  if(!valid)
    return;
  min = 1.0e302;
  minOff = 0;
  for(int i=0; i<N; i++)
    if(min > vec[i]) {
      min = vec[i];
      minOff = i;
    }
  return;
  }
void Stat::CalcMax      ( void        ) {

  if(!valid)
    return;
  max = -1.0e302;
  maxOff = 0;
  for(int i=0; i<N; i++)
    if(max < vec[i]) {
      max = vec[i];
      maxOff = i;
    }
  return;
  }
void Stat::CalcRr       ( Stat *i_st  ) {
  double  *v0;
  double  *v1;
  double   xx;
  double   ss;
  double   m0;
  double   m1;

  if(i_st != NULL) {
    if(!valid)
      return;
    if(i_st->GetN() != N)
      return;
    if(i_st->dirty) {
      i_st->CalcM();
      i_st->CalcS();
    }
    ss = 0.0;
    m0 = m;
    m1 = i_st->GetM();
    v0 = vec;
    v1 = i_st->GetVec();
    for(llong i=0; i<N; i++) {
      xx  = v0[i] - m0;
      xx *= v1[i] - m1;
      ss += xx;
    }
    ss /= (double)N;
    ss /= s;
    ss /= i_st->GetS();
    rr = ss;
    }
  else
    rr = 0.0;
  return;
}
void Stat::SetVec       ( llong   i_N, double *i_vec) {

  if(i_N > 0) {
    N        = i_N;
    valid    = true;
    vec      = i_vec;
  }
  else {
    N = 0;
    valid = false;
    vec = NULL;
  }
  m        = 0.0;
  s        = 0.0;
  a        = 0.0;
  b        = 0.0;
  g        = 0.0;
  min      = 0.0;
  minOff   = 0;
  max      = 0.0;
  maxOff   = 0;
  rr       = 0.0;
  dirty    = true;
  return;
}
