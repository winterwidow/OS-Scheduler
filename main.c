#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algorithms.h"

int read_csv(const char *filename, Process p[])
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        printf("Error: Cannot open file %s\n", filename);
        return -1;
    }

    char line[256];
    int count = 0;

    // Skip header
    if (!fgets(line, sizeof(line), fp))
    {
        fclose(fp);
        return -1;
    }

    while (fgets(line, sizeof(line), fp))
    {
        char *token = strtok(line, ",");
        if (!token)
            break;
        p[count].pid = atoi(token);

        token = strtok(NULL, ",");
        if (!token)
            break;
        p[count].arrival_time = atoi(token);

        token = strtok(NULL, ",");
        if (!token)
            break;
        p[count].burst_time = atoi(token);

        token = strtok(NULL, ",");
        if (!token)
            break;
        p[count].priority = atoi(token);

        // Initialize computed fields
        p[count].start_time = 0;
        p[count].finish_time = 0;
        p[count].waiting_time = 0;
        p[count].turnaround_time = 0;

        count++;
    }

    fclose(fp);
    return count;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <csv_file> [time_quantum]\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int quantum = 2; // default
    if (argc >= 3)
    {
        quantum = atoi(argv[2]);
        if (quantum <= 0)
        {
            printf("Invalid quantum. Using default = 2\n");
            quantum = 2;
        }
    }

    Process processes[100];
    int n = read_csv(filename, processes);
    if (n <= 0)
    {
        printf("No processes loaded from %s\n", filename);
        return 1;
    }

    printf("Loaded %d processes from %s\n", n, filename);

    // fcfs(processes, n);
    // sjf_non_preemptive(processes, n);
    // priority_non_preemptive(processes, n);
    // round_robin(processes, n, quantum);
    int choice;
    while (1)
    {
        printf("\nCPU Scheduling Simulator\n");
        printf("1. FCFS\n");
        printf("2. SJF (Non-preemptive)\n");
        printf("3. Priority (Non-preemptive)\n");
        printf("4. Round Robin\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 5)
            break;

        switch (choice)
        {
        case 1:
            fcfs(processes, n);
            break;
        case 2:
            sjf_non_preemptive(processes, n);
            break;
        case 3:
            priority_non_preemptive(processes, n);
            break;
        case 4:
            printf("Enter time quantum: ");
            scanf("%d", &quantum);
            round_robin(processes, n, quantum);
            break;
        default:
            printf("Invalid choice.\n");
        }

        return 0;
    }