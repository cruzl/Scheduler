#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "src/scheduler.h"

// Timer period = 1000us = 1ms
#define TIMER_PERIOD_US     1000UL

void
timer_callback ( int sig );

void
demo_task ( void * arg );

void
kill_demo ( int sig );

void
main ( void )
{
    static struct sigaction kill_handler;
    static struct itimerval it_timer;

    memset( &kill_handler , 0 , sizeof( kill_handler ) );
    memset( &it_timer     , 0 , sizeof( it_timer     ) );

    // Initializing the scheduler
    SCHEDULER_Init( TIMER_PERIOD_US );

    // Initializing timer period
    it_timer.it_value.tv_usec = it_timer.it_interval.tv_usec = TIMER_PERIOD_US;

    // Defining the callback function
    if ( signal( SIGALRM , timer_callback ) == SIG_ERR )
    {
        printf( "Could not catch the signal" );
        return;
    }

    // Initializing timer
    if ( setitimer( ITIMER_REAL , &it_timer , NULL ) < 0 )
    {
        printf( "Could not initialize the timer" );
        return;
    }

    // Catching CTRL+C signal
    kill_handler.sa_handler = kill_demo;
    kill_handler.sa_flags   = 0;

    sigemptyset( &kill_handler.sa_mask );
    sigaction( SIGINT , &kill_handler , NULL );

    // Demo task parameters
    static SCHEDULER_TASK_t task =
    {
        .callback   = demo_task ,
        .arg        = NULL ,
        .type       = SCHEDULER_TASK_TYPE_PERIODIC ,
        .timeout_us = 1000000   // 1 second
    };

    // Registering and starting the task
    SCHEDULER_Task_Register( &task );
    SCHEDULER_Task_Start( &task );

    while ( 1 ) {};
}

void
timer_callback ( int sig )
{
    SCHEDULER_MonitoringLoop();
}

void
demo_task ( void * arg )
{
    printf( "Demo task callback message every second.\r\n" );
}

void
kill_demo ( int sig )
{
    printf( "Caught CTRL+C\r\n" );

    SCHEDULER_Deinit();

    exit( 1 );
}
