#ifndef _DOUBLEHASHDICT_CPP
#define _DOUBLEHASHDICT_CPP

//DoubleHashDict.cpp
#include "DoubleHashDict.hpp"
#include <cassert>
#include <cstdlib>//for NULL
#include <iostream>

// An implementation of a dictionary ADT as hash table with double hashing
//

const int DoubleHashDict::primes[] = {53, 97, 193, 389, 769, 1543, 3079,
      6151, 12289, 24593, 49157, 98317, 196613, 393241, 786433, 1572869,
      3145739, 6291469, 12582917, 25165843, 50331653, 100663319,
      201326611, 402653189, 805306457, 1610612741, -1};
// List of good primes for hash table sizes from
// http://planetmath.org/goodhashtableprimes
// The -1 at the end is to guarantee an immediate crash if we run off
// the end of the array.
#define LARGEST_SIZE_INDEX 26

DoubleHashDict::DoubleHashDict() {
  size_index = 0;
  size = primes[size_index];
  table = new bucket[size](); // Parentheses force initialization to 0
  number = 0;

  // Initialize the array of counters for probe statistics
  probes_stats = new int[MAX_STATS]();
}

DoubleHashDict::~DoubleHashDict() {
  // Delete all table entries...
  for (int i=0; i<size; i++) {
    if (table[i].key!=NULL) {
      delete table[i].key;
      // Don't delete data here, to avoid double deletions.
    }
  }
  // Delete the table itself
  delete [] table;

  // It's not good style to put this into a destructor,
  // but it's convenient for this assignment...
  cout << "Probe Statistics for find():\n";
  for (int i=0; i<MAX_STATS; i++)
    cout << i << ": " << probes_stats[i] << endl;
  delete [] probes_stats;
}

int DoubleHashDict::hash1(string keyID) {
  int h=0;
  for (int i=keyID.length()-1; i>=0; i--) {
    h = (keyID[i] + 31*h) % size;
  }
// 221 Students:  DO NOT CHANGE OR DELETE THE NEXT FEW LINES!!!
// We will use this code when marking to be able to watch what
// your program is doing, so if you change things, we'll mark it wrong.
#ifdef MARKING_TRACE
std::cout << "Hash 1:  " << keyID << " to " << h << std::endl;
#endif
// End of "DO NOT CHANGE" Block
  return h;
}

int DoubleHashDict::hash2(string keyID) {
  int h=0;
  for (int i=keyID.length()-1; i>=0; i--) {
    h = (keyID[i] + 63*h) % size;
  }
  // Make sure second hash is never 0 or size
  h = h/2 + 1;

// 221 Students:  DO NOT CHANGE OR DELETE THE NEXT FEW LINES!!!
// We will use this code when marking to be able to watch what
// your program is doing, so if you change things, we'll mark it wrong.
#ifdef MARKING_TRACE
std::cout << "Hash 2:  " << keyID << " to " << h << std::endl;
#endif
// End of "DO NOT CHANGE" Block
  return h;
}

void DoubleHashDict::rehash() {
// 221 Students:  DO NOT CHANGE OR DELETE THE NEXT FEW LINES!!!
// And leave this at the beginning of the rehash() function.
// We will use this code when marking to be able to watch what
// your program is doing, so if you change things, we'll mark it wrong.
#ifdef MARKING_TRACE
std::cout << "*** REHASHING " << size;
#endif
// End of "DO NOT CHANGE" Block


  // copy everything from previous table into new table of next prime size
  // increment size_index, if size_index reached max size, don't resize anymore
  int oldsize = size;
  // printf("size index %d\n", size_index);

  if (++size_index == LARGEST_SIZE_INDEX)
    return;
  
  size = primes[size_index];
  
  // point to the old table, need to be a new copy
  bucket* oldtable = new bucket[oldsize]();

  int i;
  // are these actually new values, or pointer to the old table?
  for (i = 0; i < oldsize; i++) {
    oldtable[i].key = table[i].key;
    oldtable[i].data = table[i].data;
    oldtable[i].keyID = table[i].keyID;
  }

  // table points to entirely new array of buckets new size
  // here we just zeroed the old array!, MISTAKE!!
  table = new bucket[size]();

  // add back the old values to the new table
  for (i = 0; i < oldsize; i++) {
    if (oldtable[i].key != NULL)
      add(oldtable[i].key, oldtable[i].data);
    else
      table[i].key = NULL;
  }

  /*
   //For debugging purposes
  int j;
  for (j = 0; j < oldsize; j++) {
    std::cout << "j: " << j << "keyID: " << oldtable[j].keyID << std::endl;
  }


   //For debugging purposes
  // int j;
  for (j = 0; j < size; j++) {
    std::cout << "k: " << j << "keyID: " << table[j].keyID << std::endl;
  }
  */

  // delete the old table
  delete [] oldtable;



// 221 Students:  DO NOT CHANGE OR DELETE THE NEXT FEW LINES!!!
// And leave this at the end of the rehash() function.
// We will use this code when marking to be able to watch what
// your program is doing, so if you change things, we'll mark it wrong.
#ifdef MARKING_TRACE
std::cout << " to " << size << " ***\n";
#endif
// End of "DO NOT CHANGE" Block
}

bool DoubleHashDict::find(PuzzleState *key, PuzzleState *&pred) {
  // Returns true iff the key is found.
  // Returns the associated value in pred

  // Be sure not to keep calling getUniqId() over and over again!

  // TODO:  Your code goes here...
  // use key to find slot, then compare the pred
  string findID = key->getUniqId();
  int i = 1;
  int hash_fn1 = hash1(findID);
  int hash_fn2 = hash2(findID);
  int doubleHash = i * hash_fn2;
  int searchIndex = hash_fn1 % size;

  // while not found, keep searching
  while (table[searchIndex].keyID.compare(findID) != 0) {
    probes_stats[i]++;
    doubleHash = (++i) * hash_fn2;
    searchIndex = (hash_fn1 + doubleHash) % size;
    // problem is rehash isn't actually changing size
    // printf("size: %d\n", size);
    // printf("i: %d\n", i);
    // printf("number: %d\n", number);
    if (i >= MAX_STATS) {
      // WE MUST SET PRED TO NULL SINCE WE DIDNT FIND IT
      pred = NULL;
      return false; // could not find it
    }
  }

  // found it
  probes_stats[i]++;
  pred = table[searchIndex].data; // Got it!  Get the result.
  return true; 
}

// You may assume that no duplicate PuzzleState is ever added.
void DoubleHashDict::add(PuzzleState *key, PuzzleState *pred) {

  string uniqId = key->getUniqId();

  double loadfactor = (double)number/(double)size;
  // printf("load factor: %f\n", loadfactor);

  if (loadfactor > 0.5) {
    rehash();
  }

  int i = 1;
  int hash_fn1 = hash1(uniqId);
  int hash_fn2 = hash2(uniqId);
  int entry = hash_fn1 % size;
  int doubleHash = i * hash_fn2;

  while (table[entry].key != NULL) {
      doubleHash = (++i) * hash_fn2;
      entry = (hash_fn1 + doubleHash) % size;
      
      // don't add if exceeds or equal max probes
      if (i >= MAX_STATS)
        return;
  }

  table[entry].key = key;
  table[entry].data = pred;
  table[entry].keyID = uniqId;
  number++;
}

#endif 
