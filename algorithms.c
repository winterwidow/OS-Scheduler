#include <stdio.h>
#include <string.h> // for memcpy
#include <limits.h> // for INT_MAX
#include "algorithms.h"

static void print_results(const char *name, Process p[], int n)
{
    double total_wait = 0.0, total_tat = 0.0;

    printf("\n=== %s ===\n", name);
    printf("PID\tAT\tBT\tPR\tST\tFT\tWT\tTAT\n");
    for (int i = 0; i < n; i++)
    {
        total_wait += p[i].waiting_time;
        total_tat += p[i].turnaround_time;
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid,
               p[i].arrival_time,
               p[i].burst_time,
               p[i].priority,
               p[i].start_time,
               p[i].finish_time,
               p[i].waiting_time,
               p[i].turnaround_time);
    }

    printf("Average waiting time    : %.2f\n", total_wait / n);
    printf("Average turnaround time : %.2f\n", total_tat / n);
}

static void copy_processes(Process dest[], Process src[], int n)
{
    memcpy(dest, src, n * sizeof(Process));
}

// Gantt Chart for visualization

static void print_rr_gantt(Segment segs[], int count)
{
    if (count == 0)
        return;

    // printf("\nGantt Chart:\n");

    // Top bar
    printf(" ");
    for (int i = 0; i < count; i++)
    {
        int width = segs[i].end - segs[i].start;
        for (int j = 0; j < width; j++)
            printf("--");
        printf(" ");
    }
    printf("\n|");

    // Process labels
    for (int i = 0; i < count; i++)
    {
        int width = segs[i].end - segs[i].start;
        for (int j = 0; j < width - 1; j++)
            printf(" ");
        printf("P%d", segs[i].pid);
        for (int j = 0; j < width - 1; j++)
            printf(" ");
        printf("|");
    }

    printf("\n ");

    // Bottom bar
    for (int i = 0; i < count; i++)
    {
        int width = segs[i].end - segs[i].start;
        for (int j = 0; j < width; j++)
            printf("--");
        printf(" ");
    }

    // Time markers
    printf("\n%d", segs[0].start);
    for (int i = 0; i < count; i++)
    {
        int width = segs[i].end - segs[i].start;
        for (int j = 0; j < width; j++)
            printf("  ");
        printf("%d", segs[i].end);
    }

    printf("\n");
}

static void print_gantt_chart(Process p[], int n)
{
    // printf("\nGantt Chart:\n");

    // Top bar
    printf(" ");
    for (int i = 0; i < n; i++)
    {
        int width = p[i].finish_time - p[i].start_time;
        for (int j = 0; j < width; j++)
            printf("--");
        printf(" ");
    }
    printf("\n|");

    // Process labels
    for (int i = 0; i < n; i++)
    {
        int width = p[i].finish_time - p[i].start_time;
        for (int j = 0; j < width - 1; j++)
            printf(" ");
        printf("P%d", p[i].pid);
        for (int j = 0; j < width - 1; j++)
            printf(" ");
        printf("|");
    }

    printf("\n ");

    // Bottom bar
    for (int i = 0; i < n; i++)
    {
        int width = p[i].finish_time - p[i].start_time;
        for (int j = 0; j < width; j++)
            printf("--");
        printf(" ");
    }

    // Time markers
    printf("\n%d", p[0].start_time);
    for (int i = 0; i < n; i++)
    {
        int width = p[i].finish_time - p[i].start_time;
        for (int j = 0; j < width; j++)
            printf("  ");
        printf("%d", p[i].finish_time);
    }

    printf("\n");
}

// helper: sort by start_time for correct Gantt order
static void sort_by_start(Process p[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (p[j].start_time > p[j + 1].start_time)
            {
                Process tmp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = tmp;
            }
        }
    }
}

// First Come First Serve

void fcfs(Process orig[], int n)
{
    Process p[100];
    copy_processes(p, orig, n);

    // Sort by arrival time - using bubble sort
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (p[j].arrival_time > p[j + 1].arrival_time)
            {
                Process tmp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = tmp;
            }
        }
    }

    int current_time = 0;
    for (int i = 0; i < n; i++)
    {
        if (current_time < p[i].arrival_time)
            current_time = p[i].arrival_time;

        p[i].start_time = current_time;
        p[i].finish_time = p[i].start_time + p[i].burst_time;
        p[i].turnaround_time = p[i].finish_time - p[i].arrival_time;
        p[i].waiting_time = p[i].start_time - p[i].arrival_time;

        current_time = p[i].finish_time;
    }

    printf("First Come First Serve Gantt Chart:\n");
    print_gantt_chart(p, n);
    print_results("FCFS", p, n);
}

// Shortest Job First

void sjf_non_preemptive(Process orig[], int n)
{
    Process p[100];
    copy_processes(p, orig, n);

    int completed[100] = {0};
    int completed_count = 0;
    int current_time = 0;

    while (completed_count < n)
    {
        int idx = -1;
        int min_burst = INT_MAX;

        // Pick shortest job among arrived and not completed
        for (int i = 0; i < n; i++)
        {
            if (!completed[i] && p[i].arrival_time <= current_time)
            {
                if (p[i].burst_time < min_burst)
                {
                    min_burst = p[i].burst_time;
                    idx = i;
                }
            }
        }

        // If no process has arrived yet
        if (idx == -1)
        {
            int next_arrival = INT_MAX;
            for (int i = 0; i < n; i++)
            {
                if (!completed[i] && p[i].arrival_time < next_arrival)
                {
                    next_arrival = p[i].arrival_time;
                }
            }
            current_time = next_arrival;
            continue;
        }

        p[idx].start_time = current_time;
        p[idx].finish_time = p[idx].start_time + p[idx].burst_time;
        p[idx].turnaround_time = p[idx].finish_time - p[idx].arrival_time;
        p[idx].waiting_time = p[idx].start_time - p[idx].arrival_time;

        current_time = p[idx].finish_time;
        completed[idx] = 1;
        completed_count++;
    }

    printf("SJF (Non-preemptive) Gantt Chart:\n");
    print_gantt_chart(p, n);
    print_results("SJF (Non-preemptive)", p, n);
}

// Priority

void priority_non_preemptive(Process orig[], int n)
{
    Process p[100];
    copy_processes(p, orig, n);

    int completed[100] = {0};
    int completed_count = 0;
    int current_time = 0;

    while (completed_count < n)
    {
        int idx = -1;
        int best_priority = INT_MAX; // lower number = higher priority

        for (int i = 0; i < n; i++)
        {
            if (!completed[i] && p[i].arrival_time <= current_time)
            {
                if (p[i].priority < best_priority)
                {
                    best_priority = p[i].priority;
                    idx = i;
                }
            }
        }

        if (idx == -1)
        {
            int next_arrival = INT_MAX;
            for (int i = 0; i < n; i++)
            {
                if (!completed[i] && p[i].arrival_time < next_arrival)
                {
                    next_arrival = p[i].arrival_time;
                }
            }
            current_time = next_arrival;
            continue;
        }

        p[idx].start_time = current_time;
        p[idx].finish_time = p[idx].start_time + p[idx].burst_time;
        p[idx].turnaround_time = p[idx].finish_time - p[idx].arrival_time;
        p[idx].waiting_time = p[idx].start_time - p[idx].arrival_time;

        current_time = p[idx].finish_time;
        completed[idx] = 1;
        completed_count++;
    }

    printf("Priority (Non-preemptive) Gantt Chart:\n");
    print_gantt_chart(p, n);
    print_results("Priority (Non-preemptive)", p, n);
}

// Round Robin

void round_robin(Process orig[], int n, int quantum)
{
    Process p[100];
    copy_processes(p, orig, n);

    Segment segs[200];
    int seg_count = 0;

    int remaining[100];
    int done = 0;
    int current_time = 0;
    int first_start[100];

    for (int i = 0; i < n; i++)
    {
        remaining[i] = p[i].burst_time;
        first_start[i] = -1;
    }

    while (done < n)
    {
        int progress = 0;

        for (int i = 0; i < n; i++)
        {
            if (remaining[i] > 0 && p[i].arrival_time <= current_time)
            {
                progress = 1;

                if (first_start[i] == -1)
                    first_start[i] = current_time;

                int exec_time = (remaining[i] > quantum) ? quantum : remaining[i];

                int start_time = current_time;
                int end_time = current_time + exec_time;

                segs[seg_count].pid = p[i].pid;
                segs[seg_count].start = start_time;
                segs[seg_count].end = end_time;
                seg_count++;

                remaining[i] -= exec_time;
                current_time = end_time;

                if (remaining[i] == 0)
                {
                    p[i].finish_time = current_time;
                    done++;
                }
            }
        }

        // If no process could run at this time, jump to next arrival
        if (!progress)
        {
            int next_arrival = INT_MAX;
            for (int i = 0; i < n; i++)
            {
                if (remaining[i] > 0 && p[i].arrival_time < next_arrival)
                {
                    next_arrival = p[i].arrival_time;
                }
            }
            if (next_arrival == INT_MAX)
                break;
            if (current_time < next_arrival)
                current_time = next_arrival;
        }
    }

    // Compute waiting and turnaround
    for (int i = 0; i < n; i++)
    {
        p[i].start_time = first_start[i];
        p[i].turnaround_time = p[i].finish_time - p[i].arrival_time;
        p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;
    }

    printf("Round Robin Gantt Chart:\n");
    print_rr_gantt(segs, seg_count);
    print_results("Round Robin", p, n);
}