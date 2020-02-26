/* 
 * Authors: Henry Jerez <hjerez@lanl.gov>
 *
 * $Id: stackli.h,v 1.1.1.1 2003/06/25 17:57:05 hochstenbach Exp $
 * 
 */
#ifndef _Stack_h
#define _Stack_h
typedef char *ElementType;

struct Node;
typedef struct Node *PtrToNode;
typedef PtrToNode Stack;

int IsEmpty( Stack S );
Stack CreateStack( void );
void DisposeStack( Stack S );
void MakeEmpty( Stack S );
void Push( ElementType X, Stack S );
ElementType Top( Stack S );
void Pop( Stack S );
int  InStack(Stack S, const char *str);

#endif  /* _Stack_h */
