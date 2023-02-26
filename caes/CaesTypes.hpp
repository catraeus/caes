

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
typedef unsigned             int    uint;
typedef unsigned      long   int    ulong;
typedef          long long   int    llong;
typedef unsigned long long   int    ullong;
typedef signed        short  word;
typedef unsigned      short  field;
typedef long          double ldouble;

static const double PI=3.14159265358979323846264338327950288419716939937510;
static const double EMGAMMA=0.57721566490153286060651209008240243104215933593992;
static const double E=2.71828182845904523536028747135266249775724709369995;
static const double SQ2=1.41421356237309504880168872420969807856967187537694;
static const double SQv2=0.70710678118654752440084436210484903928483593768847;

static const double PIx2=2.0 * PI;
static const double PIv2=0.5 * PI;

static const double DBL_ROUNDER=0.4999999999999999;
static const double DBL_EPS=((double)2.22044604925031308085e-16L) * (double)2.0L;// for no damned good reason but intuition.  DBL_EPSILON is defined in Config_Heavy.pl to make math dynamic libs.
static const llong MAX_PATH_LEN=32768;
static const llong MAX_NAME_LEN=256;
static const llong MAX_LINE_LEN=32768;
static const llong MAX_LIST_LEN=32768;
static const llong MAX_CFG_LINES=32768;
static const llong MIN_PATH_LEN=256;
static const llong MAX_DIR_ENTRIES=32768;


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

typedef enum eErr_e { // MAGICK  These come from basic posix /usr/include/asm-generic/errno-base.h and the extra errno types of ALSA
  CENO_PERM            =      1 ,  // Operation not permitted
  CENO_NOENT           =      2 ,  // No such file or directory
  CENO_SRCH            =      3 ,  // No such process
  CENO_INTR            =      4 ,  // Interrupted system call
  CENO_IO              =      5 ,  // I/O error
  CENO_NXIO            =      6 ,  // No such device or address
  CENO_2BIG            =      7 ,  // Argument list too long
  CENO_NOEXEC          =      8 ,  // Exec format error
  CENO_BADF            =      9 ,  // Bad file number
  CENO_CHILD           =     10 ,  // No child processes
  CENO_AGAIN           =     11 ,  // Try again
  CENO_NOMEM           =     12 ,  // Out of memory
  CENO_ACCES           =     13 ,  // Permission denied
  CENO_FAULT           =     14 ,  // Bad address
  CENO_NOTBLK          =     15 ,  // Block device required
  CENO_BUSY            =     16 ,  // Device or resource busy
  CENO_EXIST           =     17 ,  // File exists
  CENO_XDEV            =     18 ,  // Cross-device link
  CENO_NODEV           =     19 ,  // No such device
  CENO_NOTDIR          =     20 ,  // Not a directory
  CENO_ISDIR           =     21 ,  // Is a directory
  CENO_INVAL           =     22 ,  // Invalid argument
  CENO_NFILE           =     23 ,  // File table overflow
  CENO_MFILE           =     24 ,  // Too many open files
  CENO_NOTTY           =     25 ,  // Not a typewriter
  CENO_TXTBSY          =     26 ,  // Text file busy
  CENO_FBIG            =     27 ,  // File too large
  CENO_NOSPC           =     28 ,  // No space left on device
  CENO_SPIPE           =     29 ,  // Illegal seek
  CENO_ROFS            =     30 ,  // Read-only file system
  CENO_MLINK           =     31 ,  // Too many links
  CENO_PIPE            =     32 ,  // Broken pipe
  CENO_DOM             =     33 ,  // Math argument out of domain of func
  CENO_RANGE           =     34 ,  // Math result not representable
  //  ALSA people are assholes.
  CENO_SND_BEGIN       = 500000 ,  // Lower boundary of sound error codes.
  CENO_SND_VERSION     = 500000 ,  // Kernel/library protocols are not compatible.
  CENO_SND_ALISP       = 500001 ,  // Lisp encountered an error during a call.
} eErr;

#endif // _CAES_TYPES_HPP_
