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
            sscanf(buffer, "cpu  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
                   &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
        }

        fprintf(stderr, "%s\n", buffer);
    }

    fprintf(stderr, "TEST_CPU\n");
    return 0;
}

int main(int argc, char *argv[])
{
    cpu_usage();

    fprintf(stderr, "TEST_MAIN\n");
    return 0;
}