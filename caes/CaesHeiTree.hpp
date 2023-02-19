
// $Id: CaesHeiTree.hpp 145 2017-06-16 23:13:50Z duncang $

//=================================================================================================
// Original File Name : CaesHeiTree.hpp
// Original Author    : duncang
// Creation Date      : Sept 12, 2015
// Copyright          : Copyright © 2015 by Catraeus and Duncan Gray
//
// Description        :
/*
    This is a hierarchical tree.  Add, move, prune ... lots of stuff.
    Scary thing for future design --- threadsafety.  Some static methods that lock the db and queue up requests.
    The user has a pointer to an instance.  The instance is a member of a tree.
    Metadata about the whole tree is:
      Depth    ... many levels from the top to the lowest bottom node.
      Size     ... Simple total of all nodes.
      Root     ... The top of the tree, one and only.  There can be no peers of the root.

    Metadata about any single node.
      Up     ... to the one and only parent.
      Down   ... to the first child.  There can be a lot of children.
      Pred   ... Predecessor, a peer on a parent's child list.
      Succ   ... Successor, a peer on a parent's child list.
      Depth  ... How far from Head.
      children ... an array of HeiTree *
      isLeaf ... children is empty.

    Things you can do:
      Add a node ... only from the one you're currently holding.  But where?
        Add child.      Look at the child list, go to the end and add a new one after the last.
        Add sibling.    Really append, go to the end of the sibling list via your parent.
        Insert sibling. From the node that the user is holding, insert ... needs a before version and an after version.
      Delete a node.
        First   ... you can delete a node and join up its children - slice
        Second  ... you can delete a node and its children - prune
        Non-Ortho ... For a leaf node, the two are isomorphs.
        Biggest problem ... don't "delete this;"  therefore the parent has to do the job.
        If childless, or pruning, then you just pull together the pred/succ nodes.
        else pull the child list up and insert where plucked node is were.
        Another BIG problem is the void * conts object ...  I see two solutions ...
          1. make a list of deleted nodes'conts and give to tree-owning overlords to delete the coerced back objects.
                This is a list of addresses, so comparison is easy and the check for a registry of deleted objects becomes easy.
                This makes some more involvement between the overlord and the HeiTree.
          2. Make a callback to register a delete function (or any other handler.)
                This abstracts the problem, but makes the callback have to do a good dance to never delete twice.
          This is the smart pointer problem, reference counting et al, in miniature.
      Create a whole new tree from a node of another tree.
        1. By clone, leaving the old in place, producing a new pile in another place ... with the same pointers to conts!
        2. By prune, I'll call this "TakeCutting"
      Put a whole 'nother tree onto a location of an existing tree. "Graft()"
      ReRoot.  Take a node and turn it into the root.  A fancy exercise without much reward, but what do I know.
      Find where a given void * is located.  Might be a list.

    Safety and cleanliness.
      Prevent directed cycles ... infinite loops.
      Prevent banyan vines.  Non infinite ... might be impossible since a node will have only one parent.

*/
//
//=================================================================================================

#ifndef __CAEV_HEITREE_HPP_
#define __CAEV_HEITREE_HPP_

class HeiTree {
  private:
    enum eConst {
      MAX_SIZE = 32768
      };
    struct sTreeMeta {
      HeiTree  *head;
      long      depth;
      HeiTree **line;
      long      size;
      };
  public:
                     HeiTree (             );
                     HeiTree (void *       );
    virtual         ~HeiTree (             );
            void    *Get     (void         ) {return conts;};
            HeiTree *Find    (void *i_conts);
            HeiTree *Append  (void *i_conts);
            HeiTree *Hang    (void *i_conts);
            void     Attach  (void *i_conts) {conts = i_conts; return;}
            void     Klunk   (void) {delete this; return;};
  private:
                     HeiTree (HeiTree *i_HT);
  private:
    HeiTree         *up;
    HeiTree         *dn;
    HeiTree         *pred;
    HeiTree         *succ;
    void            *conts;
    int              index;
    int              depth;
    sTreeMeta       *meta;
  };

#endif // __CAEV_HEITREE_HPP_
