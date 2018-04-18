/*
 * The MIT License
 *
 * Copyright (c) 2018 Luis CRUZ
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// -----------------------------------------------
//     INCLUDE
// -----------------------------------------------

#include "linked_list.h"
#include "traces.h"



// -----------------------------------------------
//     PRIVATE PROTOTYPES
// -----------------------------------------------

void
FREE ( void ** ptr );

NODE_t *
linked_list_CreateNode ( void * data );

NODE_t *
linked_list_FindNode ( LIST_t * list ,
                       void   * data );



//-----------------------------------------------
//     PUBLIC FUNCTIONS
// -----------------------------------------------

LIST_t *
LINKED_LIST_CreateEmptyList ( void )
{
    LIST_t * list = malloc( sizeof( LIST_t ) );

    if ( list == NULL )
    {
        ERROR( "The list could not be allocated" );

        return NULL;
    }

    list->head = NULL;

    TRACE_P( "List allocated successfully @ 0x%p" , list );

    return list;
}

LINKED_LIST_RESULT_t
LINKED_LIST_AddNode ( LIST_t * list ,
                      void   * data )
{
    if ( ( list == NULL ) || ( data == NULL ) )
    {
        ERROR_P( "Null parameter: list = 0x%p , data = 0x%p" , list , data );

        return LINKED_LIST_RESULT_ERROR_NULL_PARAM;
    }
    else
    if ( linked_list_FindNode( list , data ) != NULL )
    {
        ERROR_P( "The node whose data = 0x%p has already been added" , data );

        return LINKED_LIST_RESULT_ERROR_NODE_ALREADY_IN_LIST;
    }

    NODE_t * new_node = linked_list_CreateNode( data );

    if ( new_node != NULL )
    {
        if ( list->head == NULL )
        {
            list->head = new_node;

            TRACE( "Node added @ list head" );
        }
        else
        {
            NODE_t * last_node = list->head;

            while ( last_node->next != NULL )
            {
                last_node = last_node->next;
            }

            last_node->next = new_node;

            TRACE( "Node added @ the end of the list" );
        }

        return LINKED_LIST_RESULT_SUCCESS;
    }
    else
    {
        ERROR( "The node could not be allocated" );

        return LINKED_LIST_RESULT_ERROR_MEMORY_ALLOCATION;
    }
}

LINKED_LIST_RESULT_t
LINKED_LIST_DeleteNode ( LIST_t * list ,
                         void   * data )
{
    if ( ( list == NULL ) || ( data == NULL ) )
    {
        ERROR_P( "Null parameter: list = 0x%p , data = 0x%p" , list , data );

        return LINKED_LIST_RESULT_ERROR_NULL_PARAM;
    }

    if ( linked_list_FindNode( list , data ) != NULL )
    {
        NODE_t * current  = list->head;
        NODE_t * previous = current;

        while ( current != NULL )
        {
            if ( current->data == data )
            {
                previous->next = current->next;

                if ( current == list->head )
                {
                    list->head = current->next;
                }

                FREE( ( void ** )&current );

                TRACE_P( "The node whose data = 0x%p was removed from list @ 0x%p" , data , list );

                return LINKED_LIST_RESULT_SUCCESS;
            }

            previous = current;
            current  = current->next;
        }
    }

    ERROR_P( "The node whose data = 0x%p was not found in list @ 0x%p" , data , list );

    return LINKED_LIST_RESULT_ERROR_NODE_NOT_IN_LIST;
}

NODE_t *
LINKED_LIST_FindNode ( LIST_t * list ,
                       void   * data )
{
    return linked_list_FindNode( list , data );
}

LINKED_LIST_RESULT_t
LINKED_LIST_DisplayList ( LIST_t * list )
{
    if ( list == NULL )
    {
        ERROR( "Null parameter" );

        return LINKED_LIST_RESULT_ERROR_NULL_PARAM;
    }
    else
    if ( list->head == NULL )
    {
        ERROR_P( "Empty list @ 0x%p" , list );

        return LINKED_LIST_RESULT_ERROR_EMPTY_LIST;
    }

    TRACE_P( "Displaying list @ 0x%p" , list );

    NODE_t * current = list->head;

    while ( current->next != NULL )
    {
        TRACE_P( "  - Node = 0x%p , data = 0x%p" , current , current->data );

        current = current->next;
    }

    TRACE_P( "  - Node = 0x%p , data = 0x%p" , current , current->data );
    TRACE( "End of list" );

    return LINKED_LIST_RESULT_SUCCESS;
}

LINKED_LIST_RESULT_t
LINKED_LIST_ReverseList ( LIST_t * list )
{
    if ( list == NULL )
    {
        ERROR( "Null parameter" );

        return LINKED_LIST_RESULT_ERROR_NULL_PARAM;
    }

    NODE_t * reversed = NULL;
    NODE_t * current  = list->head;
    NODE_t * temp     = NULL;

    while ( current != NULL )
    {
        temp       = current;
        current    = current->next;
        temp->next = reversed;
        reversed   = temp;
    }

    list->head = reversed;

    TRACE_P( "List @ 0x%p reversed successfully" , list );

    return LINKED_LIST_RESULT_SUCCESS;
}

LINKED_LIST_RESULT_t
LINKED_LIST_DestroyList ( LIST_t * list )
{
    if ( list == NULL )
    {
        ERROR( "Null parameter" );

        return LINKED_LIST_RESULT_ERROR_NULL_PARAM;
    }

    if ( list->head != NULL )
    {
        NODE_t * current = list->head;
        NODE_t * next    = current;

        while ( current != NULL )
        {
            next = current->next;

            TRACE_P( "Freeing node @ 0x%p" , current );

            FREE( ( void ** )&current );

            current = next;
        }
    }

    TRACE_P( "Freeing list @ 0x%p" , list );

    FREE( ( void ** )&list );

    return LINKED_LIST_RESULT_SUCCESS;
}



// -----------------------------------------------
//     PRIVATE FUNCTIONS
// -----------------------------------------------

void
FREE ( void ** ptr )
{
    TRACE_P( "Freeing pointer @ 0x%p" , *ptr );

    free( *ptr );

    *ptr = NULL;
}

NODE_t *
linked_list_CreateNode ( void * data )
{
    TRACE_P( "Allocating node, data = 0x%p" , data );

    NODE_t * node = malloc( sizeof( NODE_t ) );

    if ( node == NULL )
    {
        ERROR( "The node could not be allocated" );

        return NULL;
    }

    node->data = data;
    node->next = NULL;

    TRACE_P( "Node allocated successfully @ 0x%p" , node );

    return node;
}

NODE_t *
linked_list_FindNode ( LIST_t * list ,
                       void   * data )
{
    TRACE_P( "Searching the node whose data = 0x%p in list @ 0x%p" , data , list );

    NODE_t * node = list->head;

    while ( node != NULL )
    {
        if ( node->data == data )
        {
            TRACE_P( "Node found @ 0x%p" , node );

            return node;
        }

        node = node->next;
    }

    TRACE( "Node not found" );

    return NULL;
}



// End of file
