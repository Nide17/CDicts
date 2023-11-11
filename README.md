## CDicts

__INTRODUCTION__

The "CDicts" is a C program that implements a simple C dictionary and Cdicts are based on hash tables. A new CDict will have a hash table with 8 slots. Items can be added or deleted; slots that previously held items that have been deleted appear in the hash table with the type DELETED.  When the load factor of the hash table exceeds 0.6, the CDict is automatically rehashed into a new hash table with double the number of slots. Rehashing reclaims deleted slots.

__DESCRIPTION__

CDicts consists of the following components:

- CD_new: creates a new CDict
- CD_free: frees the memory associated with a CDict
- CD_size: returns the number of items in a CDict
- CD_capacity: returns the number of slots in a CDict
- CD_contains: returns true if a CDict contains a given key
- CD_store: stores a key-value pair in a CDict
- CD_retrieve: retrieves the value associated with a given key
- CD_delete: deletes a key-value pair from a CDict
- CD_load_factor: returns the load factor of a CDict
- CD_print: prints the contents of a CDict
- CD_foreach: applies a function to each item in a CDict
  
__USAGE__

To use CDicts, follow these steps:

1. Compile the project using the provided Makefile. Run the following command in your terminal:
```bash
make
```
2. Run the CDicts program:
```bash
./cdict_test
```

__IMPORTANCE__

It is a versatile tool for evaluating arithmetic expressions interactively. It offers comprehensive support for various operators and nested expressions.

__KEYWORDS__

<mark>ISSE</mark>     <mark>CMU</mark>     <mark>Assignment10</mark>     <mark>CDicts</mark>     <mark>C Programming</mark>     <mark>Hash Tables</mark>     <mark>C Dictionaries</mark>  

__AUTHOR__

parmenin (Niyomwungeri Parmenide ISHIMWE) at CMU-Africa - MSIT

__DATE__

 November 17, 2023