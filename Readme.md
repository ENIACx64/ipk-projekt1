# IPK project 1

**Author:** Jindřich Vodák

**Login:** xvodak06

## Project overview

The purpose of this application is to create a lightweight server communicating via the HTTP protocol, which is able to process three types of simple requests. These types are as follows:

* get CPU load (_/load_)
* get hostname (_/hostname_)
* get CPU name (_/cpu-name_)

These requests are self-explanatory and function exactly as their names suggest. If a different request is made, the server will reply with _400 BAD REQUEST_. All necessary info is gathered from the filesystem of hosting computer. Every incoming request is logged in the server console in this form:
```
INCOMING REQUEST: <type_of_request>
```

## Details

The server can be launched from the terminal by typing this command (inside the directory with the binary file):
```
./hinfosvc <port_number>
```
The _port_number_ argument is the local port on which the server will listen.

While the server is online, it can be interacted with via _curl_ or _wget_ commands or, if needed, directly from web browser. Examples will be shown in the next few sections.

## Examples

All of the examples will be using _localhost_ as the server address because the client will be the same machine as the server. Port number 12345 will be used.

### Using _curl_

This is an example of using _curl_ to communicate with the server.
```
curl http://localhost:12345/request
```
The output should look like this:
![Output](/images/curl.png)

### Using _wget_

The _wget_ command is used in the exact same manner as the _curl_ command.
```
wget http://localhost:12345/request
```
