/*
 * cdict.c
 *
 * Dictionary based on a hash table utilizing open addressing to
 * resolve collisions.
 *
 * Author: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "cdict.h"

#define DEBUG

#define DEFAULT_DICT_CAPACITY 8
#define REHASH_THRESHOLD 0.6

typedef enum
{
  SLOT_UNUSED = 0,
  SLOT_IN_USE,
  SLOT_DELETED
} CDictSlotStatus;

struct _hash_slot
{
  CDictSlotStatus status;
  CDictKeyType key;
  CDictValueType value;
};

struct _dictionary
{
  unsigned int num_stored;
  unsigned int num_deleted;
  unsigned int capacity;
  struct _hash_slot *slot;
};

/*
 * Return a pseudorandom hash of a key with reasonable distribution
 * properties. Based on Python's implementation before Python 3.4
 *
 * Parameters:
 *   str   The string to be hashed
 *   capacity  The capacity of the dictionary
 *
 * Returns: The hash, in the range 0-(capacity-1) inclusive
 */
static unsigned int _CD_hash(CDictKeyType str, unsigned int capacity)
{
  unsigned int x;
  unsigned int len = 0;

  for (const char *p = str; *p; p++)
    len++;

  if (len == 0)
    return 0;

  const char *p = str;
  x = (unsigned int)*p << 7;

  for (int i = 0; i < len; i++)
    x = (1000003 * x) ^ (unsigned int)*p++;

  x ^= (unsigned int)len;

  return x % capacity;
}

/*
 * Rehash the dictionary, doubling its capacity
 *
 * Parameters:
 *   dict     The dictionary to rehash
 *
 * Returns: None
 */
static void _CD_rehash(CDict dict)
{

  if (dict == NULL)
    return;

  // allocate new slot array
  struct _hash_slot *new_slot = malloc(sizeof(struct _hash_slot) * dict->capacity * 2);

  // initialize new slot array
  for (int i = 0; i < dict->capacity * 2; i++)
    new_slot[i].status = SLOT_UNUSED;

  // rehash existing elements into new slot array
  for (int i = 0; i < dict->capacity; i++)
  {
    if (dict->slot[i].status == SLOT_IN_USE)
    {
      unsigned int hash = _CD_hash(dict->slot[i].key, dict->capacity * 2);
      while (new_slot[hash].status == SLOT_IN_USE)
        hash = (hash + 1) % (dict->capacity * 2);

      new_slot[hash].status = SLOT_IN_USE;
      new_slot[hash].key = dict->slot[i].key;
      new_slot[hash].value = dict->slot[i].value;
    }
  }

  // free old slot array
  free(dict->slot);

  // update dict to use new slot array
  dict->slot = new_slot;
  dict->capacity *= 2;
  dict->num_deleted = 0;
}

// Documented in .h file
CDict CD_new()
{
  CDict dict = malloc(sizeof(struct _dictionary));
  assert(dict);

  dict->num_stored = 0;
  dict->num_deleted = 0;
  dict->capacity = DEFAULT_DICT_CAPACITY;

  dict->slot = malloc(sizeof(struct _hash_slot) * dict->capacity);
  assert(dict->slot);

  for (int i = 0; i < dict->capacity; i++)
    dict->slot[i].status = SLOT_UNUSED;

  return dict;
}

// Documented in .h file
void CD_free(CDict dict)
{
  if (dict == NULL)
    return;

  // deallocate slot array
  free(dict->slot);

  // deallocate dictionary struct
  free(dict);
}

// documented in .h file
unsigned int CD_size(CDict dict)
{

  if (dict == NULL)
    return 0;

#ifdef DEBUG
  // iterate across slots, counting number of keys found
  int used = 0;
  int deleted = 0;

  for (int i = 0; i < dict->capacity; i++)
    if (dict->slot[i].status == SLOT_IN_USE)
      used++;
    else if (dict->slot[i].status == SLOT_DELETED)
      deleted++;

  assert(used == dict->num_stored);
  assert(deleted == dict->num_deleted);
#endif

  return dict->num_stored;
}

// documented in .h file
unsigned int CD_capacity(CDict dict)
{
  if (dict == NULL)
    return 0;

  return dict->capacity;
}

// Documented in .h file
bool CD_contains(CDict dict, CDictKeyType key)
{
  if (dict == NULL || key == NULL)
    return false;

  unsigned int hash = _CD_hash(key, dict->capacity);

  while (dict->slot[hash].status != SLOT_UNUSED)
  {
    if (dict->slot[hash].status == SLOT_IN_USE && strcmp(dict->slot[hash].key, key) == 0)
      return true;

    hash = (hash + 1) % dict->capacity;
  }

  return false;
}

// Documented in .h file
void CD_store(CDict dict, CDictKeyType key, CDictValueType value)
{

  if (dict == NULL || key == NULL)
    return;

  assert(dict);
  assert(key);

  // if load factor is too high, rehash
  if ((double)(dict->num_stored + dict->num_deleted) / dict->capacity > REHASH_THRESHOLD)
    _CD_rehash(dict);

  unsigned int hash = _CD_hash(key, dict->capacity);

  while (dict->slot[hash].status == SLOT_IN_USE && strcmp(dict->slot[hash].key, key) != 0)
    hash = (hash + 1) % dict->capacity;

  if (dict->slot[hash].status == SLOT_IN_USE)
  {
    // key already exists, overwrite value
    dict->slot[hash].value = value;
  }
  else
  {
    // key does not exist, store key and value
    dict->slot[hash].status = SLOT_IN_USE;
    dict->slot[hash].key = key;
    dict->slot[hash].value = value;
    dict->num_stored++;
  }
}

// Documented in .h file
CDictValueType CD_retrieve(CDict dict, CDictKeyType key)
{
  if (dict == NULL || key == NULL)
    return INVALID_VALUE;

  assert(dict);
  assert(key);

  unsigned int hash = _CD_hash(key, dict->capacity);

  while (dict->slot[hash].status != SLOT_UNUSED)
  {
    if (dict->slot[hash].status == SLOT_IN_USE && strcmp(dict->slot[hash].key, key) == 0)
      return dict->slot[hash].value;

    hash = (hash + 1) % dict->capacity;
  }

  return INVALID_VALUE;
}

// Documented in .h file
void CD_delete(CDict dict, CDictKeyType key)
{
  if (dict == NULL || key == NULL)
    return;

  assert(dict);
  assert(key);

  unsigned int hash = _CD_hash(key, dict->capacity);

  while (dict->slot[hash].status != SLOT_UNUSED)
  {
    if (dict->slot[hash].status == SLOT_IN_USE && strcmp(dict->slot[hash].key, key) == 0)
    {
      dict->slot[hash].status = SLOT_DELETED;
      dict->num_stored--;
      dict->num_deleted++;
      return;
    }

    hash = (hash + 1) % dict->capacity;
  }

  return;
}

// Documented in .h file
double CD_load_factor(CDict dict)
{
  if (dict == NULL)
    return 0;

  assert(dict);

  // load factor = (num_stored + num_deleted) / capacity
  return (double)(dict->num_stored + dict->num_deleted) / dict->capacity;
}

// Documented in .h file
void CD_print(CDict dict)
{
  if (dict == NULL)
    return;

  assert(dict);

  printf("*** capacity: %d  stored: %d  deleted: %d  load_factor: %.2f\n", dict->capacity, dict->num_stored, dict->num_deleted, CD_load_factor(dict));

  for (int i = 0; i < dict->capacity; i++)
  {
    if (dict->slot[i].status == SLOT_IN_USE)
      printf("\t%02d: IN_USE key=%s hash=%d value=%s\n", i, dict->slot[i].key, _CD_hash(dict->slot[i].key, dict->capacity), dict->slot[i].value);
    else if (dict->slot[i].status == SLOT_DELETED)
      printf("\t%02d: DELETED\n", i);
    else
      printf("\t%02d: unused\n", i);
  }

  printf("\n");
}

void CD_foreach(CDict dict, CD_foreach_callback callback, void *cb_data)
{
  if (dict == NULL || callback == NULL)
    return;

  assert(dict);

  for (int i = 0; i < dict->capacity; i++)
  {
    if (dict->slot[i].status == SLOT_IN_USE)
      callback(dict->slot[i].key, dict->slot[i].value, cb_data);
  }
}
