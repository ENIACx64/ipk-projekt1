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

// sets the error code and exits the application
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

// gets the CPU usage and stores it in parameter "output"
void get_cpu_usage(char *output)
{
    // declaration of variables
    unsigned long long int usertime, nicetime, systemtime, idletime;
    unsigned long long int ioWait, irq, softIrq, steal, guest, guestnice;
    ioWait = irq = softIrq = steal = guest = guestnice = 0;

    unsigned long long int prevusertime, prevnicetime, prevsystemtime, previdletime;
    unsigned long long int previoWait, previrq, prevsoftIrq, prevsteal, prevguest, prevguestnice;
    previoWait = previrq = prevsoftIrq = prevsteal = prevguest = prevguestnice = 0;

    char buffer1[PROC_LINE_LENGTH + 1];
    char buffer2[PROC_LINE_LENGTH + 1];

    unsigned cpu_count;

    // gets the CPU data from filesystem
    FILE *cpuinfo = popen("lscpu -a -p=cpu | tail -1", "r");
    fgets(buffer1, PROC_LINE_LENGTH, cpuinfo);
    sscanf(buffer1, "%u", &cpu_count);
    cpu_count++;

    buffer1[0] = '\0';

    // loads two data sets
    FILE *procinfo1 = popen("cat /proc/stat", "r");
    sleep(1);
    FILE *procinfo2 = popen("cat /proc/stat", "r");

    double total_percentage = 0;

    /*
    * loops for each logical CPU core and calculates data
    * (https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux)
    */
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

    pclose(procinfo1);
    pclose(procinfo2);

    // formats the output and stores it
    char buffer[PROC_LINE_LENGTH + 1];
    sprintf(buffer, "%d%%\n", (int)total_percentage);
    strcpy(output, buffer);
}

// gets the hostname and stores it in parameter "output"
void get_hostname(char *output)
{
    // gets the hostname data from filesystem
    char buffer[PROC_LINE_LENGTH + 1];
    FILE *hostname = popen("cat /proc/sys/kernel/hostname | head -n 1", "r");
    fgets(buffer, PROC_LINE_LENGTH, hostname);
    pclose(hostname);

    // formats the output and stores it
    strcpy(output, buffer);
}

// gets the CPU name and stores it in parameter "output"
void get_cpu_name(char *output)
{
    // gets the CPU name data from filesystem
    char buffer[PROC_LINE_LENGTH + 1];
    FILE *cpu_name = popen("cat /proc/cpuinfo | grep \"model name\" | head -n 1 | awk -F': ' '{print $2}'", "r");
    fgets(buffer, PROC_LINE_LENGTH, cpu_name);
    pclose(cpu_name);

    // formats the output and stores it
    strcpy(output, buffer);
}

// main() function
int main(int argc, char *argv[])
{
    // checks if arguments are correct and parses port argument
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

    // opens the socket for communication
    int endpoint = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int client;

    // sets correct options
    int option = 1;
    setsockopt(endpoint, SOL_SOCKET, SO_REUSEADDR, (const char *)&option, sizeof(int));
    setsockopt(endpoint, SOL_SOCKET, SO_REUSEPORT, (const char *)&option, sizeof(int));

    // sets the address correctly
    struct sockaddr_in address;
    int address_size = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    // gets the socket ready and starts to listen on the port
    bind(endpoint, (struct sockaddr *)&address, sizeof(address));
    listen(endpoint, 3);

    // declares buffer
    char buffer[PROC_LINE_LENGTH];

    // while loop - always true
    while (1)
    {
        // creates new socket and declares important variables
        client = accept(endpoint, (struct sockaddr *)&address, (socklen_t *)&address_size);
        char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";
        char tail[PROC_LINE_LENGTH];
        char reply[PROC_LINE_LENGTH];

        // clears all buffers
        memset(buffer, 0, sizeof(buffer));
        memset(tail, 0, sizeof(tail));
        memset(reply, 0, sizeof(reply));

        // gets the request
        int is_ok = read(client, buffer, sizeof(buffer));

        // checks if request is invalid
        if (is_ok == -1)
        {
            close(client);
            set_error(3);
        }

        // parses and then processes the request by calling the relevant function
        if (strncmp(buffer, "GET /load", 9) == 0)
        {
            fprintf(stderr, "INCOMING REQUEST: LOAD\n");
            get_cpu_usage(tail);
        }
        else if (strncmp(buffer, "GET /hostname", 13) == 0)
        {
            fprintf(stderr, "INCOMING REQUEST: HOSTNAME\n");
            get_hostname(tail);
        }
        else if (strncmp(buffer, "GET /cpu-name", 13) == 0)
        {
            fprintf(stderr, "INCOMING REQUEST: CPU-NAME\n");
            get_cpu_name(tail);
        }
        else
        {
            fprintf(stderr, "INCOMING REQUEST: BAD REQUEST\n");
            strcat(tail, "400 BAD REQUEST\n");
        }

        // formats the response
        int breakpoint;

        for (int i = 0; i < strlen(header); i++)
        {
            reply[i] = header[i];
            breakpoint = i;
        }

        breakpoint++;

        for (int i = 0; i < strlen(tail); i++)
        {
            reply[breakpoint + i] = tail[i];
        }

        // sends the response and closes the socket
        send(client, reply, strlen(reply), 0);
        close(client);
    }

    return 0;
}