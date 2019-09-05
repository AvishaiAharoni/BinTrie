#include <stdio.h>

#include "bintrie.h"

/******************************************************************************/

#define HEIGHT 4

int CreateDestryTest();
int CountTest();
int AskForPathTest();
int RemovePathTest();

/******************************************************************************/
int main()
{
	int res = 0;
	
	res = CreateDestryTest();
	if (0 != res)
	{
		printf("error in line %d\n", res);
		
		return (res);
	}
	
	res = CountTest();
	if (0 != res)
	{
		printf("error in line %d\n", res);
		
		return (res);
	}
	
	res = AskForPathTest();
	if (0 != res)
	{
		printf("error in line %d\n", res);
		
		return (res);
	}
	
	res = RemovePathTest();
	if (0 != res)
	{
		printf("error in line %d\n", res);
		
		return (res);
	}
	
	
	puts("good!");
	
	return (0);
}

/******************************************************************************/

static size_t PowerOfTwo(int exp)
{
	return (1 << exp);
}

/******************************************************************************/

int CreateDestryTest()
{
	btrie_t *b = BinTrieCreate(HEIGHT);
	
	BinTrieDestroy(b);
	
	return (0);
}

/******************************************************************************/

int CountTest()
{
	btrie_t *b = BinTrieCreate(HEIGHT);
	
	if (16 != BinTrieCountPossibilities(b))
	{
		printf("error in count. need to be 16, but %lu found\n",
												BinTrieCountPossibilities(b));
		
		return (__LINE__);
	}
	
	BinTrieDestroy(b);
	
	return (0);
}

/******************************************************************************/

int AskForPathTest()
{
	btrie_t *b = BinTrieCreate(HEIGHT);
	size_t i = 0;
	size_t pow_height = PowerOfTwo(HEIGHT);
	
	for (i = 0; i < pow_height; ++i)
	{
		if (SUCCESS != BinTrieAskForPath(b, i))
		{
			printf("problem in good ask, i = %lu\n", i);
			return (__LINE__);
		}
		PrintTrie(b);

		if (IP_TAKEN != BinTrieAskForPath(b, i))
		{
			printf("problem in bad ask, i = %lu\n", i);
			return (__LINE__);
		}
		
		if (pow_height - i - 1 != BinTrieCountPossibilities(b))
		{
			printf("error in count. need to be %lu, but %lu found\n", pow_height- i -1, 
													BinTrieCountPossibilities(b));
			return (__LINE__);
		}	
	}

	
	BinTrieDestroy(b);

	return (0);
}

/******************************************************************************/

int RemovePathTest()
{
	btrie_t *b = BinTrieCreate(HEIGHT);
	size_t i = 0;
	size_t pow_height = PowerOfTwo(HEIGHT);

	for (i = 0; i < pow_height; ++i)
	{
		if (SUCCESS != BinTrieAskForPath(b, i))
		{
			printf("problem in good ask, i = %lu\n", i);
			return (__LINE__);
		}
	}
PrintTrie(b);

	for (i = 0; i < pow_height; ++i)
	{
		if (SUCCESS != BinTrieRemovePath(b, i))
		{
			printf("problem in good remove, i = %lu\n", i);
			return (__LINE__);
		}
		
		printf("remove %lu\n", i);
		if (IP_NOT_FOUND != BinTrieRemovePath(b, i))
		{
			printf("problem in bad remove, i = %lu\n", i);
			return (__LINE__);
		}
		
		if (i + 1 != BinTrieCountPossibilities(b))
		{
			printf("error in count. need to be %lu, but %lu found\n", i + 1, 
													BinTrieCountPossibilities(b));
			return (__LINE__);
		}
PrintTrie(b);
	}
	
	BinTrieDestroy(b);

	return (0);
}

