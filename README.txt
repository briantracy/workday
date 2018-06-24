+------------------------------------------+
| workday - command line punch card + more |
+------------------------------------------+

This software comes with the following: 
1. The workday source code
2. The workday man page
3. This README.txt

INSTALLATION
------------
1. Obtain a copy of the source (workday.c)
2. Obtain a copy of the man page (workday.1)
3. Compile the source for your platform. 
4. Move the compiled binary to somewhere in your $PATH
5. Move the man page to somewhere in your man path (normally /usr/share/man)

Example: 
$ git clone <this repo>
$ cd workday
$ gcc workday.c
$ sudo mv ./a.out /usr/local/bin/workday
$ sudo chmod +x /usr/local/bin/workday
$ mv workday.1 /usr/share/man/man1
