
// $Id: CaesHeiTree.cpp 145 2017-06-16 23:13:50Z duncang $

//=================================================================================================
// Original File Name : CaesHeiTree.cpp
// Original Author    : duncang
// Creation Date      : Sept 12, 2015
// Copyright          : Copyright © 2015 by Catraeus and Duncan Gray
//
//=================================================================================================

#include "CaesHeiTree.hpp"

HeiTree::HeiTree() { // Start a whole new tree
  node           = new sNode;
  node->parent   = NULL; // One and only with NULL parent
  node->child    = NULL; // True for every terminal node (leaf)
  node->prev     = NULL;
  node->next     = NULL;
  node->conts    = NULL;
  root           = node;
  }
HeiTree::~HeiTree() {
  }
void  HeiTree::Find(void *i_conts) {
  return;
  }
void HeiTree::AddPeer(void *i_conts) {
  sNode *tNode;

  while(node->next != NULL) {
    node    = node->next;
  }
  tNode = new sNode;
  tNode->parent = node->parent;
  tNode->child  = NULL;
  tNode->prev   = node;
  tNode->next   = NULL;
  node->next    = tNode;
  node          = tNode;
  return;
}
