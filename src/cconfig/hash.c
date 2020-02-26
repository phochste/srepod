/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: hash.c,v 1.2 2006/12/19 08:33:12 hochstenbach Exp $
 * 
 */
#include "hash.h"

static int hash(const char *s);
static char *strsave(const char *s);
static HASH_ITEM lookup(HASH h, const char *name);

static int hash(const char *s) {
	int hashval;

	for (hashval = 0 ; *s != '\0' ; )
		hashval += *s++;

	return(hashval % HASHSIZE);
}

static char *strsave(const char *s) {
	int  l;
	char *p;

	l = strlen(s);

	p = (char *) malloc(sizeof(char)*(l + 1));

	if (p == NULL)
		return(NULL);

	memcpy(p, s, l+1);

	return(p);
}

static HASH_ITEM lookup(HASH h, const char *name) {
	HASH_ITEM np;

	for (np = h->tree[hash(name)] ; np != NULL ; np = np->next) {
		if (strcmp(name, np->name) == 0) {
			return(np);
		}
	}

	return(NULL);
}

/* Create a new HASH in memory. This function returns a pointer to a 
 * memory structure that has to be passed to all other hash functions.
 * Returns NULL on error
 */
HASH	hash_new() {
	HASH	h;
	int i;

	h = (struct h_struct*) malloc(sizeof(struct h_struct));

	if (h == NULL) {
		return(NULL);
	}

	h->tree = (struct nlist**) malloc(sizeof(struct nlist) * HASHSIZE);

	if (h->tree == NULL) {
		free(h);
		return(NULL);
	}

	for (i = 0 ; i < HASHSIZE ; i++) {
		h->tree[i] = NULL;
	}

	h->bucket = -1;
	h->ptr    = h->tree[0];

	return h;
}

/* Frees memory allocated by creating and populating the hash 
 */
void	hash_free(HASH h) {
	HASH_ITEM np,jnk;
	int i;

	for (i = 0 ; i < HASHSIZE ; i++) {
		np = h->tree[i];
		while (np != NULL) {
			jnk = np;
			np = np->next;
			free(jnk->name);
			free(jnk->def);
			free(jnk);
		}
	}

	free(h->tree);

	free(h);
}

/* Add a new value pair the the hash. Existing names will be overwritten.
 * Returns 0 on success -1 on error.
 */
int	hash_add(HASH h, const char *name, const char *def) {
	HASH_ITEM np;
	int hashval;

	if ((np = lookup(h, name)) == NULL) {
		np = (HASH_ITEM) malloc(sizeof(struct nlist));
		
		if (np == NULL) 
			return(-1);

		if ((np->name = strsave(name)) == NULL)
			return(-1);

		hashval = hash(np->name);

		np->next = h->tree[hashval];
		h->tree[hashval] = np;
	} else {
		free(np->def);
	}

	if ((np->def = strsave(def)) == NULL) 
		return(-1);

	return(0);
}

/* Returns a pointer to the value of the hash item with this name.
 * Returns NULL on errror.
 */
char	*hash_val(HASH h, const char *name) {
	HASH_ITEM np;

	np = lookup(h, name);

	if (np == NULL) {
		return NULL;
	}

	return np->def;
}

/* Deletes a key from the hash.
 */
void	hash_del(HASH h, const char *name) {
	HASH_ITEM np,np_prev;
	int	hashval;
	
	np_prev = NULL;

	hashval = hash(name);

	for (np = h->tree[hashval] ; np != NULL ; np = np->next) {
		if (strcmp(name, np->name) == 0) {
			if (np_prev == NULL) {
				h->tree[hashval] = np->next;
			}
			else {
			   	np_prev->next = np->next;
			}

			free(np->name);
			free(np->def);
			free(np);

			return;
		}
		np_prev = np;
	}

	return;
}

/* Call this function before you scanning the hash contents with the
 * hash_next_key or hash_next_val functions.
 */
void	hash_next_reset(HASH h) {
	h->bucket = -1;
}

/* Returns a pointer to the next key in the hash.
 * Returns NULL when no more keys are found.
 */
char	*hash_next_key(HASH h) {
	int   f;
	int   bucket;
	HASH_ITEM np;

	f = h->bucket >= 0 ? 1 : 0;

	for (bucket = f ? h->bucket : 0 ; bucket < HASHSIZE; bucket++) {
		
		for (np = f ? h->ptr : h->tree[bucket] ; np != NULL ; np = np->next) {
			h->bucket = bucket;
			h->ptr    = np->next;
			return(np->name);
		}

		f = 0;
	}

	return(NULL);
}

/* Returns a pointer to the next value in the hash.
 * Returns NULL when no more values are found.
 */
char	*hash_next_val(HASH h) {
	int   f;
	int   bucket;
	HASH_ITEM np;

	f = h->bucket >= 0 ? 1 : 0;

	for (bucket = f ? h->bucket : 0 ; bucket < HASHSIZE; bucket++) {
		
		for (np = f ? h->ptr : h->tree[bucket] ; np != NULL ; np = np->next) {
			h->bucket = bucket;
			h->ptr    = np->next;
			return(np->def);
		}

		f = 0;
	}

	return(NULL);
}
