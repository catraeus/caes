
// $Id: CaesHpNum.hpp 235 2023-02-12 04:23:42Z duncang $

//============================================================================
// Name        : qubl.hpp
// Author      : Catraeus
// Version     :
// Copyright   : Copyright © 2012 Catraeus and Duncan Gray
// Description : A class to make 256-bit mantissa floating-point numbers
//============================================================================
/*
 * HpNum.hpp
 *
 *  Created on: Nov 23, 2012
 *      Author: duncang
 */

#ifndef __CAES_HP_NUM_
#define __CAES_HP_NUM_
#include "CaesTypes.hpp"


class HpNum {
  private:
    union uDbl {
      double d;
      ullong u;
    };
  public:
                     HpNum();
    virtual         ~HpNum();
            HpNum    operator=(double id);
            HpNum    operator*(HpNum iq);
            operator double();
            HpNum    operator-(void);
            llong    mant(void) {return m3;};
  private:
    llong m0;
    llong m1;
    llong m2;
    llong m3;
    llong  e;
};

#endif /* __CAES_HP_NUM_ */
