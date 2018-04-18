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

#include "scheduler.h"
#include "linked_list.h"
#include "traces.h"



// -----------------------------------------------
//     GLOBAL VARIABLES
// -----------------------------------------------

static uint32_t   loop_period_us = 0;
static LIST_t   * task_list      = NULL;



//-----------------------------------------------
//     PUBLIC FUNCTIONS
// -----------------------------------------------

SCHEDULER_RESULT_t
SCHEDULER_Init ( uint32_t period_us )
{
    task_list = LINKED_LIST_CreateEmptyList();

    if ( task_list == NULL )
    {
        ERROR( "The list could not be allocated" );

        return SCHEDULER_RESULT_ERROR_MEMORY_ALLOCATION;
    }

    loop_period_us = period_us;

    TRACE_P( "List allocated successfully @ 0x%p" , task_list );

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_RESULT_t
SCHEDULER_Deinit ( void )
{
    if ( task_list == NULL )
    {
        ERROR( "The list has not been initialized yet" );

        return SCHEDULER_RESULT_ERROR_SCHEDULER_NOT_INIT;
    }

    LINKED_LIST_DestroyList( task_list );

    TRACE( "List destroyed successfully" );

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_RESULT_t
SCHEDULER_MonitoringLoop ( void )
{
    if ( task_list == NULL )
    {
        ERROR( "Null parameter" );

        return SCHEDULER_RESULT_ERROR_SCHEDULER_NOT_INIT;
    }

    NODE_t * task = task_list->head;

    while ( task != NULL )
    {
        if ( task->data != NULL )
        {
            SCHEDULER_TASK_t * data = ( SCHEDULER_TASK_t * )task->data;

            if ( data->status == SCHEDULER_TASK_STATUS_STARTED )
            {
                data->count += loop_period_us;

                if ( data->count >= data->timeout_us )
                {
                    data->callback( data->arg );

                    TRACE_P( "Task @ 0x%p whose callback function = 0x%p and arguments = 0x%p called successfully" , task , data->callback , data->arg );

                    if ( data->type == SCHEDULER_TASK_TYPE_PERIODIC )
                    {
                        data->count = 0;

                        TRACE_P( "Restarted periodic task @ 0x%p" , task );
                    }
                    else
                    {
                        data->status = SCHEDULER_TASK_STATUS_STOPPED;

                        TRACE_P( "One-shot task @ 0x%p finished successfully" , task );
                    }
                }
            }
        }

        task = task->next;
    }

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_RESULT_t
SCHEDULER_Task_Register ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        ERROR( "Null parameter" );

        return SCHEDULER_RESULT_ERROR_NULL_PARAM;
    }

    if ( LINKED_LIST_AddNode( task_list , ( void * )task ) == LINKED_LIST_RESULT_SUCCESS )
    {
        task->status = SCHEDULER_TASK_STATUS_STOPPED;
        task->count  = 0;

        TRACE_P( "Task @ 0x%p was registered successfully" , task );

        return SCHEDULER_RESULT_SUCCESS;
    }

    ERROR_P( "Task @ 0x%p has already been registered" , task );

    return SCHEDULER_RESULT_ERROR_TASK_ALREADY_REGISTERED;
}

SCHEDULER_RESULT_t
SCHEDULER_Task_Unregister ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        ERROR( "Null parameter" );

        return SCHEDULER_RESULT_ERROR_NULL_PARAM;
    }

    if ( LINKED_LIST_DeleteNode( task_list , ( void * )task ) == LINKED_LIST_RESULT_SUCCESS )
    {
        TRACE_P( "Task @ 0x%p was unregistered successfully" , task );

        return SCHEDULER_RESULT_SUCCESS;
    }

    ERROR_P( "Task @ 0x%p has not been registered yet" , task );

    return SCHEDULER_RESULT_ERROR_UNREGISTERED_TASK;
}

SCHEDULER_RESULT_t
SCHEDULER_Task_Start ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        ERROR( "Null parameter" );

        return SCHEDULER_RESULT_ERROR_NULL_PARAM;
    }
    else
    if ( LINKED_LIST_FindNode( task_list , ( void * )task ) == NULL )
    {
        ERROR_P( "Task @ 0x%p has not been registered yet" , task );

        return SCHEDULER_RESULT_ERROR_UNREGISTERED_TASK;
    }
    else
    if ( task->status == SCHEDULER_TASK_STATUS_STARTED )
    {
        ERROR_P( "Task @ 0x%p has already been started" , task );

        return SCHEDULER_RESULT_ERROR_TASK_ALREADY_STARTED;
    }

    task->status = SCHEDULER_TASK_STATUS_STARTED;

    TRACE_P( "Task @ 0x%p was started successfully" , task );

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_RESULT_t
SCHEDULER_Task_Stop ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        ERROR( "Null parameter" );

        return SCHEDULER_RESULT_ERROR_NULL_PARAM;
    }
    else
    if ( LINKED_LIST_FindNode( task_list , ( void * )task ) == NULL )
    {
        ERROR_P( "Task @ 0x%p has not been registered yet" , task );

        return SCHEDULER_RESULT_ERROR_UNREGISTERED_TASK;
    }
    else
    if ( task->status == SCHEDULER_TASK_STATUS_STOPPED )
    {
        ERROR_P( "Task @ 0x%p has already been stopped" , task );

        return SCHEDULER_RESULT_ERROR_TASK_ALREADY_STOPPED;
    }

    task->status = SCHEDULER_TASK_STATUS_STOPPED;

    TRACE_P( "Task @ 0x%p was stopped successfully" , task );

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_RESULT_t
SCHEDULER_Task_Reinit ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        ERROR( "Null parameter" );

        return SCHEDULER_RESULT_ERROR_NULL_PARAM;
    }
    else
    if ( LINKED_LIST_FindNode( task_list , ( void * )task ) == NULL )
    {
        ERROR_P( "Task @ 0x%p has not been registered yet" , task );

        return SCHEDULER_RESULT_ERROR_UNREGISTERED_TASK;
    }

    task->status = SCHEDULER_TASK_STATUS_STOPPED;
    task->count  = 0;

    TRACE_P( "Task @ 0x%p was reinitialized successfully" , task );

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_RESULT_t
SCHEDULER_Task_Restart ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        ERROR( "Null parameter" );

        return SCHEDULER_RESULT_ERROR_NULL_PARAM;
    }
    else
    if ( LINKED_LIST_FindNode( task_list , ( void * )task ) == NULL )
    {
        ERROR_P( "Task @ 0x%p has not been registered yet" , task );

        return SCHEDULER_RESULT_ERROR_UNREGISTERED_TASK;
    }

    task->status = SCHEDULER_TASK_STATUS_STARTED;
    task->count  = 0;

    TRACE_P( "Task @ 0x%p was restarted successfully" , task );

    return SCHEDULER_RESULT_SUCCESS;
}

SCHEDULER_TASK_STATUS_t
SCHEDULER_Task_GetStatus ( SCHEDULER_TASK_t * task )
{
    if ( task == NULL )
    {
        ERROR_P( "Task @ 0x%p has not been registered yet" , task );

        return SCHEDULER_TASK_STATUS_UNKNOWN;
    }

    TRACE_P( "The status of the task @ 0x%p is 0x%X" , task , task->status );

    return task->status;
}



// End of file
