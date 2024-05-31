#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

struct Process {
    char name[20];
    int burstTime;
    int prio;
    int Waittime;
    int Tat;
    int rt;
    int deadline;
    int period;
    int share;
};

struct SchedulingMetrics {
    double avgTat;
    double avgWait;
    double avgResponse;
};

void generateRandomProcessesToFile(const char* filename, int n) {
    FILE* outputFile = fopen(filename, "w");
    if (outputFile == NULL) {
        perror("Error: in opening the output file");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL)); // Seed for random number generation

    for (int i = 0; i < n; i++) {
        fprintf(outputFile, "P%d %d %d %d %d %d\n", i + 1, rand() % 20 + 1, rand() % 10, rand() % 30 + 1, rand() % 10 + 1, rand() % 100 + 1);
    }

    fclose(outputFile);
}

void readProcessesFromFile(const char* filename, struct Process* processes, int* n) {
    FILE* inputFile = fopen(filename, "r");
    if (inputFile == NULL) {
        perror("Error: in opening the input file");
        exit(EXIT_FAILURE);
    }

    *n = 0;

    while (fscanf(inputFile, "%s %d %d %d %d %d", processes[*n].name, &processes[*n].burstTime, &processes[*n].prio,
                  &processes[*n].deadline, &processes[*n].period, &processes[*n].share) != EOF) {
        (*n)++;
    }

    fclose(inputFile);
}

int checkprio(const void* a, const void* b) {
    return ((struct Process*)a)->prio - ((struct Process*)b)->prio;
}

int checkdeadline(const void* a, const void* b) {
    if (((struct Process*)a)->deadline == ((struct Process*)b)->deadline) {
        return ((struct Process*)a)->prio - ((struct Process*)b)->prio;
    }
    return ((struct Process*)a)->deadline - ((struct Process*)b)->deadline;
}

int checkperiod(const void* a, const void* b) {
    return ((struct Process*)a)->period - ((struct Process*)b)->period;
}

int checkshare(const void* a, const void* b) {
    return ((struct Process*)a)->share - ((struct Process*)b)->share;
}

double calAverageTat(const struct Process* processes, int n) {
    double totalTat = 0;
    for (int i = 0; i < n; i++) {
        totalTat += processes[i].Tat;
    }
    return totalTat / n;
}

double calAverageWaittime(const struct Process* processes, int n) {
    double totalWaittime = 0;
    for (int i = 0; i < n; i++) {
        totalWaittime += processes[i].Waittime;
    }
    return totalWaittime / n;
}

double calAverageResponseTime(const struct Process* processes, int n) {
    double totalResponseTime = 0;
    for (int i = 0; i < n; i++) {
        totalResponseTime += processes[i].rt;
    }
    return totalResponseTime / n;
}


void runPriorityScheduling(struct Process* processes, int n) {
    qsort(processes, n, sizeof(struct Process), checkprio);

    for (int i = 0; i < n; i++) {
        printf("Executing the task %s with priority %d for %d seconds\n", processes[i].name, processes[i].prio, processes[i].burstTime);
        sleep(processes[i].burstTime); // Simulate execution
        printf("Task with priority %d completed.\n", processes[i].prio);

        processes[i].Tat = processes[i].burstTime;
        processes[i].Waittime = 0;
        processes[i].rt = 0;

        for (int j = 0; j < i; j++) {
            processes[i].Tat += processes[j].burstTime;
            processes[i].Waittime += processes[j].burstTime;
        }
    }
}


void runEDFScheduling(struct Process* processes, int n) {
    qsort(processes, n, sizeof(struct Process), checkdeadline);

    int currentTime = 0;

    for (int i = 0; i < n; i++) {
        printf("Executing the task %s with deadline %d for %d seconds\n", processes[i].name, processes[i].deadline, processes[i].burstTime);
        sleep(processes[i].burstTime); // Simulate execution
        printf("Task with deadline %d completed.\n", processes[i].deadline);

        processes[i].Tat = currentTime + processes[i].burstTime;
        processes[i].Waittime = processes[i].Tat - processes[i].burstTime;
        processes[i].rt = currentTime;
        currentTime += processes[i].burstTime;
    }
}


void runRMScheduling(struct Process* processes, int n) {
    qsort(processes, n, sizeof(struct Process), checkperiod);

    int currentTime = 0;

    for (int i = 0; i < n; i++) {
        printf("Executing the task %s with period %d for %d seconds\n", processes[i].name, processes[i].period, processes[i].burstTime);
        sleep(processes[i].burstTime); // Simulate execution
        printf("Task with period %d completed.\n", processes[i].period);

        processes[i].Tat = currentTime + processes[i].burstTime;
        processes[i].Waittime = processes[i].Tat - processes[i].burstTime;
        processes[i].rt = currentTime;
        currentTime += processes[i].burstTime;
    }
}


void runProportionalScheduling(struct Process* processes, int n) {
    int totalShares = 0;
    for (int i = 0; i < n; i++) {
        totalShares += processes[i].share;
    }

    for (int i = 0; i < n; i++) {
        processes[i].share = (processes[i].share * 1000) / totalShares; // Convert shares to 1000-based for precise timing

        printf("Executing the task %s with %d shares for %d seconds\n", processes[i].name, processes[i].share, processes[i].burstTime);
        usleep(processes[i].share * processes[i].burstTime);

        processes[i].Tat = processes[i].burstTime;
        processes[i].Waittime = 0;
        processes[i].rt = 0;

        for (int j = 0; j < i; j++) {
            processes[i].Tat += (processes[j].share * processes[j].burstTime) / 1000;
            processes[i].Waittime += (processes[j].share * processes[j].burstTime) / 1000;
        }
    }
}




int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <numOfProcesses>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int numOfProcesses = atoi(argv[1]); // Convert the command-line argument to an integer
    if (numOfProcesses <= 0) {
    fprintf(stderr, "Error: Number of processes must be a positive integer.\n");
    return EXIT_FAILURE;
    }

    const char* filename = "random_processes.txt"; // Adjust filename as needed

    // Generate random processes and save to file
    generateRandomProcessesToFile(filename, numOfProcesses);

    struct Process processes[100]; // Assuming a maximum of 100 processes
    int n;

    // Read processes from file
    readProcessesFromFile(filename, processes, &n);

    printf("Num of Processes: %d\n", n);

    
    // Priority Scheduling
    printf("Running Priority Scheduling:\n");
    struct Process priorityProcesses[100]; // Copy of the original processes
    memcpy(priorityProcesses, processes, sizeof(struct Process) * n); // Copy the processes
    runPriorityScheduling(priorityProcesses, n);

    printf("\nResults for Priority Scheduling:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %s:\n", priorityProcesses[i].name);
        printf("  Turnaround Time: %d ms\n", priorityProcesses[i].Tat);
        printf("  Waiting Time: %d ms\n", priorityProcesses[i].Waittime);
        printf("  Response Time: %d ms\n", priorityProcesses[i].rt);
    }

    
    // Earliest Deadline First Schduling
    printf("\nRunning Earliest Deadline First Scheduling:\n");
    struct Process EDF_Processes[100]; // Copy of the original processes
    memcpy(EDF_Processes, processes, sizeof(struct Process) * n); // Copy the processes
    runEDFScheduling(EDF_Processes, n);

    printf("\nResults for Earliest Deadline First Scheduling:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %s:\n", EDF_Processes[i].name);
        printf("  Turnaround Time: %d ms\n", EDF_Processes[i].Tat);
        printf("  Waiting Time: %d ms\n", EDF_Processes[i].Waittime);
        printf("  Response Time: %d ms\n", EDF_Processes[i].rt);
    }


    // Rate Monotonic Schdeuling
    printf("\nRunning Rate Monotonic Scheduling:\n");
    struct Process RM_Processes[100]; // Copy of the original processes
    memcpy(RM_Processes, processes, sizeof(struct Process) * n); // Copy the processes
    runRMScheduling(RM_Processes, n);

    printf("\nResults for Rate Monotonic Scheduling:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %s:\n", RM_Processes[i].name);
        printf("  Turnaround Time: %d ms\n", RM_Processes[i].Tat);
        printf("  Waiting Time: %d ms\n", RM_Processes[i].Waittime);
        printf("  Response Time: %d ms\n", RM_Processes[i].rt);
    }


    // Proportional Share Scheduling
    printf("\nRunning Proportional Share Scheduling:\n");
    struct Process PS_Processes[100]; // Copy of the original processes
    memcpy(PS_Processes, processes, sizeof(struct Process) * n); // Copy the processes
    runProportionalScheduling(PS_Processes, n);

    printf("\nResults for Proportional Share Scheduling:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %s:\n", PS_Processes[i].name);
        printf("  Turnaround Time: %d ms\n", PS_Processes[i].Tat);
        printf("  Waiting Time: %d ms\n", PS_Processes[i].Waittime);
        printf("  Response Time: %d ms\n", PS_Processes[i].rt);
    }

   

   // Calculate Metrics
    struct SchedulingMetrics priorityMetrics, edfMetrics, rmMetrics, psMetrics;

    priorityMetrics.avgTat = calAverageTat(priorityProcesses, n);
    priorityMetrics.avgWait = calAverageWaittime(priorityProcesses, n);
    priorityMetrics.avgResponse = calAverageResponseTime(priorityProcesses, n);

    edfMetrics.avgTat = calAverageTat(EDF_Processes, n);
    edfMetrics.avgWait = calAverageWaittime(EDF_Processes, n);
    edfMetrics.avgResponse = calAverageResponseTime(EDF_Processes, n);

    rmMetrics.avgTat = calAverageTat(RM_Processes, n);
    rmMetrics.avgWait = calAverageWaittime(RM_Processes, n);
    rmMetrics.avgResponse = calAverageResponseTime(RM_Processes, n);

    psMetrics.avgTat = calAverageTat(PS_Processes, n);
    psMetrics.avgWait = calAverageWaittime(PS_Processes, n);
    psMetrics.avgResponse = calAverageResponseTime(PS_Processes, n);

    // Print Metrics
    printf("\nMetrics Comparison:\n");
    printf("Priority Scheduling:\n");
    printf("  Average Turnaround Time: %.2f ms\n", priorityMetrics.avgTat);
    printf("  Average Waiting Time: %.2f ms\n", priorityMetrics.avgWait);
    printf("  Average Response Time: %.2f ms\n", priorityMetrics.avgResponse);

    printf("\nEarliest Deadline First Scheduling:\n");
    printf("  Average Turnaround Time: %.2f ms\n", edfMetrics.avgTat);
    printf("  Average Waiting Time: %.2f ms\n", edfMetrics.avgWait);
    printf("  Average Response Time: %.2f ms\n", edfMetrics.avgResponse);

    printf("\nRate Monotonic Scheduling:\n");
    printf("  Average Turnaround Time: %.2f ms\n", rmMetrics.avgTat);
    printf("  Average Waiting Time: %.2f ms\n", rmMetrics.avgWait);
    printf("  Average Response Time: %.2f ms\n", rmMetrics.avgResponse);

    printf("\nProportional Share Scheduling:\n");
    printf("  Average Turnaround Time: %.2f ms\n", psMetrics.avgTat);
    printf("  Average Waiting Time: %.2f ms\n", psMetrics.avgWait);
    printf("  Average Response Time: %.2f ms\n", psMetrics.avgResponse);

    // Conclusion
    printf("\nConclusion : With Number of Process = %d\n", n);
    printf("\nBy comparing the Average TurnAround Time\n");
    if (priorityMetrics.avgTat < edfMetrics.avgTat && priorityMetrics.avgTat < rmMetrics.avgTat && priorityMetrics.avgTat < psMetrics.avgTat) {
        printf("Priority Scheduling is the best.\n");
    } else if (edfMetrics.avgTat < rmMetrics.avgTat && edfMetrics.avgTat < psMetrics.avgTat) {
        printf("Earliest Deadline First Scheduling is the best.\n");
    } else if (rmMetrics.avgTat < psMetrics.avgTat) {
        printf("Rate Monotonic Scheduling is the best.\n");
    } else {
        printf("Proportional Share Scheduling is the best.\n");
    }

    printf("\nBy comparing the Average Waiting Time\n");
    if (priorityMetrics.avgWait < edfMetrics.avgWait && priorityMetrics.avgWait < rmMetrics.avgWait && priorityMetrics.avgWait < psMetrics.avgWait) {
        printf("Priority Scheduling is the best.\n");
    } else if (edfMetrics.avgWait < rmMetrics.avgWait && edfMetrics.avgWait < psMetrics.avgWait) {
        printf("Earliest Deadline First Scheduling is the best.\n");
    } else if (rmMetrics.avgWait < psMetrics.avgWait) {
        printf("Rate Monotonic Scheduling is the best.\n");
    } else {
        printf("Proportional Share Scheduling is the best.\n");
    }


    return 0;

}
