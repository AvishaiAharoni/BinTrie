#ifndef BINTRIE_H_
#define BINTRIE_H_

/******************************************************************************/

enum return_status { SUCCESS, MALLOC_FAIL, IP_TAKEN, IP_NOT_FOUND };

typedef struct btrie btrie_t;

typedef struct btrie_node btrie_node_t;

typedef struct value value_t;

/******************************************************************************/

/* to create a new trie.
   if fail - returns NULL, and for sucssess - pointer to the trie */
btrie_t *BinTrieCreate(size_t height);

/* to destroy a trie and all its elements */
void BinTrieDestroy(btrie_t *btrie);

/* to count all the free possibilities in the trie */
size_t BinTrieCountPossibilities(const btrie_t *btrie);

/* to mark a path as TAKEN.
   if the path is already token - returns IP_TAKEN,
   if the allocation failed - returns - MALLOC_FAIL,
   for sucssess - returns SUCCESS */
int BinTrieAskForPath(btrie_t *btrie, size_t ip);

/* to print the trie and all its elements (in-order) */
void PrintTrie(const btrie_t *trie);

/* to mark a path as FREE.
   if the path is already free - returns IP_NOT_FOUND,
   if the allocation failed - returns - MALLOC_FAIL,
   for sucssess - returns SUCCESS */
int BinTrieRemovePath(btrie_t *btrie, size_t ip_to_free);


#endif
