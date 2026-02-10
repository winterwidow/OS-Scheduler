#ifndef ALGORITHMS_H
#define ALGORITHMS_H

// define a structure to represent a process
typedef struct
{
    int pid; // process ID
    int arrival_time; // time at which the process arrives in the ready queue
    int burst_time; // total time required by the process on the CPU
    int priority; // priority of the process

    int start_time; // time at which the process starts execution
    int finish_time; // time at which the process finishes execution
    int waiting_time; // total time the process has been waiting in the ready queue
    int turnaround_time; // total time taken from arrival to completion (finish_time - arrival_time)
} Process;

// for ROUND ROBIN, we need to track segments of execution for the Gantt chart
typedef struct
{
    int pid;
    int start;
    int end;
} Segment;

void fcfs(Process p[], int n); // first-come first-served
void sjf_non_preemptive(Process p[], int n); // shortest job first (non-preemptive)
void priority_non_preemptive(Process p[], int n); // priority scheduling (non-preemptive)
void round_robin(Process p[], int n, int quantum); // round robin scheduling

#endif