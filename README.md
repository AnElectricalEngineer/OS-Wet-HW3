# TTFTPS
 
TTFTP (Trivial Trivial File Transfer Protocol Server) is a FTP server for GNU/Linux operating systems.

## About TTFTPS

Bank is a multithreaded application written in C++ which uses the *pthreads* library. The bank is comprised of N ATMs - each with a unique ID number, and bank accounts. Each bank account has an account number, a PIN, and a balance. 

## Using Bank

Bank should be run from the terminal by executing the following line (from within the directory containing the executable: `./Bank <Number of ATMs – N> <ATM_1_input_file> <ATM_2_input_file>…<ATM_N_input_file>`. Each ATM input file is a text file which contains commands that will be executed at the corresponding ATM (ATM_1_input_file corresponds to the first ATM). Example input file:
```
O 12345 1234 100
W 12345 1234 50
D 12345 1234 12
O 12346 1234 45
W 12345 1234 35
```

To see a list of all the possible input commands, see the attached PDF.

The Bank will output to standard output the current bank status - a list of all the accounts with their balances and passwords, every half-second. In addition, the Bank will print to a logfile a record of all transactions conducted.

## Screenshots

![image](https://user-images.githubusercontent.com/47844685/130353266-da0ca478-d254-4346-98fa-fd2c27fbdd98.png)
![image](https://user-images.githubusercontent.com/47844685/130353287-3cff55c3-fc7f-44ae-aa52-09ebcd3013a4.png)

## Build from source

To build from source, first clone the repository. Then open a terminal in the directory into which you cloned the repository and run `make`.
