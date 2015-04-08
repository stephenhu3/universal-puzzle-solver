#ifndef _LINEARHASHDICT_CPP
#define _LINEARHASHDICT_CPP

//LinearHashDict.cpp
#include "LinearHashDict.hpp"
#include <cassert>
#include <cstdlib>//for NULL
#include <iostream>

// An implementation of the dictionary ADT as a hash table with linear probing
//

const int LinearHashDict::primes[] = {53, 97, 193, 389, 769, 1543, 3079,
      6151, 12289, 24593, 49157, 98317, 196613, 393241, 786433, 1572869,
      3145739, 6291469, 12582917, 25165843, 50331653, 100663319,
      201326611, 402653189, 805306457, 1610612741, -1};
// List of good primes for hash table sizes from
// http://planetmath.org/goodhashtableprimes
// The -1 at the end is to guarantee an immediate crash if we run off
// the end of the array.
#define LARGEST_SIZE_INDEX 26

LinearHashDict::LinearHashDict() {
  size_index = 0;
  size = primes[size_index];
  table = new bucket[size](); // Parentheses force initialization to 0
  number = 0;

  // Initialize the array of counters for probe statistics
  probes_stats = new int[MAX_STATS]();
}

LinearHashDict::~LinearHashDict() {
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

  // // Code for calculating average
  int total = 0;
  int samples = 0;
  for (int i=0; i<MAX_STATS; i++) { 
    samples += probes_stats[i];
    for (int j = 0; j < probes_stats[i]; j++)
      total = samples -probes_stats[0];
  }
  cout << "Average: " << (double)total/(double)samples << endl;  

  delete [] probes_stats;
}

int LinearHashDict::hash(string keyID) {
  int h=0;
  for (int i=keyID.length()-1; i>=0; i--) {
    h = (keyID[i] + 31*h) % size;
  }
// 221 Students:  DO NOT CHANGE OR DELETE THE NEXT FEW LINES!!!
// We will use this code when marking to be able to watch what
// your program is doing, so if you change things, we'll mark it wrong.
#ifdef MARKING_TRACE
std::cout << "Hashing " << keyID << " to " << h << std::endl;
#endif
// End of "DO NOT CHANGE" Block
  return h;
}

void LinearHashDict::rehash() {
// 221 Students:  DO NOT CHANGE OR DELETE THE NEXT FEW LINES!!!
// And leave this at the beginning of the rehash() function.
// We will use this code when marking to be able to watch what
// your program is doing, so if you change things, we'll mark it wrong.
#ifdef MARKING_TRACE
std::cout << "*** REHASHING " << size;
#endif
// End of "DO NOT CHANGE" Block

  // std::cout << "Rehashing number = " << number << ", size = " << size << std::endl;
  // copy everything from previous table into new table of next prime size
  // increment size_index, if size_index reached max size, don't resize anymore
  int oldsize = size;

  if (++size_index == LARGEST_SIZE_INDEX)
    return;
  
  size = primes[size_index];
  
  // point to the old table, need to be a new copy
  bucket* oldtable = new bucket[oldsize]();

  int i;

  for (i = 0; i < oldsize; i++) {
    oldtable[i].key = table[i].key;
    oldtable[i].data = table[i].data;
    oldtable[i].keyID = table[i].keyID;
  }

  // table points to entirely new array of buckets new size
  table = new bucket[size]();

  // add back the old values to the new table
  for (i = 0; i < oldsize; i++) {
    if (oldtable[i].key != NULL)
      add(oldtable[i].key, oldtable[i].data);
    else
      table[i].key = NULL;
  }

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

bool LinearHashDict::find(PuzzleState *key, PuzzleState *&pred) {
  // Returns true iff the key is found.
  // Returns the associated value in pred

  // Be sure not to keep calling getUniqId() over and over again!

  // use key to find slot
  string findID = key->getUniqId();
  int i = 0;
  int searchIndex = hash(findID) % size;

  // while not found, keep searching
  while (table[searchIndex].keyID.compare(findID) != 0) {
    searchIndex = (hash(findID) + (++i)) % size;
    
    if (i >= MAX_STATS) {
      // WE MUST SET PRED TO NULL SINCE WE DIDNT FIND IT
      probes_stats[MAX_STATS-1]++;
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
void LinearHashDict::add(PuzzleState *key, PuzzleState *pred) {

  string uniqId = key->getUniqId();

  double loadfactor = (double)number/(double)size;

  if (loadfactor > 0.5)
    rehash();

  int i = 1;
  int entry = hash(uniqId) % size;

  while (table[entry].key != NULL) {
      entry = (hash(uniqId) + (i++)) % size;
      
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