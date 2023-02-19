
// $Id: CaesMath.cpp 231 2022-12-29 02:47:03Z duncang $

//=================================================================================================
// Original File Name : caes.c
// Original Author    : duncang
// Created on         : Apr 16, 2011
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
// Description        : Vector functions to do many things.
//=================================================================================================


#include <caes/CaesMath.hpp>
#include <caes/CaesTypes.hpp>
#include <stdlib.h>
#include <math.h>
#include <iostream>

#define __IN_CAEV_MATH_CPP_
#define BIGNO_INT32      (int)    (        0x7fffffff)
#define BIGNO_INT16      (int)    (            0x7fff)
#define BIGNO_RANDDBL    (llong)  (0x0fffffffffffffff)
#define RAND_RECT_FACTOR          (1.613098018221020000E-09)
#define RAND_RECT_OFF    (llong)  (0x0000000040000000)
#define RAND_GAUSS_FACTOR         (4.656612875245800000E-10)
#define RAND_GAUSS_OFF   (llong)  (0x0000000300000000)
#include <caes/CaesPrimes.hpp>
//==  Conversions
       void     caes_Conv_f64v_i32i_s ( llong N, double *vA,  double *vB, void   *dst) {
  int     *pDst;
  //ullong   t1;
  //ullong   t2;
  double *pL;
  double *pR;

  pL = vA;
  pR = vB;
  pDst = (int *)dst;


  //i = 0;
  //__asm__ __volatile__("rdtsc": "=A" (t1));
  while(N-- > 0) {
    *(pDst++) = (int)(*(pL++));
    *(pDst++) = (int)(*(pR++));
    }
  //__asm__ __volatile__("rdtsc": "=A" (t2));
  //fprintf(stdout, "that took %Lu\n", (t2 - t1));
  return;
  }
       void     caes_Conv_f64v_i16i_s ( llong N, double *vA,  double *vB, void   *dst) {
  word   *pDst;
  //ullong   t1;
  //ullong   t2;
  double *pL;
  double *pR;

  pL = vA;
  pR = vB;
  pDst = (word *)dst;


  //i = 0;
  //__asm__ __volatile__("rdtsc": "=A" (t1));
  while(N-- > 0) {
    *(pDst++) = (word)(*(pL++));
    *(pDst++) = (word)(*(pR++));
    }
  //__asm__ __volatile__("rdtsc": "=A" (t2));
  //fprintf(stdout, "that took %Lu\n", (t2 - t1));
  return;
  }
       void     caes_Conv_i32i_f64v_s ( llong N, void   *src, double *vA, double *vB) {
  int     *pSrc;
  //ullong   t1;
  //ullong   t2;
  double *pL;
  double *pR;
  static const double K=1.0 / (double)(BIGNO_INT32 );

  pL = vA;
  pR = vB;
  pSrc = (int *)src;



  //i = 0;
  //__asm__ __volatile__("rdtsc": "=A" (t1));
  while(N-- > 0) {
    *(pL++) = K * (double)(*(pSrc++));
    *(pR++) = K * (double)(*(pSrc++));
    }
  //__asm__ __volatile__("rdtsc": "=A" (t2));
  //fprintf(stdout, "that took %Lu\n", (t2 - t1));
  return;
  }
       void     caes_Conv_i16i_f64v_s ( llong N, void   *src, double *vA, double *vB) {
  word   *pSrc;
  //ullong   t1;
  //ullong   t2;
  double *pL;
  double *pR;
  double  K;

  pL = vA;
  pR = vB;
  pSrc = (word *)src;
  K = 1.0 / (double)(BIGNO_INT16);


  //i = 0;
  //__asm__ __volatile__("rdtsc": "=A" (t1));
  while(N-- > 0) {
    *(pL++) = K * (double)(*(pSrc++));
    *(pR++) = K * (double)(*(pSrc++));
    }
  //__asm__ __volatile__("rdtsc": "=A" (t2));
  //fprintf(stdout, "that took %Lu\n", (t2 - t1));
  return;
  }
inline int      caes_double2int       ( double flt  ) {
         int intgr;
         __asm__ __volatile__ ("flds %1; fistpl %0;" : "=m" (intgr) : "m" (flt));
         return intgr ;
         }
inline int      caes_float2int        ( float  flt  ) {
         int intgr;
         __asm__ __volatile__ ("flds %1; fistpl %0;" : "=m" (intgr) : "m" (flt));
         return intgr;
         }
       double   dmin                  ( double  x, double  y ) {return x < y ? x : y;}
       double   dmax                  ( double  x, double  y ) {return x > y ? x : y;}
       ullong   ullmin                ( ullong  x, ullong  y ) {return x < y ? x : y;}
       llong    llmin                 (  llong  x,  llong  y ) {return x < y ? x : y;}
       ullong   ullmax                ( ullong  x, ullong  y ) {return x > y ? x : y;}
       llong    llmax                 (  llong  x,  llong  y ) {return x > y ? x : y;}
       double   dabs                  ( double  x            ) {return x > 0.0 ? x : -1.0 * x ;}
//==  Signals
       void     ConvolveExt           ( llong i_Na, double *i_a, llong i_Nb, double *i_b, double *o_o, void (*Report)(double *, void *), void *ClasPtr) {
  ullong  numShort; // We can get any length you please, but it is commutative and easier to do one way vs. the other.
  double *vecShort;
  ullong  numLong;
  double *vecLong;
  double *o;
  double  completeness;

  o = o_o;
                              numShort = i_Na;  numLong = i_Nb;  vecShort  = i_a;  vecLong  = i_b;        // vecShort is shorter
  if(numShort > numLong) {    numShort = i_Nb;  numLong = i_Na;  vecShort  = i_b;  vecLong  = i_a;  }     // vecLong is shorter
  // Things about the whole overall convolution
  ullong   wholeCount = 0;
  ullong   termCount  = numShort + numLong - 1; // Is an exterior convo.
  double  *pOut   = o;
  // Things about the intro convolution
  ullong   convoLen   = 1;
  while(wholeCount < numShort ) {
    double sum;
    sum = 0.0;
    double *pBhead = &(vecLong[wholeCount]);
    double *pAhead = vecShort;
    for(ullong dex = 0; dex < convoLen; dex++) {
      sum += *pBhead * *pAhead;
      pAhead++;
      pBhead--;
    }
    *pOut = sum;
    pOut++;
    wholeCount++;
    convoLen++;
  }
  // Now convoLen better be numShort
  // Things about the body convolution
  while(wholeCount <= numLong) {
    double sum;
    sum = 0.0;
    double *pBhead = &(vecLong[wholeCount]);
    double *pAhead = vecShort;
    if((wholeCount % 1000000) == 0) {
      completeness = (double)wholeCount / (double)numLong;
      if(Report != NULL) Report(&completeness, ClasPtr);
    }
    for(ullong dex = 0; dex < convoLen; dex++) {
      sum += *pBhead * *pAhead;
      pAhead++;
      pBhead--;
    }
    *pOut = sum;
    pOut++;
    wholeCount++;
  }
  // Things about the outro convolution
  convoLen--;
  while(wholeCount < termCount) {
    double sum;
    sum = 0.0;
    double *pBhead = &(vecLong[numLong - 1]);
    double *pAhead = &(vecShort[numShort - convoLen]);
    for(ullong dex = 0; dex < convoLen; dex++) {
      sum += *pBhead * *pAhead;
      pAhead++;
      pBhead--;
    }
    *pOut = sum;
    pOut++;
    wholeCount++;
    convoLen--;
  }
  return;
}
       void     ConvolveInt           ( llong i_Na, double *i_a, llong i_Nb, double *i_b, double *o_o, void (*Report)(double *, void *), void *ClasPtr) {
  ullong   numShort; // We can get any length you please, but it is commutative and easier to do one way vs. the other.
  double  *vecShort;
  double  *pShortHead;
  ullong   numLong;
  double  *pLongHead;
  double  *vecLong;

  llong    convoLen;
  ullong   wholeCount;
  ullong   termCount;
  double  *pOut;
  double   sum;
  double   completeness;
  ullong   reportMask = 0x000000000004ffff;

                              numShort = i_Na;  numLong = i_Nb;  vecShort  = i_a;  vecLong  = i_b;        // presume a is shorter
  if(numShort > numLong) {    numShort = i_Nb;  numLong = i_Na;  vecShort  = i_b;  vecLong  = i_a;  }     // woops,  b is shorter
  wholeCount = 0;
  termCount  = numLong - numShort + 1; // Is an interior convo.
  pOut       = o_o;
  convoLen   = numShort;
  // Now convoLen better be numShort
  // Things about the body convolution
  while(wholeCount <= termCount) {
    sum = 0.0;
    pLongHead  = &(vecLong[wholeCount + numShort - 1]);
    pShortHead =  vecShort;
    if((wholeCount & reportMask) == 0) {
      completeness = (double)wholeCount / (double)termCount;
      if(Report != NULL) Report(&completeness, ClasPtr);
    }
    for(llong dex = 0; dex < convoLen; dex++) {
      sum += *pLongHead * *pShortHead;
      pShortHead++;
      pLongHead--;
    }
    *pOut = sum;
    pOut++;
    wholeCount++;
  }
  return;
}
//==  Numbers, Integers
       ullong   GCF                   ( ullong a, ullong b) {
  ullong i;
  // According to Euclid
  while (b != 0) {
    i = a % b;
    a = b;
    b = i;
    }
  return a;
  }
       void     caes_factorize        ( ullong a, ullong *N, ullong *f) {
  ullong   dex;
  ullong   rmd;
  ullong   candidate;
  ullong   trial;
  *N = 0;

  dex = 0;
  rmd = a;
  while(rmd > 1) {
    candidate = THE_PRIMES[dex];
    trial = rmd / candidate;
    while((trial * candidate == rmd) && (rmd > 1)) {
      f[(*N)++] = candidate;
      rmd = trial;
      trial /= candidate;
      }
    dex++;
    }
  return;
  }
       ullong   ReverseBits           ( ullong  x, ullong  N ) {
  ullong  i;
  ullong  r;

  r = 0;
  for(i = 0; i < N; i++) {
    r <<= 1;
    r  |= x & 1;
    x >>= 1;
    }
  return r;
  }
//==  Numbers, Reals
       ullong   Log2                  ( ullong  N   ) {
  ullong  i;
  ullong  l;
  l = 0;
  i = 1;
  while(i < N) {
    i<<=1;
    l++;
    }
  return l;
  }
       ullong   NextPowTwo            ( ullong  i   ) {
  ullong ll;
  ullong sh;

  if(i <= 1)
    return 1;
  ll = i;
  sh = 0;
  while(ll != 0) {
    ll >>= 1;
    sh++;
  }
  ll = 1 << (sh);
  if(ll - i * 2 == 0)
    ll = i;
  return ll;
}
       bool     IsPowTwo              ( ullong  x   ) {
  if  (x < 2)
    return false;
  else if(x &(x-1))
    return false;
  else
    return true;
  }
       ldouble  pow10                 ( ldouble x            ) {
  long double z;
  z = powl((long double)10.0, x);
  return z;
}
//==  Stats
       double   caes_RandRect         ( void        ) {
  double r;
  long long ii;
  ii  = rand();
  ii -= RAND_RECT_OFF;
  r = (double)ii * RAND_RECT_FACTOR;
  return r;
  }
       void     caes_RandGaussVec     ( ullong N, double **v) {
  double *pV;

  pV = *v;
  for(uint i=0; i<N;  i++)
    *(pV++)  = rand();
  for(uint j=0; j<11; j++) { // MAGICK we just know to do the 12 thing.
    pV = *v;
    for(uint i=0; i<N;  i++)
      *(pV++) += rand();
    }
  pV = *v;
  for(uint i=0; i<N;  i++)
    *(pV++) -= RAND_GAUSS_OFF;
  pV = *v;
  for(uint i=0; i<N;  i++)
    *(pV++) *= RAND_GAUSS_FACTOR;
  return;
  }
       double   caes_RandGauss        ( void        ) {
  double en;
  llong  ii;

  en = 0.0;
  ii = 0;
  for(uint i=0; i<12; i++)
    ii += rand();
  ii -= RAND_GAUSS_OFF;
  en = (double)ii;
  en *= RAND_GAUSS_FACTOR;
  return en;
  }
#undef __IN_CAEV_MATH_CPP_
