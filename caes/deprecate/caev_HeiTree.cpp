
// $Id: caev_HeiTree.cpp 142 2017-06-02 01:43:40Z duncang $

//=================================================================================================
// Original File Name : caev_HeiTree.cpp
// Original Author    : duncang
// Creation Date      : Sept 12, 2015
// Copyright          : Copyright © 2015 by Catraeus and Duncan Gray
//
//=================================================================================================


#include "caev_HeiTree.hpp"

HeiTree::HeiTree()
: up     (0)
, dn     (0)
, pred   (0)
, succ   (0)
, conts  (0)
, index  (0)
, depth  (0)
, meta   (0) { // Start a whole new tree
  meta          = new sTreeMeta;
  meta->head    = this;
  meta->line    = new HeiTree*[MAX_SIZE];
  meta->size    = 0;
  index         = 0;
  depth         = 1;
  meta->line[0] = this;
  meta->depth   = 1;
  meta->size    = 1;
  }
HeiTree::HeiTree(HeiTree *i_HT)
: up     (0)
, dn     (0)
, pred   (0)
, succ   (0)
, conts  (0)
, index  (0)
, depth  (0)
, meta   (0) { // Become part of an existing tree
  meta = i_HT->meta; // don't new since this is part of an existing tree
  // The current location doesn't move to this
  // we have no clue in the depth.  The append or hang functions will take care of this.
  meta->line[meta->size] = this; // We do know that the straight line will grow
  index = meta->size;
  meta->size++; // ibid.
  }
HeiTree::~HeiTree() {
  }
HeiTree *HeiTree::Find(void *i_conts) {
  HeiTree *tHT;

  tHT = meta->head;
  while((tHT->conts != i_conts) && (tHT != 0)) { // not even close to a complete tree walk
    if(tHT->succ != 0) {
      tHT = tHT->succ;
      }
    else{
      if(tHT->dn != 0) {
        tHT = tHT->dn;
        }
      else {
        tHT = tHT->up;
        }
      }
    }
  if(tHT->conts != i_conts)
    return 0;
  else
    return  tHT;
  }
HeiTree *HeiTree::Append(void *i_conts) {
  HeiTree *tHT;

  tHT = this;
  while(tHT->succ != 0)
    tHT = tHT->succ;
  tHT->succ        = new HeiTree(tHT);
  tHT->succ->pred = tHT;
  tHT->succ->up   = up;
  tHT->succ->dn   = 0;
  tHT->succ->conts = i_conts;
  return tHT->succ;
  }
