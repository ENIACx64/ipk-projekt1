# IPK project 1

**Author:** Jindřich Vodák

**Login:** xvodak06

## Project overview

The purpose of this application is to create a lightweight server communicating via the HTTP protocol, which is able to process three types of simple requests. These types are as follows:

* get CPU load
* get hostname
* get CPU name

These requests are self-explanatory and function exactly as their names suggest. All necessary info is gathered from the filesystem of hosting computer.

## Details

The server can be launched from the terminal by typing this command (inside the directory with the binary file):
```
./hinfosvc <port_number>
```