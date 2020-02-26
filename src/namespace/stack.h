/* STACK.H
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

#define MAX_STACK 4096

typedef struct stack {
  int *stack;
  int TOS;
  int BOS;
} STACK;

STACK * create_stack();
int pop_tos_stack(STACK *s);
int pop_bos_stack(STACK *s);
int push_stack(STACK *s, int c);
int destroy_stack(STACK *s);
