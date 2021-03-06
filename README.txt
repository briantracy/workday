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
4. Move the compiled binary to somewhere in your $PATH (normaly /usr/local/bin)
5. Move the man page to somewhere in your man path. /usr/bin/manpath will output
   this for you. You should put this man page in /usr/share/man/man1

Examples of each step:
1, 2.
    $ git clone 'https://github.com/briantracy/workday.git'
 OR
    $ curl -o workday.c 'https://raw.githubusercontent.com/briantracy/workday/master/workday.c'
    $ curl -o workday.1 'https://raw.githubusercontent.com/briantracy/workday/master/workday.1'

3.
    $ gcc -Wall -Wextra -Werror -Wpedantic workday.c

4.
    $ mv a.out /usr/local/bin/workday
    $ chmod +x /usr/local/bin/workday  # might be necessary, honestly not sure. 

5.
    $ mv workday.1 /usr/share/man/man1/workday.1


To accomplish all of these automatically, you can use the install.sh script.

    $ curl 'https://raw.githubusercontent.com/briantracy/workday/master/install.sh' -o install.sh && sh install.sh && rm install.sh

Please, for the sanity of programmers everywhere, do not blindly paste the above command into your terminal and run it. You are downloading a file from the internet, then giving it access to roam freely on your system. The least you can do is give it a once-over and always remain weary of the "curl | sh" idiom.
