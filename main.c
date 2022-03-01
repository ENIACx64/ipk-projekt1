#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PROC_LINE_LENGTH 128

int cpu_usage()
{
    unsigned long long int usertime, nicetime, systemtime, idletime;
    unsigned long long int ioWait, irq, softIrq, steal, guest, guestnice;
    ioWait = irq = softIrq = steal = guest = guestnice = 0;
    char buffer[PROC_LINE_LENGTH + 1];
    unsigned cpu_count;

    FILE *cpuinfo = popen("lscpu -a -p=cpu | tail -1", "r");
    fgets(buffer, PROC_LINE_LENGTH, cpuinfo);
    sscanf(buffer, "%u", &cpu_count);
    cpu_count++;

    buffer[0] = '\0';

    FILE *procinfo = popen("cat /proc/stat", "r");

    for (int i = 0; i < cpu_count; i++)
    {
        fgets(buffer, PROC_LINE_LENGTH, procinfo);

        if (i == 0)
        {
            sscanf(buffer, "cpu  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
                   &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
        }
        else
        {
            unsigned cpuid;
            sscanf(buffer, "cpu%u  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
                   &cpuid, &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
        }

        fprintf(stderr, "%s\n", buffer); // DELETE

        fprintf(stderr, "TEST - USERTIME = %llu\n", usertime);  // DELETE

        usertime = usertime - guest;
        nicetime = nicetime - guestnice;

        unsigned long long int idlealltime = idletime + ioWait;
        unsigned long long int systemalltime = systemtime + irq + softIrq;
        unsigned long long int virtalltime = guest + guestnice;
        unsigned long long int totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;

        unsigned percentage = (totaltime - idlealltime) / (totaltime * 100);

        fprintf(stderr, "TEST - TOTALTIME = %llu\n", totaltime);  // DELETE
        fprintf(stderr, "TEST - IDLEALLTIME = %llu\n", idlealltime);  // DELETE

        fprintf(stderr, "PERCENTAGE for CPU %d: %u\n", i, percentage); // DELETE
    }

    fprintf(stderr, "TEST_CPU\n"); // DELETE
    return 0;
}

int main(int argc, char *argv[])
{
    cpu_usage();

    fprintf(stderr, "TEST_MAIN\n"); // DELETE
    return 0;
}