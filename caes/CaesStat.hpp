
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
             Stat           ( void      );
    virtual ~Stat           ( void      );
    void     SetVec         ( llong   i_N, double  *i_vec);
    double  *GetVec         ( void      )  {return vec;};
    llong    GetN           ( void      )  {return N;  };
    void     CalcNormal     ( void      );
    void     CalcRr         ( Stat *i_st);
    bool     CalcTaylor3    ( double *o_t0, double *o_t1, double *o_t2);
    void     RunPearson     ( llong   i_x, llong i_y);
    void     CalcLR         ( void      );
    double   GetM           ( void      )  {return m          ; };
    double   GetS           ( void      )  {return s          ; };
    double   GetMin         ( void      )  {return min        ; };
    llong    GetMinOff      ( void      )  {return minOff     ; };
    double   GetMax         ( void      )  {return max        ; };
    llong    GetMaxOff      ( void      )  {return maxOff     ; };
    double   GetRr          ( void      )  {return rr         ; };
    bool     GetValid       ( void      )  {return valid      ; };
  private:
    void     CalcM          ( void      );
    void     CalcS          ( void      );
    void     CalcMin        ( void      );
    void     CalcMax        ( void      );
  public:
  private:
    llong    N;
    double  *vec;

    double   m;
    double   s;
    double   a;
    double   b;
    double   g;
    double   t2;
    double   t1;
    double   t0;
    double   min;
    llong    minOff;
    double   max;
    llong    maxOff;
    double   rr;

    bool     valid;
    bool     dirty;
  };

#endif // __CAES_STAT_HPP_
