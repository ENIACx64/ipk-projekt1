#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PROC_LINE_LENGTH 64

int cpu_usage()
{
    unsigned long long int usertime, nicetime, systemtime, idletime;
    unsigned long long int ioWait, irq, softIrq, steal, guest, guestnice;
    char buffer[PROC_LINE_LENGTH + 1];

    FILE *test = popen("cat /proc/stat", "r");
    fgets(buffer, PROC_LINE_LENGTH, test);
    sscanf(buffer, "cpu  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
           &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
    fprintf(stderr, "%llu\n", nicetime);
    fprintf(stderr, "TEST_CPU\n");

    return 0;
}

int main(int argc, char *argv[])
{
    cpu_usage();
    fprintf(stderr, "TEST_MAIN\n");

    return 0;
}