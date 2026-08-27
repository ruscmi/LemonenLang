# System library documenatation lmnlang

system lib - standard library on lmnlang allowing you to control system functions.

with it, you can turn off your device, detect the current OS, 
run software, delete files, create files, and much more.

**system library functional:**

let's start with the sys_exec() function - this function allows you to run files and 
returns the result after execution (usually 0 if the work ends successfully)

```lmnlang
lmport(system);
sys_exec("micro")
```
will run the terminal code editor on linux will return the number 0 after exiting.

next function - sys_read()

does not return any logs after execution and
runs the command covertly from the user using the pipe shape
although the principle of execution is the same as that 
of sys_exec() - it executes the commands that you give it.

```lmnlang
lmport(system);
sys_read("echo Hello")
```

will withdraw 'Hello' without logs

Now let's move on to the small features

function sys_getcwd() - display the user current directory

```lmnlang
lmport(system);
sys_getcwd()
```

execute current dir

function sys_getos() - display your operation system 

```lmnlang
lmport(system);
sys_getos()
```

execute your OS

function sys_cd() - allows you to enter a directory or exit all previous ones

```lmnlang
lmport(system);
sys_cd("/LemonenLang/libs/string")
```

will move you to the directory specified in quotes

function sys_sleep() - shuts down your device

```lmnlang
lmport(system);
sys_sleep()
```

system will go to sleep

NOTE! in Android it is possible to turn off the device through the terminal only under root rights

Thank you for reading the System Library documentation.

by [@ruscmi](https://github.com/ruscmi)
