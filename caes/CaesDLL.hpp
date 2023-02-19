
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
template <typename T>          CaesDLL<T>::CaesDLL      ( void        ) {
  numDLL    = 0;
  curr      = NULL;
  head      = curr;
  tail      = curr;
  pushStore = NULL;
  return;
}
template <typename T>          CaesDLL<T>::~CaesDLL     ( void        ) {
  return;
}
template <typename T> llong    CaesDLL<T>::Num          ( void        ) {

  numDLL = 0;
  Head();
  if(curr == NULL)
    return numDLL;
  numDLL++;
  while(!AtTail()) {
    numDLL++;
    Next();
  }
  return numDLL;
}
template <typename T> T        CaesDLL<T>::Head         ( void        ) {
  curr = head;
  return curr == NULL ? NULL : curr->d;
}
template <typename T> T        CaesDLL<T>::Prev         ( void        ) {
  if(curr->prev != NULL)
    curr = curr->prev;
  return curr == NULL ? NULL : curr->d;
}
template <typename T> T        CaesDLL<T>::Curr         ( void        ) {
  return curr == NULL ? NULL : curr->d;
}
template <typename T> T        CaesDLL<T>::Next         ( void        ) {
  if(curr->next != NULL)
    curr = curr->next;
  return curr == NULL ? NULL : curr->d;
}
template <typename T> T        CaesDLL<T>::Tail         ( void        ) {
  curr = tail;
  return curr == NULL ? NULL : curr->d;
}
template <typename T> bool     CaesDLL<T>::AtHead       ( void        ) {
  if(curr == NULL) return true;
  return(curr == head);
}
template <typename T> bool     CaesDLL<T>::AtTail       ( void        ) {
  bool tailTest;
  if(curr == NULL)
    tailTest = true;
  else
    tailTest = (curr == tail);
  return tailTest;
}
template <typename T> bool     CaesDLL<T>::IsEmpty      ( void        ) {
  return(curr == NULL);
}
template <typename T> void     CaesDLL<T>::Prepend      ( T       i_d ) {
  Head();
  InsBefore(i_d);
}
template <typename T> void     CaesDLL<T>::Append       ( T       i_d ) {
  Tail();
  InsAfter(i_d);
}
template <typename T> void     CaesDLL<T>::InsBefore    ( T       i_d ) {
  node *pred;
  node *succ;
  T    *td;
  if(curr == NULL) { // handle empty DLL
    curr = new node;
    curr->prev = NULL;
    curr->next = NULL;
    head = curr;
    tail = curr;
  }
  else {
    pred = curr->prev;
    succ = curr;
    curr = new node;
    curr->prev = pred;
    curr->next = succ;
    pred->next = curr;
    succ->prev = curr;
    if(pred == NULL)
      head = curr;
  }
  curr->d = i_d;
  return curr->d;
}
template <typename T> void     CaesDLL<T>::InsAfter     ( T       i_d ) {
  node *pred;
  node *succ;
  if(curr == NULL) { // handle empty DLL
    curr = new node;
    curr->prev = NULL;
    curr->next = NULL;
    head = curr;
    tail = curr;
  }
  else {
    pred = curr;
    succ = curr->next;
    curr = new node;
    curr->prev = pred;
    curr->next = succ;
    if(pred!= NULL)
      pred->next = curr;
    if(succ != NULL)
      succ->prev = curr;
    else
      tail = curr;
  }
  curr->d = i_d;
  return;
}
template <typename T> T        CaesDLL<T>::Delete       ( void        ) {
  node *pred;
  node *succ;
  if(curr == NULL)  // handle empty DLL
    return NULL;
  pred = curr->prev;
  succ = curr->next;
  if((pred == NULL) && (succ == NULL)) { // this is the only node
    delete curr;
    curr = NULL;
    head = NULL;
    tail = NULL;
  }
  else if(pred == NULL) { // This is the old head
    delete curr;
    curr = succ;
    curr->prev = NULL;
    head = curr;
  }
  else if(succ == NULL) { // Delete the old tail
    delete curr;
    curr = pred;
    curr->next = NULL;
    tail = curr;
  }
  else { // It is a mid-list delete
    delete curr;
    curr = pred; // We squeeze downward, maybe needs expansion
    if(curr->prev == NULL) // Check that we might have been next to head
      head = curr;
    else if(curr->next == NULL) // Check that we might have been next to tail
      tail = curr;
  }
  return curr == NULL ? NULL : curr->d;
}
template <typename T> T        CaesDLL<T>::Find         ( T       i_d ) {

}
template <typename T> llong    CaesDLL<T>::Count        ( T       i_d ) {
  llong k;
  k = 0;
  if(IsEmpty())
    return k;
  Head();
  k++;
  while(!AtTail()) {
    Next();
    k++;
  }
  return k;
}
template <typename T> void     CaesDLL<T>::Push1        ( void        ) {
  if(pushStore != NULL)
    return;
  pushStore = curr;
  return;
}
template <typename T> void     CaesDLL<T>::Pop1         ( void        ) {
  if(pushStore != NULL)
    return;
  curr = pushStore;
  pushStore = NULL;
  return;
}

#endif // __CAES_DLL_HPP_
