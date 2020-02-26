/* STACK.C
 * Copyright (C) 1997, 1998 Stephen Rust <steve@tp.org>
 *
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2, or (at your option) 
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define DEFAULT_SIZE 256

STACK * create_stack(int size)
{
  STACK * s;

  if (!size)
	size = DEFAULT_SIZE;

  s = (STACK *) malloc(sizeof(STACK));

  if (s == NULL) {
    	fprintf(stderr, "Error allocating memory.  Out of memory.\n\r");
    	exit(1);
  } 

  s->stack = (int *) malloc(sizeof(int *) * size);

  if (s->stack == NULL) {
	fprintf(stderr, "Out of memory.\r\n");
	exit(1);
  }
  s->TOS = 0;
  s->BOS = 0;

  return s;
}

int pop_tos_stack(STACK *s) 
{
  if (!s || s->TOS < 1)
	return EOF;

  return(s->stack[--(s->TOS)]);
}

int pop_bos_stack(STACK *s)
{
  if (!s || s->BOS == s->TOS)
	return EOF;

  return(s->stack[(s->BOS)++]);
}

int push_stack(STACK *s, int c)
{
  if (!s)
	return EOF;

  if (s->TOS > MAX_STACK-1)
    	return 1;  

  s->stack[(s->TOS)++] = c;

  return 0;
}

int destroy_stack(STACK *s)
{
  if (!s)
	return EOF;

  free(s);
  return 0;
}

