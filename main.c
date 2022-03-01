/*
IPK project 1
Author: Jindřich Vodák
Login: xvodak06
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PROC_LINE_LENGTH 128

int get_cpu_usage()
{
    unsigned long long int usertime, nicetime, systemtime, idletime;
    unsigned long long int ioWait, irq, softIrq, steal, guest, guestnice;
    ioWait = irq = softIrq = steal = guest = guestnice = 0;

    unsigned long long int prevusertime, prevnicetime, prevsystemtime, previdletime;
    unsigned long long int previoWait, previrq, prevsoftIrq, prevsteal, prevguest, prevguestnice;
    previoWait = previrq = prevsoftIrq = prevsteal = prevguest = prevguestnice = 0;

    char buffer1[PROC_LINE_LENGTH + 1];
    char buffer2[PROC_LINE_LENGTH + 1];

    unsigned cpu_count;

    FILE *cpuinfo = popen("lscpu -a -p=cpu | tail -1", "r");
    fgets(buffer1, PROC_LINE_LENGTH, cpuinfo);
    sscanf(buffer1, "%u", &cpu_count);
    cpu_count++;

    buffer1[0] = '\0';

    FILE *procinfo1 = popen("cat /proc/stat", "r");
    sleep(1);
    FILE *procinfo2 = popen("cat /proc/stat", "r");

    double total_percentage = 0;

    for (int i = 0; i < cpu_count; i++)
    {
        fgets(buffer1, PROC_LINE_LENGTH, procinfo1);
        fgets(buffer2, PROC_LINE_LENGTH, procinfo2);

        if (i == 0)
        {
            sscanf(buffer1, "cpu  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
                   &prevusertime, &prevnicetime, &prevsystemtime, &previdletime, &previoWait, &previrq, &prevsoftIrq, &prevsteal, &prevguest, &prevguestnice);
            sscanf(buffer2, "cpu  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
                   &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
        }
        else
        {
            unsigned prevcpuid;
            unsigned cpuid;
            sscanf(buffer1, "cpu%u  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
                   &prevcpuid, &prevusertime, &prevnicetime, &prevsystemtime, &previdletime, &previoWait, &previrq, &prevsoftIrq, &prevsteal, &prevguest, &prevguestnice);
            sscanf(buffer2, "cpu%u  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
                   &cpuid, &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
        }

        double previdle = previdletime + previoWait;
        double idle = idletime + ioWait;

        double prevnonidle = prevusertime + prevnicetime + prevsystemtime + previrq + prevsoftIrq + prevsteal;
        double nonidle = usertime + nicetime + systemtime + irq + softIrq + steal;

        double prevtotal = previdle + prevnonidle;
        double total = idle + nonidle;

        double totald = total - prevtotal;
        double idled = idle - previdle;

        double percentage = ((totald - idled) / (totald)) * 100;

        total_percentage += percentage / 12;
    }

    fprintf(stderr, "%d%%\n", (int)total_percentage);
    return 0;
}

int get_hostname()
{
    char buffer[PROC_LINE_LENGTH + 1];
    FILE *hostname = popen("cat /proc/sys/kernel/hostname | head -n 1", "r");
    fgets(buffer, PROC_LINE_LENGTH, hostname);

    fprintf(stderr, "%s", buffer);
    return 0;
}

int get_cpu_name()
{
    char buffer[PROC_LINE_LENGTH + 1];
    FILE *cpu_name = popen("cat /proc/cpuinfo | grep \"model name\" | head -n 1 | awk -F': ' '{print $2}'", "r");
    fgets(buffer, PROC_LINE_LENGTH, cpu_name);

    fprintf(stderr, "%s", buffer);
    return 0;
}

int main(int argc, char *argv[])
{
    fprintf(stderr, "\033[0;31mTEST_MAIN START\033[0m\n\n"); // DELETE

    fprintf(stderr, "\033[0;34mTEST_CPU START\033[0m\n"); // DELETE
    get_cpu_usage();
    fprintf(stderr, "\033[0;34mTEST_CPU END\033[0m\n\n"); // DELETE

    fprintf(stderr, "\033[0;33mTEST_HOSTNAME START\033[0m\n"); // DELETE
    get_hostname();
    fprintf(stderr, "\033[0;33mTEST_HOSTNAME END\033[0m\n\n"); // DELETE

    fprintf(stderr, "\033[0;35mTEST_CPUNAME START\033[0m\n"); // DELETE
    get_cpu_name();
    fprintf(stderr, "\033[0;35mTEST_CPUNAME END\033[0m\n\n"); // DELETE

    fprintf(stderr, "\033[0;31mTEST_MAIN END\033[0m\n"); // DELETE
    return 0;
}