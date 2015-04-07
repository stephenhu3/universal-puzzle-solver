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

// void LinearHashDict::callrehash() {
//   rehash();
// }      

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

  // copy everything from previous table into new table of next prime size
  // increment size_index, if size_index reached max size, don't resize anymore
  int oldsize = size;
  if (++size_index == LARGEST_SIZE_INDEX);
    return;
  printf("size index %d\n",size_index);
  size = primes[size_index];
  // issue here, pointer to old table, not a copy of the old table
  // better to create an entirely new table and copy over
  // bucket* oldtable = new bucket[oldsize]();
  
  // point to the old table, need to be a new copy
  bucket* oldtable = new bucket[oldsize]();

  int i;
  // are these actually new values, or pointer to the old table?
  for (i = 0; i < oldsize; i++) {
    // PuzzleState *key_copy = new PuzzleState;
    // string keyID_copy = new string;
    // PuzzleState *data_copy = new PuzzleState;

    bucket* bucket_copy = new bucket();

    bucket_copy->key = table[i].key;
    bucket_copy->data = table[i].data;
    bucket_copy->keyID = table[i].keyID;
    
    oldtable[i] = *bucket_copy;
    // oldtable[i].key = key_copy;
    // oldtable[i].data = data_copy;
    // oldtable[i].keyID = keyID_copy;
  }
  // memcpy(oldtable, table, sizeof(table));
  
  // delete [] table;

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

  // use key to find slot, then compare the pred
  string findID = key->getUniqId();
  int i = 0;
  int searchIndex = hash(findID) % size;

  // while not found, keep searching
  while (table[searchIndex].keyID.compare(findID) != 0) {
    probes_stats[i]++;
    searchIndex = (hash(findID) + (++i)) % size;
    // problem is rehash isn't actually changing size
    // printf("size: %d\n", size);
    // printf("i: %d\n", i);
    // printf("number: %d\n", number);
    if (i >= MAX_STATS)
      return false; // could not find it
  }

  // found it
  probes_stats[i]++;
  // printf("found it\n");
  pred = table[searchIndex].data; // Got it!  Get the result.
  return true;
}

// You may assume that no duplicate PuzzleState is ever added.
void LinearHashDict::add(PuzzleState *key, PuzzleState *pred) {


   //For debugging purposes
  // int j;
  // for (j = 0; j < size; j++) {
  //   std::cout << "j: " << j << "keyID: " << table[j].keyID << std::endl;
  // }

  
  string uniqId = key->getUniqId();

  int i = 1;
  int entry = hash(uniqId) % size;

  while (table[entry].key != NULL) {
      entry = (hash(uniqId) + (i++)) % size;
      
      // table full
      if (i >= MAX_STATS)
        return;
  }

  table[entry].key = key;
  table[entry].data = pred;
  table[entry].keyID = uniqId;
  number++;

  double loadfactor = (double)number/(double)size;
  printf("load factor: %f\n", loadfactor);

  if (loadfactor > 0.5) {
    printf("rehashing\n");
    rehash();
    //issue here where the new array isn't actaully of new size, size not updated, scope issue?
    // return add(key, pred);
  }
}

#endif 