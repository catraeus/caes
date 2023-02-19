// $Id: CaesFunc.cpp 202 2022-11-19 21:53:43Z duncang $

//=================================================================================================
// Original File Name : CaesSpectrum.cpp
// Original Author    : duncang
// Creation Date      : Dec 9, 2012
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//    The one and only
//
//=================================================================================================

#include <math.h>

#include <caes/CaesTypes.hpp>

#include <caes/CaesFunc.hpp>

double Si_by_Series(double i_x) {return PIv2;}
double CiSi_by_ExpInt(double i_x) {return PIv2;}
double Tk(long k, double x) {
  if(fabs(x)<=1.0)
    return(cos((double)k*acos(x)));
  else
    return(cosh((double)k*acosh(x)));
  }
