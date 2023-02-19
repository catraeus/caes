
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

     Stat::Stat         ( llong   i_N, double **i_ary, llong i_ch) {
  m      = NULL;
  s      = NULL;
  a      = NULL;
  b      = NULL;
  g      = NULL;
  min    = NULL;
  minOff = NULL;
  max    = NULL;
  maxOff = NULL;
  rr     =  0.00;
  SetVec(i_N, i_ary, i_ch);
}
     Stat::Stat         ( void        ) {
  m      = NULL;
  s      = NULL;
  a      = NULL;
  b      = NULL;
  g      = NULL;
  min    = NULL;
  minOff = NULL;
  max    = NULL;
  maxOff = NULL;
  rr     =  0.00;
  SetVec(0, NULL, 0);
}
     Stat::~Stat        ( void        ) {
  SetVec(0, NULL, 0);
}
void Stat::Calc         ( void        ) {
  CalcM    ();
  CalcS    ();
  CalcMin  ();
  CalcMax  ();
  CalcRr   ();
  return;
}
void Stat::CalcM        ( void        ) {
  const llong    NN = N;
        double  *vv;
        double   xx;
  if(!valid)
    return;
  for(int j=0; j<ch; j++) {
    xx = 0.0;
    vv = ary[j];
    for(int i=0; i<NN; i++)
      xx += vv[i];
    xx /= (double)NN;
    m[j] = xx;
    }
  return;
  }
void Stat::CalcS        ( void        ) {
  const llong    NN = N;
        double  *vv;
        double   xx;
        double   ss;
        double   mm;
  if(!valid)
    return;
  for(int j=0; j<ch; j++) {
    ss = 0.0;
    mm = m[j];
    vv = ary[j];
    for(int i=0; i<NN; i++) {
      xx  = vv[i] - mm;
      xx *= xx;
      ss += xx;
      }
    ss /= (double)NN;
    ss = sqrt(ss);
    s[j] = ss;
    }
  return;
  }
void Stat::CalcMin      ( void        ) {
  const llong    NN = N;
        double  *vv;
        double   xx;
        llong    oo;
  if(!valid)
    return;
  for(int j=0; j<ch; j++) {
    xx = 1.0e302;
    vv = ary[j];
    oo = 0;
    for(int i=0; i<NN; i++)
      if(xx > vv[i]) {
        xx = vv[i];
        oo = i;
      }
    min   [j] = xx;
    minOff[j] = oo;
    }
  return;
  }
void Stat::CalcMax      ( void        ) {
  const llong    NN = N;
        double  *vv;
        double   xx;
        llong    oo;
  if(!valid)
    return;
  for(int j=0; j<ch; j++) {
    xx = -1.0e302;
    vv = ary[j];
    oo = 0;
    for(int i=0; i<NN; i++)
      if(xx < vv[i]) {
        xx = vv[i];
        oo = i;
      }
    max   [j] = xx;
    maxOff[j] = oo;
    }
  return;
  }
void Stat::CalcRr       ( void        ) {
  if(ch == 2) {
    const llong    NN = N;
          double  *v0;
          double  *v1;
          double   xx;
          double   ss;
          double   m0;
          double   m1;
    if(!valid)
      return;
    ss = 0.0;
    m0 = m[0];
    m1 = m[1];
    v0 = ary[0];
    v1 = ary[1];
    for(int i=0; i<NN; i++) {
      xx  = v0[i] - m0;
      xx *= v1[i] - m1;
      ss += xx;
      }
    ss /= (double)NN;
    ss /= s[0];
    ss /= s[1];
    rr = ss;
    }
  else
    rr = 0.0;
  return;
  }
void Stat::SetVec       ( llong   i_N, double **i_ary, llong i_ch) {
  valid = false;
  if(m        != NULL)   { delete m;        m        = NULL; }
  if(s        != NULL)   { delete s;        s        = NULL; }
  if(a        != NULL)   { delete g;        a        = NULL; }
  if(b        != NULL)   { delete b;        b        = NULL; }
  if(g        != NULL)   { delete a;        g        = NULL; }
  if(min      != NULL)   { delete min;      min      = NULL; }
  if(minOff   != NULL)   { delete minOff;   minOff   = NULL; }
  if(max      != NULL)   { delete max;      max      = NULL; }
  if(maxOff   != NULL)   { delete maxOff;   maxOff   = NULL; }

  N        = 0;
  ary      = NULL;
  ch       = 0;
  valid    = false;
  dirty    = true;
  // TODO  Make a test for a maximum number of input channels, maybe.
  if((i_N != 0) && (i_ary != NULL) && (i_ch != 0)) {
    N        = i_N;
    ary      = i_ary;
    ch       = i_ch;
    m        = new double[ch];
    s        = new double[ch];
    a        = new double[ch];
    b        = new double[ch];
    g        = new double[ch];
    min      = new double[ch];
    minOff   = new llong [ch];
    max      = new double[ch];
    maxOff   = new llong [ch];
    for(int i=0; i<ch; i++) {
      m[i]        = 0.0;
      s[i]        = 0.0;
      a[i]        = 0.0;
      b[i]        = 0.0;
      g[i]        = 0.0;
      min[i]      = 0.0;
      minOff[i]   = 0;
      max[i]      = 0.0;
      maxOff[i]   = 0;
      }
    }
  rr       = 0.0;
  valid = true;
  return;
  }
