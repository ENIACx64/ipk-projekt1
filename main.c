/*
IPK project 1
Author: Jindřich Vodák
Login: xvodak06
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PROC_LINE_LENGTH 128

/*
    ERROR CODES

    1 - uncorrect arguments
    2 - invalid port
    3 - invalid read
*/

void set_error(int error_code)
{
    switch (error_code)
    {
    case 1:
        fprintf(stderr, "\033[0;31mERROR 1:\033[0m       Uncorrect arguments! Usage: ./hinfosvc <port>\n");
        break;
    case 2:
        fprintf(stderr, "\033[0;31mERROR 2:\033[0m       Invalid port!\n");
        break;
    case 3:
        fprintf(stderr, "\033[0;31mERROR 3:\033[0m       Read operation has failed!\n");
        break;
    default:
        fprintf(stderr, "\033[0;31mERROR:\033[0m         Undefined error, application will end...\n");
        break;
    }

    exit(error_code);
}

void get_cpu_usage(char* output)
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

    for (unsigned i = 0; i < cpu_count; i++)
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

        pclose(procinfo1);
        pclose(procinfo2);

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

    char buffer[PROC_LINE_LENGTH + 1];
    sprintf(buffer, "%d%%", (int)total_percentage);
    strcpy(output, buffer);
}

char* get_hostname()
{
    char buffer[PROC_LINE_LENGTH + 1];
    FILE *hostname = popen("cat /proc/sys/kernel/hostname | head -n 1", "r");
    fgets(buffer, PROC_LINE_LENGTH, hostname);
    pclose(hostname);

    char* result = buffer;
    
    return buffer;
}

int get_cpu_name()
{
    char buffer[PROC_LINE_LENGTH + 1];
    FILE *cpu_name = popen("cat /proc/cpuinfo | grep \"model name\" | head -n 1 | awk -F': ' '{print $2}'", "r");
    fgets(buffer, PROC_LINE_LENGTH, cpu_name);
    pclose(cpu_name);

    fprintf(stderr, "%s\n", buffer);
    return 0;
}

int test() // DELETE
{
    char buffer[PROC_LINE_LENGTH + 1];

    // TEST_MAIN START
    fprintf(stderr, "\033[0;31mTEST_MAIN START\033[0m\n\n");

    // TEST_CPU
    fprintf(stderr, "\033[0;34mTEST_CPU START\033[0m\n");
    get_cpu_usage(buffer);
    fprintf(stderr, "%s\n", buffer);
    buffer[0] = '\0';
    fprintf(stderr, "\033[0;34mTEST_CPU END\033[0m\n\n");

    // TEST_HOSTNAME
    fprintf(stderr, "\033[0;33mTEST_HOSTNAME START\033[0m\n");
    get_hostname();
    fprintf(stderr, "%s\n", buffer);
    buffer[0] = '\0';
    fprintf(stderr, "\033[0;33mTEST_HOSTNAME END\033[0m\n\n");

    // TEST_CPUNAME
    fprintf(stderr, "\033[0;35mTEST_CPUNAME START\033[0m\n");
    get_cpu_name();
    buffer[0] = '\0';
    fprintf(stderr, "\033[0;35mTEST_CPUNAME END\033[0m\n\n");

    // TEST_MAIN END
    fprintf(stderr, "\033[0;31mTEST_MAIN END\033[0m\n");

    return 0;
}

int main(int argc, char *argv[])
{
    // DELETE
    test();

    if (argc != 2)
    {
        set_error(1);
    }

    long port;
    char *next_char;

    port = strtol(argv[1], &next_char, 10);

    if ((next_char == argv[1]) || (*next_char != '\0'))
    {
        set_error(2);
    }

    int endpoint = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int client;

    int option = 1;
    setsockopt(endpoint, SOL_SOCKET, SO_REUSEADDR, (const char *)&option, sizeof(int));
    setsockopt(endpoint, SOL_SOCKET, SO_REUSEPORT, (const char *)&option, sizeof(int));

    struct sockaddr_in address;
    int address_size = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    bind(endpoint, (struct sockaddr *)&address, sizeof(address));

    listen(endpoint, 3);

    char buffer[PROC_LINE_LENGTH];

    while (1)
    {
        client = accept(endpoint, (struct sockaddr *)&address, (socklen_t *)&address_size);
        char* header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";

        // POSSIBLE ISSUE
        buffer[0] = '\0';

        int is_ok = read(client, buffer, sizeof(buffer));

        if (is_ok == -1)
        {
            // EXITS THE PROGRAM - RESOLVE?
            close(client);
            set_error(3);
        }

        fprintf(stderr, "%s\n", buffer);

        // MODIFY
        send(client, header, sizeof(header), 0);
        close(client);
    }

    return 0;
}