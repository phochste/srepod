/*
 * $Id: hash.h,v 1.1.1.1 2003/06/25 17:57:00 hochstenbach Exp $
 */
#ifndef	__hash_h
#define __hash_h

#include <stdlib.h>
#include <string.h>

/* Hash implementation. Inspiration from: 
 *
 *  The C Programming Language
 *  Brian W. Kerbughan
 *  Dennis M. Ritchie
 *  0131101633
 *
 *  Pages 135-136
 */

struct	nlist {
	char	*name;
	char	*def;
	struct nlist *next;
};

struct	h_struct {
	int		bucket;
	struct	nlist  	*ptr; 
	struct	nlist	**tree;
};

typedef struct	nlist 	*HASH_ITEM;
typedef struct	h_struct *HASH;

#define	HASHSIZE	100

/* Create a new HASH in memory. This function returns a pointer to a 
 * memory structure that has to be passed to all other hash functions.
 * Returns NULL on error
 */
HASH	hash_new();

/* Frees memory allocated by creating and populating the hash 
 */
void	hash_free(HASH h);

/* Add a new value pair the the hash. Existing names will be overwritten.
 * Returns 0 on success -1 on error.
 */
int	hash_add(HASH h, const char *name, const char *value);

/* Returns a pointer to the value of the hash item with this name.
 * Returns NULL on errror.
 */
char 	*hash_val(HASH h, const char *name);

/* Call this function before you scanning the hash contents with the
 * hash_next_key or hash_next_val functions.
 */
void	hash_next_reset(HASH h);

/* Returns a pointer to the next key in the hash.
 * Returns NULL when no more keys are found.
 */
char	*hash_next_key(HASH h);

/* Returns a pointer to the next value in the hash.
 * Returns NULL when no more values are found.
 */
char	*hash_next_val(HASH h);

/* Deletes a key from the hash.
 */
void	hash_del(HASH h, const char *name);

#endif
