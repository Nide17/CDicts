/*
 * dict_test.c
 *
 * Test code for ISSE dictionary
 *
 * Author: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>

#include "cdict.h"

// Checks that value is true; if not, prints a failure message and
// returns 0 from this function
#define test_assert(value)                                         \
  {                                                                \
    if (!(value))                                                  \
    {                                                              \
      printf("FAIL %s[%d]: %s\n", __FUNCTION__, __LINE__, #value); \
      goto test_error;                                             \
    }                                                              \
  }

typedef struct
{
  const char *city;
  const char *team;
} team_data_t;

team_data_t team_data[] = {{"Atlanta", "Hawks"}, {"Boston", "Celtics"}, {"Brooklyn", "Nets"}, {"Charlotte", "Hornets"}, {"Chicago", "Bulls"}, {"Cleveland", "Cavaliers"}, {"Dallas", "Mavericks"}, {"Denver", "Nuggets"}, {"Detroit", "Pistons"}, {"Golden State", "Warriors"}, {"Houston", "Rockets"}, {"Indiana", "Pacers"}, {"Los Angeles", "Lakers"}, {"Memphis", "Grizzlies"}, {"Miami", "Heat"}, {"Milwaukee", "Bucks"}, {"Minnesota", "Timberwolves"}, {"New Orleans", "Pelicans"}, {"New York", "Knicks"}, {"Oklahoma City", "Thunder"}, {"Orlando", "Magic"}, {"Philadelphia", "76ers"}, {"Phoenix", "Suns"}, {"Portland", "Trail Blazers"}, {"Sacramento", "Kings"}, {"San Antonio", "Spurs"}, {"Toronto", "Raptors"}, {"Utah", "Jazz"}, {"Washington", "Wizards"}};

const int team_data_len = sizeof(team_data) / sizeof(team_data[0]);

/*
 * Tests basic store/retrieve, with just 4 elements (not enough to
 * force a rehash and with no collisions)
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_basic_operation()
{
  const int num_items = 4;
  unsigned int init_capacity = 0;
  CDict dict;

  dict = CD_new();

  init_capacity = CD_capacity(dict);
  test_assert(CD_size(dict) == 0);

  for (int i = 0; i < num_items; i++)
  {
    CD_store(dict, team_data[i].city, team_data[i].team);

    test_assert(CD_size(dict) == i + 1);
    test_assert(CD_capacity(dict) == init_capacity);
    const char *tm = CD_retrieve(dict, team_data[i].city);
    test_assert(tm != NULL);
    test_assert(strcmp(tm, team_data[i].team) == 0);
    test_assert(CD_contains(dict, team_data[i].city));
  }

  test_assert(CD_load_factor(dict) - ((double)num_items / init_capacity) < 0.001);

  // test that we can't retrieve a non-existent key
  test_assert(CD_retrieve(dict, "Non-existent") == NULL);

  // test that we can't delete a non-existent key
  CD_delete(dict, "Non-existent");

  // test that we can't retrieve a deleted key
  CD_delete(dict, team_data[0].city);
  test_assert(CD_retrieve(dict, team_data[0].city) == NULL);

  // test that we can't delete a deleted key
  CD_delete(dict, team_data[0].city);
  test_assert(CD_retrieve(dict, team_data[0].city) == NULL);

  // error cases
  test_assert(CD_retrieve(dict, NULL) == NULL);
  test_assert(CD_contains(dict, NULL) == false);
  CD_store(dict, NULL, "value");
  test_assert(CD_retrieve(dict, NULL) == NULL);
  test_assert(CD_contains(dict, NULL) == false);
  CD_delete(dict, NULL);
  test_assert(CD_retrieve(dict, NULL) == NULL);
  test_assert(CD_contains(dict, NULL) == false);

  test_assert(CD_retrieve(NULL, "key") == NULL);
  test_assert(CD_contains(NULL, "key") == false);
  CD_store(NULL, "key", "value");
  test_assert(CD_retrieve(NULL, "key") == NULL);
  test_assert(CD_contains(NULL, "key") == false);
  CD_delete(NULL, "key");
  test_assert(CD_retrieve(NULL, "key") == NULL);
  test_assert(CD_contains(NULL, "key") == false);

  // corner cases
  CD_store(dict, "", "value");
  test_assert(CD_retrieve(dict, "") != NULL);
  test_assert(CD_contains(dict, "") == true);
  CD_delete(dict, "");
  test_assert(CD_retrieve(dict, "") == NULL);
  test_assert(CD_contains(dict, "") == false);

  CD_store(dict, "key", "");
  test_assert(CD_retrieve(dict, "key") != NULL);
  test_assert(CD_contains(dict, "key") == true);
  CD_delete(dict, "key");
  test_assert(CD_retrieve(dict, "key") == NULL);
  test_assert(CD_contains(dict, "key") == false);

  CD_free(dict);

  return 1;

test_error:
  CD_free(dict);
  return 0;
}

/*
 * Demonstrates basic operation of the dictionary. Returns 1 if all
 * tests, pass, 0 otherwise.
 */
int demonstrate_dict()
{
  CDict dict = CD_new();

  test_assert(CD_size(dict) == 0);
  test_assert(CD_load_factor(dict) == 0.0);

  CD_store(dict, "Atlanta", "Hawks");
  CD_store(dict, "Boston", "Celtics");
  CD_store(dict, "Los Angeles", "Lakers");
  CD_store(dict, "Denver", "Nuggets");

  printf("The Denver team is called the %s\n", CD_retrieve(dict, "Denver"));
  printf("  current load factor: %.2f\n", CD_load_factor(dict));

  CD_delete(dict, "Boston");
  printf("  current size: %d\n", CD_size(dict));

  printf("Printing the entire dictionary:\n");
  CD_print(dict);

  // overwrite an existing value
  CD_store(dict, "Denver", "Broncos");
  printf("After updating Denver to 'Broncos':\n");
  CD_print(dict);

  // add two more elements to force a rehash
  CD_store(dict, "New York", "Knicks");
  CD_store(dict, "Miami", "Head");

  printf("After adding New York and Miami:\n");
  CD_print(dict);

  // test that we can still retrieve the values
  printf("The Denver team is called the %s\n", CD_retrieve(dict, "Denver"));

  // test collisions by adding new elements that hashes to the same slot as an existing element
  CD_store(dict, "New Orleans", "Pelicans");
  CD_store(dict, "New Jersey", "Nets");
  CD_store(dict, "New Mexico", "Rangers");
  CD_store(dict, "New York", "Giants");
  CD_store(dict, "New York", "Rangers");
  CD_store(dict, "New York", "Yankees");
  CD_store(dict, "Arizona", "Cardinals");

  // Delete New York, then get New Orleans, Arizona, and New York
  CD_delete(dict, "New York");
  printf("After deleting New York:\n");
  CD_print(dict);

  printf("The New Orleans team is called the %s\n", CD_retrieve(dict, "New Orleans"));
  printf("The Arizona team is called the %s\n", CD_retrieve(dict, "Arizona"));
  printf("The New York team is called the %s\n", CD_retrieve(dict, "New York"));

  printf("After adding New Orleans, New Jersey, and New Mexico:\n");
  CD_retrieve(dict, "Arizona");
  printf("The Arizona team is called the %s\n", CD_retrieve(dict, "Arizona"));
  CD_retrieve(dict, "New York");
  printf("The New York team is called the %s\n", CD_retrieve(dict, "New York"));

  CD_print(dict);

  CD_store(dict, "New York", "Knicks");
  CD_store(dict, "Pickerington", "Tigers");
  CD_store(dict, "Pickerington", "Panthers");
  CD_store(dict, "Pitsburgh", "Penguins");
  CD_store(dict, "Pitsburgh", "Steelers");
  CD_store(dict, "Washington", "Capitals");
  CD_store(dict, "Okalahoma City", "Thunder");
  CD_store(dict, "Portland", "Trail Blazers");
  CD_store(dict, "Sacramento", "Kings");
  CD_store(dict, "San Antonio", "Spurs");
  CD_store(dict, "Toronto", "Raptors");
  CD_store(dict, "Utah", "Jazz");
  CD_store(dict, "Washington", "Wizards");
  CD_store(dict, "Brooklyn", "Nets");
  CD_store(dict, "Charlotte", "Hornets");
  CD_store(dict, "Chicago", "Bulls");
  CD_delete(dict, "Utah");
  CD_delete(dict, "Washington");
  CD_store(dict, "Cleveland", "Cavaliers");

  CD_print(dict);
  CD_free(dict);

  return 1;

test_error:
  CD_free(dict);
  return 0;
}

/*
 * Tests rehashing, with collisions
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */

int test_rehash()
{
  const int num_items = 10;
  unsigned int init_capacity = 0;
  CDict dict;

  dict = CD_new();

  init_capacity = CD_capacity(dict);
  test_assert(CD_size(dict) == 0);

  for (int i = 0; i < num_items; i++)
  {
    CD_store(dict, team_data[i].city, team_data[i].team);

    test_assert(CD_size(dict) == i + 1);
    const char *tm = CD_retrieve(dict, team_data[i].city);
    test_assert(tm != NULL);
    test_assert(strcmp(tm, team_data[i].team) == 0);

    test_assert(CD_contains(dict, team_data[i].city));
  }

  test_assert(CD_load_factor(dict) - ((double)num_items / init_capacity) < 0.001);
  CD_free(dict);

  return 1;

test_error:
  CD_free(dict);
  return 0;
}

/*
 * Test error cases
 */
int test_error_cases()
{
  CDict dict = CD_new();

  // Test NULL dictionary
  test_assert(CD_retrieve(NULL, "key") == NULL);
  test_assert(CD_contains(NULL, "key") == false);
  CD_store(NULL, "key", "value");

  test_assert(CD_retrieve(NULL, "key") == NULL);
  test_assert(CD_contains(NULL, "key") == false);
  CD_delete(NULL, "key");

  test_assert(CD_retrieve(NULL, "key") == NULL);
  test_assert(CD_contains(NULL, "key") == false);

  // Test NULL key
  test_assert(CD_retrieve(dict, NULL) == NULL);
  test_assert(CD_contains(dict, NULL) == false);
  CD_store(dict, NULL, "value");
  test_assert(CD_retrieve(dict, NULL) == NULL);
  test_assert(CD_contains(dict, NULL) == false);
  CD_delete(dict, NULL);
  test_assert(CD_retrieve(dict, NULL) == NULL);
  test_assert(CD_contains(dict, NULL) == false);

  CD_print(dict);

  // add Denver, then delete it
  CD_store(dict, "Denver", "Nuggets");

  // Test deleting the last element
  CD_delete(dict, "Denver");
  test_assert(CD_size(dict) == 0);
  test_assert(CD_retrieve(dict, "Denver") == NULL);
  test_assert(CD_contains(dict, "Denver") == false);

  // Test dictionary undergoing rehashing
  printf("\n\nAdding 100 elements to force rehashing:\n");
  char k[100][20];
  char val[100][20];

  for (int i = 0; i < 100; i++)
  {
    snprintf(k[i], 20, "Key%d-Weird", i);
    snprintf(val[i], 20, "Value%d-Weird", i);
    CD_store(dict, k[i], val[i]);
  }

  CD_print(dict);
  printf("After adding 100 elements:\n\n");

  // Test deleting last 100 elements
  for (int i = 0; i < 100; i++)
  {
    CD_delete(dict, k[i]);
  }

  CD_print(dict);
  printf("After deleting 100 elements:\n\n");

  // Test dictionary undergoing rehashing
  printf("\n\nAdding 150 elements to force rehashing:\n");
  char key[150][20];
  char value[150][20];

  for (int i = 0; i < 150; i++)
  {
    snprintf(key[i], 20, "Key%d-Weird", i);
    snprintf(value[i], 20, "Value%d-Weird", i);
    CD_store(dict, key[i], value[i]);
  }

  printf("After adding 150 elements:\n\n");
  CD_print(dict);

  for (int i = 0; i < 150; i++)
    CD_delete(dict, key[i]);

  printf("After deleting 150 elements:\n\n");
  CD_print(dict);
  printf("Retrieving Key13-Weird: %s\n", CD_retrieve(dict, "Key13-Weird"));
  printf("Contains Key13-Weird: %s\n", CD_contains(dict, "Key13-Weird") ? "true" : "false");
  CD_delete(dict, "Key13-Weird");
  printf("Retrieving Key13-Weird: %s\n", CD_retrieve(dict, "Key13-Weird"));
  CD_free(dict);
  
  return 1;

test_error:
  CD_free(dict);
  return 0;
}

/*
 * Callback function for foreach
 */
void foreach_callback(CDictKeyType key, CDictValueType value, void *cb_data)
{
  printf("  %s: %s\n", key, value);
}

/**
 * Tests the foreach function
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_foreach()
{
  const int num_items = 4;
  unsigned int init_capacity = 0;
  CDict dict;

  dict = CD_new();

  init_capacity = CD_capacity(dict);
  test_assert(CD_size(dict) == 0);

  for (int i = 0; i < num_items; i++)
  {
    CD_store(dict, team_data[i].city, team_data[i].team);

    test_assert(CD_size(dict) == i + 1);
    test_assert(CD_capacity(dict) == init_capacity);
    const char *tm = CD_retrieve(dict, team_data[i].city);
    test_assert(tm != NULL);
    test_assert(strcmp(tm, team_data[i].team) == 0);

    test_assert(CD_contains(dict, team_data[i].city));
  }

  test_assert(CD_load_factor(dict) - ((double)num_items / init_capacity) < 0.001);

  CD_foreach(dict, foreach_callback, NULL);

  CD_free(dict);

  return 1;

test_error:
  CD_free(dict);
  return 0;
}

int test_update_key_value_pair()
{
  CDict dict = CD_new();
  CD_store(dict, "key1", "value1");
  test_assert(CD_contains(dict, "key1"));
  test_assert(strcmp(CD_retrieve(dict, "key1"), "value1") == 0);
  CD_store(dict, "key1", "value2");                              // updating the value
  test_assert(CD_contains(dict, "key1"));                        // key should still exist
  test_assert(strcmp(CD_retrieve(dict, "key1"), "value2") == 0); // new value should be retrieved
  CD_free(dict);
  return 1;

test_error:
  CD_free(dict);
  return 0;
}

int test_insert_duplicate_keys()
{
  CDict dict = CD_new();
  CD_store(dict, "key1", "value1");
  test_assert(strcmp(CD_retrieve(dict, "key1"), "value1") == 0);
  CD_store(dict, "key1", "value2");                              // duplicate key with new value
  test_assert(strcmp(CD_retrieve(dict, "key1"), "value2") == 0); // new value should be retrieved
  CD_free(dict);
  return 1;
test_error:
  CD_free(dict);
  return 0;
}

int test_capacity_limits()
{
  CDict dict = CD_new();

  for (int i = 0; i < 100 + 10; i++)
  {
    char key[15];
    sprintf(key, "key%d", i);
    CD_store(dict, key, "value");
  }

  test_assert(CD_capacity(dict) >= 100);
  CD_free(dict);
  return 1;
test_error:
  CD_free(dict);
  return 0;
}

int test_null_values()
{
  CDict dict = CD_new();
  CD_store(dict, "key1", NULL);
  test_assert(CD_retrieve(dict, "key1") == NULL);
  CD_free(dict);
  return 1;
test_error:
  CD_free(dict);
  return 0;
}

int test_handle_collisions()
{
  CDict dict = CD_new();
  char *key1 = "key1";
  char *key2 = "key2";

  // Assuming these two keys hash to the same slot
  CD_store(dict, key1, "value1");
  CD_store(dict, key2, "value2");

  test_assert(strcmp(CD_retrieve(dict, key1), "value1") == 0);
  test_assert(strcmp(CD_retrieve(dict, key2), "value2") == 0);

  CD_free(dict);
  return 1;

test_error:
  CD_free(dict);
  return 0;
}

int main()
{
  int passed = 0;
  int num_tests = 0;

  num_tests++;
  passed += demonstrate_dict();
  num_tests++;
  passed += test_basic_operation();
  num_tests++;
  passed += test_rehash();
  num_tests++;
  passed += test_foreach();
  num_tests++;
  passed += test_error_cases();
  num_tests++;
  passed += test_update_key_value_pair();
  num_tests++;
  passed += test_insert_duplicate_keys();
  num_tests++;
  passed += test_null_values();
  num_tests++;
  passed += test_handle_collisions();
  num_tests++;
  passed += test_capacity_limits();

  printf("Passed %d/%d test cases\n", passed, num_tests);
  fflush(stdout);
  return 0;
}
