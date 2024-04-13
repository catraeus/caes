
// $Id: CaesDLL.hpp 204 2022-11-21 04:29:00Z duncang $

//=================================================================================================
// Original File Name : caes_stat.h
// Original Author    : duncang
// Created on         : Oct 4, 2013
// Copyright          : Copyright © 2013 by Catraeus and Duncan Gray
// Description        : Vector functions of statistics.
//=================================================================================================

#ifndef __CAES_DLL_HPP_
#define __CAES_DLL_HPP_

#include <stdlib.h>

#include <caes/CaesTypes.hpp>

template <typename T> class CaesDLL {
  public:
  private:
    struct node {
      T      d;
      node  *next;
      node  *prev;
    };
  public:
             CaesDLL   ();
    virtual ~CaesDLL   ();
    llong    Num       ();
    T        Head      ();
    T        Prev      ();
    T        Curr      ();
    T        Next      ();
    T        Tail      ();
    bool     AtHead    ();
    bool     AtTail    ();
    bool     IsEmpty   ();
    void     Prepend   (T i_d);
    void     Append    (T i_d);
    void     InsBefore (T i_d);
    void     InsAfter  (T i_d);
    T        Delete    (     );
    T        Find      (T i_d);
    llong    Count     (T i_d);
    void     Push1     (void );
    void     Pop1      (void );
  public:
  private:
    llong    numDLL;
    node    *head;
    node    *tail;
    node    *curr;
    node    *pushStore;
  };

#endif // __CAES_DLL_HPP_
