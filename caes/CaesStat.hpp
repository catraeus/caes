
// $Id: CaesStat.hpp 202 2022-11-19 21:53:43Z duncang $

//=================================================================================================
// Original File Name : caes_stat.h
// Original Author    : duncang
// Created on         : Oct 4, 2013
// Copyright          : Copyright © 2013 by Catraeus and Duncan Gray
// Description        : Vector functions of statistics.
//=================================================================================================

#ifndef __CAES_STAT_HPP_
#define __CAES_STAT_HPP_
#include <caes/CaesTypes.hpp>

class Stat {
  public:
  private:
    enum eConst {
      ECS_MAX_CH = 8
    };
  public:
             Stat           ( llong   i_N, double **i_ary, llong i_ch);
             Stat           ( void      );
    virtual ~Stat           ( void      );
    void     SetVec         ( llong   i_N, double **i_ary, llong i_ch);
    void     Calc           ( void      );
    void     RunPearson     ( llong   i_x, llong i_y);
    void     CalcLR         ( void      );
    double   GetM           ( llong   c )  {return m   [c] ; };
    double   GetS           ( llong   c )  {return s   [c] ; };
    double   GetMin         ( llong   c )  {return min    [c] ; };
    llong    GetMinOff      ( llong   c )  {return minOff [c] ; };
    double   GetMax         ( llong   c )  {return max    [c] ; };
    llong    GetMaxOff      ( llong   c )  {return maxOff [c] ; };
    double   GetRr          ( void      )  {return rr         ; };
    bool     GetValid       ( void      )  {return valid;       };
  private:
    void     CalcM          ( void      );
    void     CalcS          ( void      );
    void     CalcMin        ( void      );
    void     CalcMax        ( void      );
    void     CalcRr         ( void      );
  public:
  private:
    llong    N;
    double **ary;
    llong    ch;

    double  *m;
    double  *s;
    double  *a;
    double  *b;
    double  *g;
    double  *min;
    llong   *minOff;
    double  *max;
    llong   *maxOff;
    double   rr;

    bool     valid;
    bool     dirty;
  };

#endif // __CAES_STAT_HPP_
