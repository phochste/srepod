/* 
 * Authors: Henry Jerez <hjerez@lanl.gov>
 *
 * $Id: stackli.c,v 1.1.1.1 2003/06/25 17:57:05 hochstenbach Exp $
 * 
 */
#include "stackli.h"
#include "fatal.h"
#include <stdlib.h>

struct Node {
        ElementType Element;
        PtrToNode   Next;
};

int  	IsEmpty(Stack S ) {
	return S->Next == NULL;
}

Stack CreateStack( void ) {
	Stack S;

        S = (Stack) malloc( sizeof( struct Node ) );

        if ( S == NULL )  FatalError( "Out of space!!!" );

	S->Element = NULL;
        S->Next = NULL;

	MakeEmpty( S );
	return S;
}

void    MakeEmpty( Stack S ) {
	if( S == NULL )
	          Error( "Must use CreateStack first" );
	while( !IsEmpty( S ) ) Pop( S );
}

void    DisposeStack( Stack S ) {
	MakeEmpty( S );
        free( S );
}

void 	Push( ElementType X, Stack S ) {
        PtrToNode  TmpCell; 

        TmpCell = malloc( sizeof( struct Node ) );

	if (TmpCell == NULL)
            FatalError( "Out of space!!!" );

	TmpCell->Element = (ElementType) malloc(strlen(X)+1);	
        strcpy(TmpCell->Element,X);
	
	TmpCell->Next = S->Next;
	S->Next = TmpCell;
}
	        
ElementType Top( Stack S ) {
	if( !IsEmpty( S ) )
		return S->Next->Element;
	return NULL; 
}

void 	Pop( Stack S ) {
	PtrToNode FirstCell;

        if( IsEmpty(S) )
                return;

	FirstCell = S->Next;
	S->Next   = S->Next->Next;
	free(FirstCell->Element);
	free(FirstCell);
}

int 	InStack(Stack S, const char *str) {
	PtrToNode TmpCell; 

	TmpCell = S;
	while(TmpCell->Next != NULL) {	

		if (TmpCell->Element != NULL && strcmp(TmpCell->Element, str) == 0) {
			return 1;
		}

		TmpCell = TmpCell->Next;
	}			
	return 0;
}
