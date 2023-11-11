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
    test_assert(CD_capacity(dict) == init_capacity || CD_capacity(dict) == init_capacity * 2);
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

  printf("Passed %d/%d test cases\n", passed, num_tests);
  fflush(stdout);
  return 0;
}
