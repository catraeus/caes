// $Id: CaesCplx.cpp 145 2017-06-16 23:13:50Z duncang $

//=================================================================================================
// Original File Name : CaesCplx.cpp
// Original Author    : duncang
// Creation Date      : Dec 8, 2012
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//    The one and only
//
//=================================================================================================

#include "CaesCplx.hpp"
#include <math.h>

    Complex::Complex(void)                   : a(0.0),   b(0.0)   {}
    Complex::Complex(double i_a, double i_b) : a(i_a),   b(i_b)   {}
    Complex::Complex(const Complex& c)     : a(c.a), b(c.b) {}
    Complex::~Complex() {}
Complex Complex::operator+(const Complex&  c) { // working
  Complex cc(a + c.a, b + c.b);
  return cc;
  }
Complex Complex::operator-(const Complex&  c) { // working
  Complex cc(a - c.a, b - c.b);
  return cc;
  }
Complex Complex::operator*(const Complex&  c) {
  Complex cc(c.a * a - c.b * b, c.a * b + c.b * a);
  return cc;
  }
Complex Complex::operator/(const Complex&  c) {
  double dn = sqrt(c.a * c.a + c.b * c.b);
  Complex cc((a * c.a + b * c.b) / dn, (b * c.a - a * c.b) / dn);
  return cc;
  }
Complex Complex::operator+(const double&  d) {
  Complex cc(a + d, b);
  return cc;
  }
Complex Complex::operator-(const double&  d) {
  Complex cc(a - d, b);
  return cc;
  }
Complex Complex::operator*(const double&  d) {
  Complex cc(a * d, b * d);
  return cc;
  }
Complex Complex::operator/(const double&  d) {
  Complex cc(a / d, b / d);
  return cc;
  }

Complex Complex::operator~(void) {
  Complex cc(a, b * -1.0);
  return cc;
  }
Complex Complex::operator-(void) { // working
  Complex cc(*this);
  cc.a *= -1.0;
  cc.b *= -1.0;
  return cc;
  }
