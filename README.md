## CDict

__INTRODUCTION__

The "CDict" is a C program that implements a simple C dictionary that is based on hash tables. A new CDict has a hash table with 8 slots. Items can be added or deleted; slots that previously held items that have been deleted appear in the hash table with the type DELETED.  When the load factor of the hash table exceeds 0.6, the CDict is automatically rehashed into a new hash table with double the number of slots. Rehashing reclaims deleted slots.

__DESCRIPTION__

CDict consists of the following components or functions:

- **CD_new**: creates a new CDict.
- **CD_free**: frees the memory associated with a CDict.
- **CD_size**: returns the number of items in a CDict.
- **CD_capacity**: returns the number of slots in a CDict.
- **CD_contains**: returns true if a CDict contains a given key.
- **CD_store**: stores a key-value pair in a CDict.
- **CD_retrieve**: retrieves the value associated with a given key.
- **CD_delete**: deletes a key-value pair from a CDict.
- **CD_load_factor**: returns the load factor of a CDict.
- **CD_print**: prints the contents of a CDict.
- **CD_foreach**: applies a function to each item in a CDict.
- **_CD_rehash**: rehashes a CDict into a new CDict with double the number of slots.
  
__USAGE__

To use CDict, follow these steps:

1. Compile the project using the provided Makefile. Run the following command in the terminal of a machine with the GNU C compiler installed:
```bash
make
```
2. Run the CDict program:
```bash
./cdict_test
```

__IMPORTANCE__

This is a versatile tool that can be used for many purposes. For example, it can be used to store the names of students and their grades in a class. It can also be used to store the names of students and their student IDs. It can also be used to store the names of students and their email addresses. It can also be used to store the names of students and their phone numbers, etc.

__KEYWORDS__

<mark>ISSE</mark>     <mark>CMU</mark>     <mark>Assignment10</mark>     <mark>CDict</mark>     <mark>C Programming</mark>     <mark>Hash Tables</mark>     <mark>C Dictionaries</mark>  

__AUTHOR__

parmenin (Niyomwungeri Parmenide ISHIMWE) at CMU-Africa - MSIT

__DATE__

 November 16, 2023