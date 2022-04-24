

//=================================================================================================
// Original File Name : CaesTypes.hpp
// Original Author    : catraeus
// Creation Date      : Apr 17, 2022
// Copyright          : Copyright © 2022 by Catraeus and Duncan Gray
//
// Description        :
/*
   I do like short type names.
*/
//
//=================================================================================================
/*
This file is part of Catraeus Libraries.

Catraeus Libraries is free software: you can redistribute it and/or modify it under the terms of
  the GNU Lesser General Public License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

Catraeus Libraries is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
  the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with dmmsnoop.
  If not, see <https://www.gnu.org/licenses/>.
*/
//=================================================================================================

#ifndef _CAES_TYPES_HPP_
#define _CAES_TYPES_HPP_


typedef unsigned      char   uchar;
typedef unsigned      char   ubyte;
typedef unsigned      char   byte;
typedef signed        char   sbyte;
typedef unsigned      short  ushort;
typedef unsigned      int    uint;
typedef unsigned      long   ulong;
typedef          long long   llong;
typedef unsigned long long   ullong;
typedef signed        short  word;
typedef unsigned      short  field;
typedef long          double ldouble;

static const double  PI              = 3.14159265358979323846264338327950288419716939937510;
static const double  EMGAMMA         = 0.57721566490153286060651209008240243104215933593992;
static const double  E               = 2.71828182845904523536028747135266249775724709369995;
static const double  SQ2             = 1.41421356237309504880168872420969807856967187537694;
static const double  SQv2            = 0.70710678118654752440084436210484903928483593768847;

static const double  PIx2            = 2.0 * PI;
static const double  PIv2            = 0.5 * PI;

static const double DBL_ROUNDER      = 0.4999999999999999;
static const double DBL_EPS          = ((double)2.22044604925031308085e-16L) * (double)2.0L; // for no damned good reason but intuition.  DBL_EPSILON is defined in Config_Heavy.pl to make math dynamic libs.
static const llong  MAX_PATH_LEN     = 32768;
static const llong  MAX_NAME_LEN     =   128;
static const llong  MAX_LINE_LEN     = 32768;
static const llong  MAX_LIST_LEN     = 32768;
static const llong  MAX_CFG_LINES    = 32768;
static const llong  MIN_PATH_LEN     =   256;
static const llong  MAX_DIR_ENTRIES  = 32768;


#if __BYTE_ORDER == __LITTLE_ENDIAN
#define PACK_08_TO_32(a,b,c,d)  ((a) | ((b)<<8) | ((c)<<16) | ((d)<<24))
#define LE_16(v)         (v)
#define LE_32(v)         (v)
#define BE_16(v) bswap_16(v)
#define BE_32(v) bswap_32(v)
#elif __BYTE_ORDER == __BIG_ENDIAN
#define PACK_08_TO_32(a,b,c,d)  ((d) | ((c)<<8) | ((b)<<16) | ((a)<<24))
#define LE_16(v) bswap_16(v)
#define LE_32(v) bswap_32(v)
#define BE_16(v)         (v)
#define BE_32(v)         (v)
#else
  //error("Wrong endian");
#endif


#endif // _CAES_TYPES_HPP_
