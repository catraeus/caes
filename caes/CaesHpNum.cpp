
// $Id: CaesHpNum.cpp 235 2023-02-12 04:23:42Z duncang $

/*
 * HpNum.cpp
 *
 *  Created on: Nov 23, 2012
 *      Author: duncang
 */

#include "CaesHpNum.hpp"

HpNum::HpNum() {
  m0 = 0;
  m1 = 0;
  m2 = 0;
  m3 = 0;
  m0 = 0;
  e  = 0;
  }

HpNum::~HpNum() {

  }

HpNum HpNum::operator=(double id) {
  uDbl  ud;
  ud.d = id;
  m3 = ud.u;
  e  = ud.u;
  m3 &= 0x000fffffffffffff; // IEEE says 24 bits with implicit ms 1.
  m3 |= 0x0010000000000000; // So put on the implicit 1.
  m3 <<= 10;                // and left-justify
  if(ud.u & 0x8000000000000000) { // Dig out the sign bit.  We're lucky because it will never overflow.
    m3 = ~m3;  // Doing the 2's compliment thing.
    m3++; // and the lower dwords will be all zeroes.
    }
  m2 = 0x0000000000000000;
  m1 = 0x0000000000000000;
  m0 = 0x0000000000000000;
  e &= 0x7ff0000000000000; // dig out the exponent
  e >>= 52;                // put it down to integer basis
  e -= 1023;               // And undo the offset binary of it all.
  return(*this);
  }
      HpNum::operator double() {
  uDbl   ud;
  HpNum tx   = *this;
  ullong mant = tx.m3;
  llong  exp  = tx.e;
  if(mant & 0x8000000000000000)
    tx = -tx;
  mant >>= 10;
  if(tx.m3 & 0x0000000000000200) // round nearest
    mant++;
  if(mant & 0x0020000000000000) {
    mant >>= 1;
    exp++;
    }
  if(exp < -1023)
    ud.u = 0x0000000000000000;
  else if(exp > 1024) {
    if(m3 & 0x8000000000000000)
      ud.u = 0xfff0000000000000;
    else
      ud.u = 0x7ff0000000000000;
    }
  else {
    exp += 1023;
    exp <<= 52;
    if(m3 & 0x8000000000000000)
      ud.u = 0x8000000000000000;
    else
      ud.u = 0x0000000000000000;
    ud.u |= exp;
    ud.u |= mant & 0x000fffffffffffff;
    }
  return(ud.d);
  }
HpNum HpNum::operator *(HpNum iq) {
  double d1 = *this;
  double d2 = iq;
  d2 *= d1;
  *this = d2;
  return *this;
  }
HpNum HpNum::operator-(void) {

  m0 = ~m0;
  m0++;
  m1 = ~m1;
  if(m0 == 0x0000000000000000) {
    m1++;
    m2 = ~m2;
    if(m1 == 0x0000000000000000) {
      m2++;
      m3 = ~m3;
      if(m2 == 0x0000000000000000)
        m3++;
      }
    else
      m3 = ~m3;
    }
  else {
    m2 = ~m2;
    m3 = ~m3;
    }
  return *this;
  }
