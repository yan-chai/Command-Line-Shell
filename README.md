# Project 2: Command Line Shell

Author: Haoyu Yan

## About This Project

This project is a UNIX/LINUX shell made by c. It support some command in real shell.

### introduction

The most useful application we use in UNIX-like system is terminal. In this program I build a UNIX-like terminal in c language.
This has two mode, the scripting mode and tty mode
In tty mode:
        $ ./crash
        >>-[🙂]─[1]─[yan@yan:~]$ (input command here)
In scripting mode:
        $ ./crash < input.file (This run a file of commmands line by line)


### supported command

This terminal support most of command of UNIX terminal, for example: ls, echo, sleep ......
There are other commands it also supports:

        exit: exit this program and stop the prompt line;
        cd : change current directory;
        # : the input behind "#" will be ignore;
        history: show user input history, the output is liek this:
            1  ls /ect
            2  echo 2
            3  sleep 5
            4  history
        ! ：1. !number: the number is the index of history command.(if we use "!1", the actrual command is "ls /ect")
            2. !! : rerun the previous instruction.
            3. !prefix : prefix is a command frefix, for example: "!ls", it will run "ls /ect"
        jobs : show background process
        IO redirection: use "<", ">>" and ">" to set input and output stream
        & : add at the end of line to make command run in backgroud

User can use basic terminal command in this program, also thye can use build-in command mentioned above.

### Include Files

There are several files included. There are:
* MakeFile: Including to compile and run the program.
* history.c: Implement the history part of program, use a clist data structure
* jobs.c : Store background jobs in an elist and modity them
* ui.c : The ui part of program, include the output and inpout formate, print prompt, show the status of last command, shoe current path and show user and hostname of current mechine.
* shell.c: The main function is in this file and it call other function from other file.

To compile and run:
        make
        ./crash

### Program output

        
        [yan@yan P2-yan-chai]$ ./crash
        ui.c:26:init_ui(): Initializing UI...
        ui.c:32:init_ui(): Setting locale: en_US.UTF-8
        jobs.c:38:elist_create(): Initializing new elist: capacity=[10], item_sz=[100]
        >>-[🙂]-[1]-[yan@yan:~/P2-yan-chai]-> echo 1
        shell.c:124:main(): Input command: echo 1
        1
        shell.c:67:handler(): This job end: echo 1
        >>-[🙂]-[2]-[yan@yan:~/P2-yan-chai]-> echo 2
        shell.c:124:main(): Input command: echo 2
        2
        shell.c:67:handler(): This job end: echo 2
        >>-[🙂]-[3]-[yan@yan:~/P2-yan-chai]-> echo 3
        shell.c:124:main(): Input command: echo 3
        3
        shell.c:67:handler(): This job end: echo 3
        >>-[🙂]-[4]-[yan@yan:~/P2-yan-chai]-> echo 4
        shell.c:124:main(): Input command: echo 4
        4
        shell.c:67:handler(): This job end: echo 4
        >>-[🙂]-[5]-[yan@yan:~/P2-yan-chai]-> history
        shell.c:124:main(): Input command: history
            1  echo 1
            2  echo 2
            3  echo 3
            4  echo 4
            5  history
        >>-[🙂]-[6]-[yan@yan:~/P2-yan-chai]-> exit
        shell.c:124:main(): Input command: exit
        shell.c:142:main(): Exiting shell

## Testing 

Use 'make test' to run test cases. To pull in and update test cases, use 'make testupdate'. To run certain test cases, use 'make test run=[number]'

        # Run all tests
        make test
        # Update test cases
        make testupdate
        # Run certain test case
        make test run=[number]
