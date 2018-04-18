# SCHEDULER
Scheduler library in C language for embedded software.


## Description

Cadenced by an interruption timer, Software or Hardware, `scheduler` allows to
execute multiple tasks, **periodically** or **one-shot**.

The execution of the task won't block the main program.

The resolution of the scheduler (min. period) depends on the capabilities
of the timer.


## Dependencies

`scheduler.c` includes the modules [traces](https://github.com/cruzl/traces),
and [linked-list](https://github.com/cruzl/linked-list).


## API

# Functions:
* SCHEDULER_Init
* SCHEDULER_Deinit
* SCHEDULER_MonitoringLoop
* SCHEDULER_Task_Register
* SCHEDULER_Task_Unregister
* SCHEDULER_Task_Start
* SCHEDULER_Task_Stop
* SCHEDULER_Task_Reinit
* SCHEDULER_Task_Restart
* SCHEDULER_Task_GetStatus

# Variable types:
* SCHEDULER_TASK_CB_t
* SCHEDULER_RESULT_t
* SCHEDULER_TASK_TYPE_t
* SCHEDULER_TASK_STATUS_t
* SCHEDULER_TASK_t


## Usage

### Example 1

Program [demo-linux.c](demo-linux.c).

Compiling and running the example program:
```console
foo@bar:~$ gcc -I $(pwd) -c src/scheduler.c -o scheduler.o
foo@bar:~$ gcc -c demo-linux.c -o demo-linux.o
foo@bar:~$ gcc scheduler.o demo-linux.o -o demo-linux
foo@bar:~$ rm scheduler.o demo-linux.o
foo@bar:~$ ./demo-linux
```

Result:
```
Demo task callback message every second.
Demo task callback message every second.
Demo task callback message every second.
Demo task callback message every second.
Demo task callback message every second.
```


## Todo

Nothing for the moment.


## License

**SCHEDULER** is licensed under the **MIT license**. See [LICENSE](LICENSE) for further information.
