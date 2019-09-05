#include <stdio.h>		/* printf */
#include <stddef.h>		/*size_t*/
#include <stdlib.h>		/*malloc*/
#include <assert.h>		/*assert*/

#include "bintrie.h"

/******************************************************************************/

enum state_of_bit { NOT_EXIST = -1, FREE, TAKEN, DUMMY };
enum sons_side { LEFT, RIGHT, NUM_OF_SONS };

/******************************************************************************/

struct btrie
 {
	btrie_node_t *root;
	size_t height;
 };

struct value 
 {
	unsigned char bit_val;
	int bit_state;
 };

struct btrie_node
 {
	value_t data;
	btrie_node_t *sons_side[NUM_OF_SONS];
 };

/******************************************************************************/

/* help function */
static int OppositeSide(int side)
{
	assert((LEFT == side) || (RIGHT == side));

	return (side == LEFT ? RIGHT : LEFT);
}

/******************************************************************************/

/* help function */
static btrie_node_t *GetRoot(const btrie_t *btrie)
{
	assert(btrie);
	assert(btrie->root);

	return (btrie->root);
}

/******************************************************************************/

/* help function */
static size_t GetHeight(const btrie_t *btrie)
{
	assert(btrie);

	return (btrie->height);
}

/******************************************************************************/

/* help function */
static void PrintNodes(const btrie_node_t *node)
{
    if (node == NULL)
    {
        return;
    }
      
    printf("[%d|%d]-> ", node->data.bit_val, node->data.bit_state);
    
    if ((node->sons_side[LEFT] == NULL) && (node->sons_side[RIGHT] == NULL))
    {
        printf("Leaf\n");
    }
   
    PrintNodes(node->sons_side[LEFT]);
    PrintNodes(node->sons_side[RIGHT]);
}
   
/******************************************************************************/

/* help function */
static int GetBit(size_t num, size_t idx)
{
	return (1 == ((num >> (idx - 1)) & 1));
}

/******************************************************************************/

/* help function to set the data in a new node (with given status) */
static btrie_node_t *InitNode(btrie_node_t *node, int side, int status)
{
	assert(node);
	assert((LEFT == side) || (RIGHT == side));

	node->sons_side[side]->sons_side[LEFT] = NULL;
	node->sons_side[side]->sons_side[RIGHT] = NULL;
	node->sons_side[side]->data.bit_val = side;
	node->sons_side[side]->data.bit_state = status;

	return (node);
}

/******************************************************************************/

/* help function */
static int InitTrie(btrie_t *trie, size_t height)
{
	assert(trie);
	assert(0 < height);
	
	trie->root = (btrie_node_t *)malloc(sizeof(btrie_node_t));
	if (NULL == trie->root)
	{
		free(trie); trie = NULL;
		
		return (MALLOC_FAIL);
	}
	
	trie->height = height;
	
	trie->root->sons_side[LEFT] = NULL;
	trie->root->sons_side[RIGHT] = NULL;
	trie->root->data.bit_val = DUMMY;
	trie->root->data.bit_state = FREE;
	
	return (SUCCESS);
}

/******************************************************************************/

/* to free all the elements in the trie */
static void FreeNodes(btrie_node_t *node)
{
	if (node)
	{
		FreeNodes(node->sons_side[LEFT]);
		FreeNodes(node->sons_side[RIGHT]);
		free(node); node = NULL;
	}
}

/******************************************************************************/

/* help function */
static size_t PowerOfTwo(int exp)
{
	assert(0 <= exp);

	return (1 << exp);
}

/******************************************************************************/

/* help function */
static size_t CountFreeLeaf(const btrie_node_t *node, size_t height, int parent_state)
{
	size_t counter = 0;

	assert((FREE == parent_state) || (TAKEN == parent_state));

	if (NULL == node)
	{
		if (TAKEN != parent_state)
		{
			return (PowerOfTwo(height));
		}
		
		return (0);
	}
	
	if (0 == height)
	{
		if (FREE == node->data.bit_state)
		{
			return (1);
		}
		
		return (0);
	}
		
	counter += CountFreeLeaf(node->sons_side[LEFT], height - 1, node->data.bit_state);
	counter += CountFreeLeaf(node->sons_side[RIGHT], height - 1, node->data.bit_state);

	return (counter);
}

/******************************************************************************/

/* help function */
static int GetSideToMove(const btrie_node_t *node, size_t height, size_t ip)
{
	assert(node);

	return (GetBit(ip, height) == 0 ? LEFT : RIGHT);
}

/******************************************************************************/

/* help function to create a new node, and set its data by a given status */
static btrie_node_t *CreateNode(btrie_node_t *node, int side, int status)
{
	assert(node);
	assert((LEFT == side) || (RIGHT == side));
	assert((FREE == status) || (TAKEN == status));
	
	node->sons_side[side] = (btrie_node_t *)malloc(sizeof(btrie_node_t));
	if (NULL == node->sons_side[side])
	{
		return (NULL);
	}
	
	return (InitNode(node, side, status));
}

/******************************************************************************/

/* help function */
static void FreeSons(btrie_node_t *node)
{
	assert(node);

	free(node->sons_side[LEFT]); node->sons_side[LEFT] = NULL;
	free(node->sons_side[RIGHT]); node->sons_side[RIGHT] = NULL;
}

/******************************************************************************/

/* help function */
static int AreSonsLeaves(const btrie_node_t *node)
{
	assert(node);
	
	return ((NULL == node->sons_side[LEFT]) &&
			(NULL == node->sons_side[RIGHT]));
}

/******************************************************************************/

/* help function */
static int IsSonLeaf(const btrie_node_t *node)
{
	assert(node);
	
	return ((1 == AreSonsLeaves(node->sons_side[LEFT])) &&
			(1 == AreSonsLeaves(node->sons_side[RIGHT])));
}

/******************************************************************************/

/* help function */
static int GetStatus(const btrie_node_t *node)
{
	return (NULL == node ? NOT_EXIST : node->data.bit_state);
}

/******************************************************************************/

/* help function */
static int DoSonsHaveTheSameStatus(const btrie_node_t *node, int status)
{
	assert(node);
	assert((FREE == status) || (TAKEN == status));
		
	return ((status == GetStatus(node->sons_side[LEFT])) && 
			(status == GetStatus(node->sons_side[RIGHT])));
}

/******************************************************************************/

/* help function */
static void UpdateNodesToLazy(btrie_node_t *node, int side, int status)
{
	assert(node);
	assert((LEFT == side) || (RIGHT == side));
	assert((FREE == status) || (TAKEN == status));

	side = OppositeSide(side);
	
	if ((NULL != node->sons_side[side]) &&
		(1 == DoSonsHaveTheSameStatus(node, status)) &&
		(status == node->sons_side[side]->data.bit_state))
	{
		FreeSons(node);
		node->data.bit_state = status;
	}
}

/******************************************************************************/

/* help function */
static int CheckAndSetInLeaf(btrie_node_t *node, int state_to_find, int err_res)
{
	assert(node);
	assert((FREE == state_to_find) || (TAKEN == state_to_find));
	assert((IP_NOT_FOUND == err_res) || (IP_TAKEN == err_res));
	
	if (state_to_find == node->data.bit_state)
	{
		return (err_res);
	}
	
	node->data.bit_state = state_to_find;
	
	return (SUCCESS);
}

/******************************************************************************/

/* help function to find the path, insert a node, and - if necessary - 
   release and mark it */
static int FindPath(btrie_node_t *node, size_t height, size_t ip)
{
	int side = LEFT;
	int res = SUCCESS;
		
	assert(node);

	if (TAKEN == node->data.bit_state)
	{
		res = IP_TAKEN;
		
		return (res);
	}
	
	if (0 == height)
	{
		res = CheckAndSetInLeaf(node, TAKEN, IP_TAKEN);
		
		return (res);
	}

	side = GetSideToMove(node, height, ip);

	if ((NULL == node->sons_side[side]) &&
		(NULL == CreateNode(node, side, FREE)))
	{
		return (MALLOC_FAIL);
	}

	res = FindPath(node->sons_side[side], height - 1, ip);
	
	if (SUCCESS == res)
	{
		UpdateNodesToLazy(node, side, TAKEN);
	}
	
	return (res);
}

/******************************************************************************/

/* help function to find the path, remove a node, and - if necessary - 
   release and mark it */
static int RemovePath(btrie_node_t *node, size_t height, size_t ip_to_free)
{
	int side = LEFT;
	int res = SUCCESS;
		
	assert(node);

	if (0 == height)
	{
		res = CheckAndSetInLeaf(node, FREE, IP_NOT_FOUND);
		
		return (res);
	}

	if (TAKEN == node->data.bit_state)
	{
		if ((NULL == CreateNode(node, LEFT, TAKEN)) ||
			(NULL == CreateNode(node, RIGHT, TAKEN)))
		{
			return (MALLOC_FAIL);
		}
		node->data.bit_state = FREE;
	}

	side = GetSideToMove(node, height, ip_to_free);

	if (NULL == node->sons_side[side])
	{
		res = IP_NOT_FOUND;
			
		return (res);
	}
	
	res = RemovePath(node->sons_side[side], height - 1, ip_to_free);

	if ((SUCCESS == res) && (1 == IsSonLeaf(node)))
	{
		UpdateNodesToLazy(node, side, FREE);
	}
	
	return (res);
}

/******************************************************************************/

/* to print the trie and all its elements (in-order) */
void PrintTrie(const btrie_t *btrie)
{
	assert(btrie);
	
    PrintNodes(GetRoot(btrie));
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

/* to create a new trie.
   if fail - returns NULL, and for sucssess - pointer to the trie */
btrie_t *BinTrieCreate(size_t height)
{
	btrie_t *trie = NULL;

	assert(0 < height);
	
	trie = (btrie_t *)malloc(sizeof(btrie_t));
	if (NULL == trie)
	{
		return (NULL);
	}
	
	if (MALLOC_FAIL == InitTrie(trie, height))
	{
		free(trie);
		
		return (NULL);
	}
	
	return (trie);
}

/******************************************************************************/

/* to destroy a trie and all its elements */
void BinTrieDestroy(btrie_t *btrie)
{
	assert(btrie);

	FreeNodes(GetRoot(btrie));

	free(btrie); btrie = NULL;
}

/******************************************************************************/

/* to count all the free possibilities in the trie */
size_t BinTrieCountPossibilities(const btrie_t *btrie)
{
	assert(btrie);

	return (CountFreeLeaf(GetRoot(btrie), GetHeight(btrie), FREE));
}

/******************************************************************************/

/* to mark a path as TAKEN.
   if the path is already token - returns IP_TAKEN,
   if the allocation failed - returns - MALLOC_FAIL,
   for sucssess - returns SUCCESS */
int BinTrieAskForPath(btrie_t *btrie, size_t ip)
{
	assert(btrie);
	
	return (FindPath(GetRoot(btrie), GetHeight(btrie), ip));
}

/******************************************************************************/

/* to mark a path as FREE.
   if the path is already free - returns IP_NOT_FOUND,
   if the allocation failed - returns - MALLOC_FAIL,
   for sucssess - returns SUCCESS */
int BinTrieRemovePath(btrie_t *btrie, size_t ip_to_free)
{
	assert(btrie);

	return (RemovePath(GetRoot(btrie), GetHeight(btrie), ip_to_free));
}

/******************************************************************************/
