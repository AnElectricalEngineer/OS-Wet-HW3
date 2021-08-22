# TTFTPS
 
TTFTP (Trivial Trivial File Transfer Protocol Server) is an FTP server for GNU/Linux operating systems.

## About TTFTPS

TTFTPS is a FTP server written in C++ which implements a simplified version of the Trivial File Transfer Protocol (TFTP). As such, it uses UDP and supports only one simultaneous client. The TTFTP server supports transmission of octect packets (binary information packets that do not require translation) only, and not ASCII packets. The server utilizes sockets in order to connect to clients. For a full understanding of the logic of the server, see the attached PDF.

## Using TTFTPS

To test the TTFTP server (on a single machine), open two terminals - one from the desired target directory for the file, and one from the directory which currently holds the file to be transferred. In the first terminal, run the TTFTP server by executing the command: `./ttftps <port_num>`, where port_num is the port that the server will listen on (if a permission denied error is received, ensure that the input port number is greater than or equal to 10,000). Then, in the second terminal, run the client by executing the following command: `./tftp <server_name> <server_port> S <file_name> [<dest_file_name>]`, where **server_name** is **local_host**, server_port is the same port number that was chosen when the server was run, S indicates sending, file_name is the name of the file to send, and dest_file_name is an optional field which sets the name of the file in the target directory. If a permission error is encountered, ensure that the client executable (tftp) has execute permissions.

## Screenshots

![image](https://user-images.githubusercontent.com/47844685/130356657-0b8dd514-e9ad-48f2-afaa-2893b634afd2.png)
![image](https://user-images.githubusercontent.com/47844685/130356686-d0a712ea-c4e9-4514-9ea0-08c1e5e8d5cb.png)

## Build from source

To build from source, first clone the repository. Then open a terminal in the directory into which you cloned the repository and run `make`.
