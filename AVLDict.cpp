#ifndef _AVLDICT_CPP
#define _AVLDICT_CPP

//AVLDict.cpp
#include "AVLDict.hpp"
#include <cassert>
#include <cstdlib>//for NULL
#include <iostream>


#include <sstream>
#include <iomanip> // provides setw()
#include <cstdlib> // provides atoi() abs()
#include <cassert> // provides assert()
#include <vector> 
#include <string> 

// An implementation of a dictionary ADT as an unsorted linked list.
//
AVLDict::AVLDict() {
  root = NULL;

  // Initialize array of counters for depth statistics
  depth_stats = new int[MAX_STATS]();
}

void AVLDict::destructor_helper(node * x) {
  if (x == NULL) return; // Nothing to destroy.
  destructor_helper(x->left);
  destructor_helper(x->right);
  delete x->key;
  delete x;
}

AVLDict::~AVLDict() {
  // Clean up the tree.
  // This is most easily done recursively.
  destructor_helper(root);

  // It's not good style to put this into a destructor,
  // but it's convenient for this assignment...
  cout << "Depth Statistics for find():\n";
  for (int i=0; i<MAX_STATS; i++) 
    cout << i << ": " << depth_stats[i] << endl;

  // Code for calculating average depth
  // int total = 0;
  // int samples = 0;
  // for (int i=0; i<MAX_STATS; i++) { 
  //   samples += depth_stats[i];
  //   for (int j = 0; j < depth_stats[i]; j++)
  //     total += i;
  // }
  // cout << "Average Depth: " << total/samples << endl;  
  delete [] depth_stats;
}

bool AVLDict::find_helper(node *r, string keyID, PuzzleState *&pred, int depth) {
  if (r==NULL) {
    if (depth<MAX_STATS) depth_stats[depth]++;
    return false; // Didn't find it.
  }
  if (keyID == r->keyID) {
    if (depth<MAX_STATS) depth_stats[depth]++;
    pred = r->data; // Got it!  Get the result.
    return true;
  }
  if (keyID < r->keyID) return find_helper(r->left, keyID, pred, depth+1);
  else return find_helper(r->right, keyID, pred, depth+1);
}

bool AVLDict::find(PuzzleState *key, PuzzleState *&pred) {
  return find_helper(root, key->getUniqId(), pred, 0);
}

bool AVLDict::update_height( node * x ) {
  //
  // Recalculates the height of x from the height of its children.
  // Returns true iff the height of x changes.
  //
  // COMPLETED
  if( x == NULL ) return false;
  int m = std::max(height(x->left), height(x->right)) + 1;
  if( x->height != m ) {
    x->height = m;
    return true;
  }
  return false;
}

void AVLDict::rotate_left( node *& a ) {
  // "rotates" the subtree rooted at a to the left (counter-clockwise)

// 221 Students:  DO NOT CHANGE OR DELETE THE NEXT FEW LINES!!!
// We will use this code when marking to be able to watch what
// your program is doing, so if you change things, we'll mark it wrong.
#ifdef MARKING_TRACE
std::cout << "Rotate Left: " << a->keyID << std::endl;
#endif
// End of "DO NOT CHANGE" Block

  // NOW COMPLETE
  // --------
  std::cout << "Rotate Left: " << a->keyID << std::endl;
  // --------  
  node * temp = a->right;
  a->right = temp->left;
  temp->left = a;
  update_height(a);
  update_height(temp);
  a = temp;
}

void AVLDict::rotate_right( node *& b ) {
  // "rotates" the subtree rooted at b to the right (clockwise)

// 221 Students:  DO NOT CHANGE OR DELETE THE NEXT FEW LINES!!!
// We will use this code when marking to be able to watch what
// your program is doing, so if you change things, we'll mark it wrong.
#ifdef MARKING_TRACE
cout << "Rotate Right: " << b->keyID << endl;
#endif
// End of "DO NOT CHANGE" Block

  // NOW COMPLETE
  // --------
  cout << "Rotate Right: " << b->keyID << endl;
  // --------
  node * temp = b->left;
  b->left = temp->right;
  temp->right = b;
  update_height(b);
  update_height(temp);
  b = temp;
}


// You may assume that no duplicate PuzzleState is ever added.
void AVLDict::add(PuzzleState *key, PuzzleState *pred) {
  // NOW COMPLETE

  // avoid duplicates
  if (find(key, pred))
    return;

  node * temp = new node();
  temp->key = key;
  temp->keyID = key->getUniqId();
  temp->data = pred;
  temp->left = NULL;
  temp->right = NULL;

  if( !root ) {
    root = temp;
    return;
  }

  node * current = root;

  bool leftSide;

  while (true) { 
    if( temp->keyID < current->keyID ) {
      if (current->left)
        current = current->left;
      else {
        current->left = temp;
        break;
      }
    }
    else if( temp->keyID > current->keyID ) {                     
      if (current->right)
        current = current->right;
      else {
        current->right = temp;
        break;
      }
    }

  }

  // We know: descendant had a child added (in recursive call, above).
  // Now we're "unwinding" the call-stack (returning from the recursive calls, 
  // one level at a time, until we get to the original call). 
  // Do we have to rebalance at this level?
  if( update_height(temp) )  { 
    balance(temp);
  }
}

  //
  // Check if node x is unbalanced (i.e., the heights of its
  // two children differ by more than one).  If it is, rebalance
  // at x using one of rotateLeft, rotateRight, doubleRotateLeft,
  // or doubleRotateRight, whichever is appropriate.
  //
void AVLDict::balance( node *& x ) {
  // NOW COMPLETE
  // compute left and right heights, single rotation in respective direction if zig-zig, 
  // else double rotation in respective direction if zig-zag
  // since we are balancing with each insertion, balance is in range [-2, 2]
  // is balanced if root's balance is in range [-1,1]
  // check for zig-zag or zig-zig case based on -1 or 1 balance for the root's child

  if (height(x->left) - height(x->right) == 2) { // left case
    if (height(x->left->left) - height(x->left->right) == -1) // zig-zag case
      doubleRotateRight(x);
    else if (height(x->left->left) - height(x->left->right) == 1) // zig-zig case
      rotate_right(x);
  }else if (height(x->left) - height(x->right) == -2) { // right case
    if (height(x->right->left) - height(x->right->right) == 1) // zig-zag case
      doubleRotateLeft(x);
    else if (height(x->right->left) - height(x->right->right) == -1) // zig-zig case
      rotate_left(x);
  }
}

void AVLDict::doubleRotateLeft( node *& a ) {
  // NOW COMPLETE
  // even if assert() fails, the errors get reported  
  assert(a != NULL); // not wrapped in else-body (so: no doubt it happens)
  // --------
  std::cout << "doubleRotateLeft: a->key = " << a->key << std::endl; 
  // --------
  rotate_right(a->right);
  rotate_left(a);
}

void AVLDict::doubleRotateRight( node *& a ) {
  // NOW COMPLETE
  assert(a != NULL);
  // --------
  std::cout << "doubleRotateRight: a->key = " << a->key << std::endl; 
  // --------
  rotate_left(a->left);
  rotate_right(a);
}


#endif 
