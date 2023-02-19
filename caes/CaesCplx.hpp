
// $Id: CaesCplx.hpp 145 2017-06-16 23:13:50Z duncang $

//=================================================================================================
// Original File Name : CaesCplx.hpp
// Original Author    : duncang
// Creation Date      : Dec 8, 2012
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//    The one and only
//
//=================================================================================================

#ifndef CAEV_CPLX_HPP_
#define CAEV_CPLX_HPP_
#include <math.h>
#include "CaesMath.hpp"

class Complex {
  public:
             Complex();
             Complex(double i_a, double i_b);
             Complex(const Complex& i_c);
    virtual ~Complex();

    double a;
    double b;
    Complex operator+(const Complex& i_c);
    Complex operator-(const Complex& i_c);
    Complex operator*(const Complex& i_c);
    Complex operator/(const Complex& i_c);
    Complex operator+(const double&  i_d);
    Complex operator-(const double&  i_d);
    Complex operator*(const double&  i_d);
    Complex operator/(const double&  i_d);
    Complex operator~(void);
    Complex operator-(void);
    //void    operator=(const Complex& c);
  };
struct cplx {
    double a;
    double b;
  };
static inline cplx   cxNew (double ia,  double ib ) {
  cplx c;
  c.a = ia;
  c.b = ib;
  return c;
  }
static inline cplx   cxNeg (cplx   z              ) {
  cplx c;
  c.a = -z.a;
  c.b = -z.b;
  return c;
  }
static inline void   cxNeg (cplx*  z              ) {
  z->a = -z->a;
  z->b = -z->b;
  return;
  }
static inline cplx   cxCjg (cplx   z              ) {
  cplx c;
  c.a =  z.a;
  c.b = -z.b;
  return c;
  }
static inline void   cxCjg (cplx*  z              ) {
  z->b = -z->b;
  return;
  }
static inline cplx   cxAdd (cplx   z1,  cplx   z2) {
  cplx c;
  c.a = z1.a + z2.a;
  c.b = z1.b + z2.b;
  return c;
  }
static inline cplx   cxAdd (cplx z, double x) {
  cplx c;
  c.a = z.a + x;
  c.b = z.b;
  return c;
  }
static inline cplx   cxAdd (double x, cplx z) {
  cplx c;
  c.a = z.a + x;
  c.b = z.b;
  return c;
  }
static inline cplx   cxSub (cplx   z1,  cplx   z2 ) {
  cplx c;
  c.a = z1.a - z2.a;
  c.b = z1.b - z2.b;
  return c;
  }
static inline cplx   cxSub (cplx   z,   double x  ) {
  cplx c;
  c.a = z.a - x;
  c.b = z.b;
  return c;
  }
static inline cplx   cxSub (double x,   cplx   z  ) {
  cplx c;
  c.a = x - z.a;
  c.b =    -z.b;
  return c;
  }
static inline cplx   cxMul (cplx   a,   cplx   b  ) {
  cplx c;
  c.a = a.a * b.a - a.b * b.b;
  c.b = a.b * b.a + a.a * b.b;
  return c;
  }
static inline cplx   cxMul (double x,   cplx   z  ) {
  cplx c;
  c.a = x * z.a;
  c.b = x * z.b;
  return c;
  }
static inline cplx   cxMul (cplx   z,   double x  ) {
  cplx c;
  c.a = x * z.a;
  c.b = x * z.b;
  return c;
  }
static inline cplx   cxDiv (cplx   z1,  cplx   z2 ) {
  cplx   c;
  double r;
  double d;
  if (fabs(z2.a) >= fabs(z2.b)) { // Enhance the accuracy in cancellation.
    r   =  z2.b / z2.a;
    d   =  z2.a + r * z2.b;
    c.a = (z1.a + r * z1.b) / d;
    c.b = (z1.b - r * z1.a) / d;
    }
  else {
    r   =  z2.a / z2.b;
    d   =  z2.b + r * z2.a;
    c.a = (z1.a * r + z1.b) / d;
    c.b = (z1.b * r - z1.a) / d;
    }
  return c;
  }
static inline cplx   cxDiv (cplx   z,   double x  ) {
  cplx   c;
  c.a = z.a / x;
  c.b = z.b / x;
  return c;
  }
static inline double cxAbs (cplx   z              ) {
  double ia;
  double ib;
  double c;
  double r;

  if (z.a == 0.0) {
    return fabs(z.b);
    }
  if (z.b == 0.0) {
    return fabs(z.a);
    }
  ia = fabs(z.a);
  ib = fabs(z.b);
  if (ia > ib) { // Keep cancelation errors in Vegas.
    r = ib / ia;
    c = ia;
    }
  else {
    r = ia / ib;
    c = ib;
    }
  c *= sqrt(1.0 + r * r);
  return c;
  }
static inline cplx   cxSqrt(cplx   z              ) {
  double ia;
  double ib;
  double w;
  double r;
  cplx   c;
  if ((z.a == 0.0) && (z.b == 0.0)) {
    c.a = 0.0;
    c.b = 0.0;
    return c;
    }

  ia = fabs(z.a);
  ib = fabs(z.b);
  if (ia >= ib) {
    r = ib / ia;
    w = sqrt(ia) * sqrt(0.5 * (1.0 + sqrt(1.0 + r * r)));
    }
  else {
    r = ia / ib;
    w = sqrt(ib) * sqrt(0.5 * (r   + sqrt(1.0 + r * r)));
    }
  if (z.a >= 0.0) {
    c.a = w;
    c.b = z.b / (2.0 * w);
    }
  else {
    c.b = (z.b >= 0.0) ? w : -w;
    c.a =  z.b / (2.0 * c.b);
    }
  return c;
  }
static        cplx   cx1 = cxNew(1.0, 0.0);
static        cplx   cxj = cxNew(0.0, 1.0);

#endif /* CAEV_CPLX_HPP_ */
