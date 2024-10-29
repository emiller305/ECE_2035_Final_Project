// ============================================
// The HashTable class file
//
// Copyright 2023 Georgia Tech. All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must NOT be made publicly available anywhere.
//==================================================================

/****************************************************************************
 * Include the Public Interface
 *
 * By including the public interface at the top of the file, the compiler can
 * enforce that the function declarations in the the header are not in
 * conflict with the definitions in the file. This is not a guarantee of
 * correctness, but it is better than nothing!
 ***************************************************************************/
#include "hash_table.h"

/****************************************************************************
 * Include other private dependencies
 *
 * These other modules are used in the implementation of the hash table module,
 * but are not required by users of the hash table.
 ***************************************************************************/
#include <stdlib.h> // For malloc and free
#include <stdio.h>  // For printf

/****************************************************************************
 * Hidden Definitions
 *
 * These definitions are not available outside of this file. However, because
 * the are forward declared in hash_table.h, the type names are
 * available everywhere and user code can hold pointers to these structs.
 ***************************************************************************/
/**
 * This structure represents an a hash table.
 * Use "HashTable" instead when you are creating a new variable. [See top comments]
 */
struct _HashTable
{
    /** The array of pointers to the head of a singly linked list, whose nodes
        are HashTableEntry objects */
    HashTableEntry **buckets;

    /** The hash function pointer */
    HashFunction hash;

    /** The number of buckets in the hash table */
    unsigned int num_buckets;
};

/**
 * This structure represents a hash table entry.
 * Use "HashTableEntry" instead when you are creating a new variable. [See top comments]
 */
struct _HashTableEntry
{
    /** The key for the hash table entry */
    unsigned int key;

    /** The value associated with this hash table entry */
    void *value;

    /**
     * A pointer pointing to the next hash table entry
     * NULL means there is no next entry (i.e. this is the tail)
     */
    HashTableEntry *next;
};

/****************************************************************************
 * Private Functions
 *
 * These functions are not available outside of this file, since they are not
 * declared in hash_table.h.
 ***************************************************************************/
/**
 * createHashTableEntry
 *
 * Helper function that creates a hash table entry by allocating memory for it on
 * the heap. It initializes the entry with key and value, initialize pointer to
 * the next entry as NULL, and return the pointer to this hash table entry.
 *
 * @param key The key corresponds to the hash table entry
 * @param value The value stored in the hash table entry
 * @return The pointer to the hash table entry
 */
static HashTableEntry *createHashTableEntry(unsigned int key, void *value)
{
    // allocate memory for hash table entry on the heap
    HashTableEntry *newEntry = (HashTableEntry*)malloc(sizeof(HashTableEntry));

    // initialize components of hash table entry
    newEntry->key = key;
    newEntry->value = value;
    newEntry->next = NULL;

    // return pointer to new hash table entry
    return newEntry;
}

/**
 * findItem
 *
 * Helper function that checks whether there exists the hash table entry that
 * contains a specific key.
 *
 * @param hashTable The pointer to the hash table.
 * @param key The key corresponds to the hash table entry
 * @return The pointer to the hash table entry, or NULL if key does not exist
 */
static HashTableEntry *findItem(HashTable *hashTable, unsigned int key)
{
    // pointer to the item we want to find
    HashTableEntry *item = hashTable->buckets[hashTable->hash(key)];
    // loop to check if the key exists
    while(item) {
        if (item->key == key) {
            return item; // return pointer to the hash table entry
        }
        item = item->next; // move to next node
    } // end loop
    return NULL; // key does not exist
}

/****************************************************************************
 * Public Interface Functions
 *
 * These functions implement the public interface as specified in the header
 * file, and make use of the private functions and hidden definitions in the
 * above sections.
 ****************************************************************************/
// The createHashTable is provided for you as a starting point.
HashTable *createHashTable(HashFunction hashFunction, unsigned int numBuckets)
{
    // The hash table has to contain at least one bucket. Exit gracefully if
    // this condition is not met.
    if (numBuckets == 0)
    {
        printf("Hash table has to contain at least 1 bucket...\n");
        exit(1);
    }

    // Allocate memory for the new HashTable struct on heap.
    HashTable *newTable = (HashTable *)malloc(sizeof(HashTable));

    // Initialize the components of the new HashTable struct.
    newTable->hash = hashFunction;
    newTable->num_buckets = numBuckets;
    newTable->buckets = (HashTableEntry **)malloc(numBuckets * sizeof(HashTableEntry *));

    // As the new buckets are empty, init each bucket as NULL.
    unsigned int i;
    for (i = 0; i < numBuckets; ++i)
    {
        newTable->buckets[i] = NULL;
    }

    // Return the new HashTable struct.
    return newTable;
}

void destroyHashTable(HashTable *hashTable)
{
    // Loop through each bucket of the hash table to remove all items.
    unsigned int numBuckets = hashTable->num_buckets;
    for (unsigned int i = 0; i < numBuckets; ++i) {
        // temp variable to hold the first entry of the ith bucket
        HashTableEntry *temp = hashTable->buckets[i];
        // delete all entries
        while (temp) {
            HashTableEntry *next = temp->next;
            free(temp->value);
            free(temp);
            temp = next;

        }
    } // end loop
    // Free buckets
    free(hashTable->buckets);
    // Free hash table
    free(hashTable);
}

void *insertItem(HashTable *hashTable, unsigned int key, void *value)
{
    // First, we want to check if the key is present in the hash table.
    HashTableEntry *oldItem = hashTable->buckets[hashTable->hash(key)];
    void *oldValue;
    // If the key is present in the hash table, store new value and return old value
    if (oldItem != NULL && oldItem->key == key) {
        oldValue = oldItem->value;
        oldItem->value = value;
        return oldValue;
    }
    // If not, create entry for new value and return NULL
    HashTableEntry *newItem = createHashTableEntry(key, value);
    HashTableEntry *head = hashTable->buckets[hashTable->hash(key)];
    if (head) {
        newItem->next = head;
    }
    hashTable->buckets[hashTable->hash(key)] = newItem;
    return NULL;
}

void *getItem(HashTable *hashTable, unsigned int key)
{
    // First, we want to check if the key is present in the hash table.
    // If the key exists, return the value.
    // call findItem method
    HashTableEntry *item = findItem(hashTable, key);
    if (item) return item->value; // return the pointer to the value in the entry
    // If not return NULL
    return NULL;
}

void *removeItem(HashTable *hashTable, unsigned int key)
{
    // Get the bucket number and the head entry
    unsigned int bucket_num = hashTable->hash(key);
    HashTableEntry *head = hashTable->buckets[bucket_num]; // head of the bucket
    HashTableEntry *temp = head; // temporary variable
    void *tempValue;

    // if hashTable is empty
    if (!head) return NULL;
    // If the head holds the key, change the head to the next value, and return the old value
    tempValue = head->value;
    if (head->key == key) {
        temp = head->next;
        free(head);
        hashTable->buckets[bucket_num] = temp; // head is now temp
        return tempValue;
    }
    // If not the head, search for the key to be removed
    temp = head;
    while(temp->next) {
        if (temp->next->key == key) {
            // unlink the node from the list and return the old value
            tempValue = temp->next->value;
            HashTableEntry *item = temp->next; // another temp
            temp->next = temp->next->next;
            free(item);
            return tempValue;
        }
        temp = temp->next;
    }
    // If the key is not present in the list, return NULL
    return NULL;
}

void deleteItem(HashTable *hashTable, unsigned int key)
{
   // Remove the entry and free the returned data
    // call removeItem
    void *data = removeItem(hashTable, key);
    // check if value returned from remove is not NULL
    if (data) {
        // free that value
        free(data);
    }
}