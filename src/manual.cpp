/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/parser.hpp"
#include <iostream>
#include "../include/utf8_win.hpp"
using namespace std;
/*
//ru//
перенес мануал сюда потому что он разрастается и мешает в парсере,
а так его можно будет просто инклудить и вызывать в рекурсионном спуске внутри парсера
//en//
moved the manual here because it grows and interferes in the parser, 
otherwise it can simply be inserted and called in recursion descent inside the parser
*/
unique_ptr<Node> Parser::parse_manual() {
	setup_utf8();
	Token current = peer();
	const char* bluec = "\033[34m";
	const char* resbc = "\033[0m";
	auto node = make_unique<Node>(peer());
	node->KEY = ST_NOP;
	node->VAL = "nop";
	if(is_runner) {
		cout<<"\033[1;33mE: Cannot call manual in file run mode\033[0m"<<endl;
		return node;
	}
	if(current.KEY == TTYPE::STRING && current.VAL == "man") {
		advanced();
		if(peer().KEY == TTYPE::STRING && peer().VAL == "list") {
			advanced();
			cout<<bluec<<R"(   Hello buddy!
		Args for man
 math - see algebraic examples
 assignment - see examples assignments
 print - see examples prints vars or strings
 runner - see instructions and examples of run files code
 arrays - see examples of working with arrays
 comparisons - see instructions to use comparisons
 input - see examples using input
 operators - see all operators and boolea operators
 typeof - see the manual on using type checking
 length - see the manual on using length checking
 while - see examples while loop
 types - see all data types
 func - see examples using functions
 wait - see lmit command manual
 lib - use lib + <arg> or lib list
 dictionaries - see manual to use dictionares)"<<resbc<<endl;
		}
		else if(peer().KEY == TTYPE::STRING && peer().VAL == "dictionaries") {
		    advanced();
		    cout<<bluec<<R"(    sup this is a manual on dictionaries
 To create dictionaries, use the following syntax
    example:
        dict = {
            "age" : 14,
            "name" : "ruscmi"
        }
 To add to the dictionary
    example:
        dict["hobby"] = "programming"
 to manage the index
    example:
        dict["age"] = 15;
 that's all for big dick)"<<resbc<<endl;
		}
		else if(peer().KEY == TTYPE::STRING && peer().VAL == "wait") {
		    advanced();
		    cout<<bluec<<R"(   for use wait command
		    it makes the program fall asleep for a certain amount of time
Use the 'lmit' keyword.
    example:
        lmuck("L",lmit(1),"M",lmit(1),"N");
it's all thx for huge tits.
		    )"<<resbc<<endl;
		}
		else if(peer().KEY == TTYPE::STRING && peer().VAL == "func") {
		    advanced();
		    cout<<bluec<<R"(
		        in Lmnlang, the functions 
		have about the same scope as in the petyhon
    basic func:
        example:
            func f() {
                lmuck(5)
            };
            f()
        write '5'
    Nested Functions:
        example:
            func f() { 
                func d() { 
                    lmuck(7) 
                }; 
                lmuck(6); 
                d() 
            }; 
            f()
        write '6' \n '7'	 
    Scopes:
        example:
            func f() {
                x = 5;
                return x;
            }
            f()
        write '5'
    it's all suchary
        )"<<resbc<<endl;
		}
		else if (peer().KEY == TTYPE::STRING && peer().VAL == "lib") {
		    advanced();
		    if(peer().KEY == TTYPE::STRING && peer().VAL == "list") {
		       advanced();
		       cout<<bluec<<R"(   hello,this is all libs
all libs written on the lmn itself
ansi - see manual for ansi lib
string - see manual for string lib
random - see manual for random lib
for load libs or file use:
    lmport(lib)
or for file:
    lmport "/path/to/file")"<<resbc<<endl;
		    }
		    else if(peer().KEY == TTYPE::STRING && peer().VAL == "random") {
		        advanced();
		        cout<<bluec<<R"(this lib is small
it has 2 features: <randint> and <choice>
use:
    rnd_<func>
example:
    rnd_randint(1,10);
execute a number from 1 to 10
this all thx for reading manual)"<<resbc<<endl;
		    }
		    else if(peer().KEY == TTYPE::STRING && peer().VAL == "string") {
		        advanced();
		        cout<<bluec<<R"(    how use string library?
 str_invertion(str) - invertion string in parentheses
 example:
    str_invertion("Hello")
 execute 'olleH' 

 str_repeat(str,count) - repeat string 
 as many times as you type in instead of count
 example:
    str_repeat("lmn",5)
 execute 'lmnlmnlmnlmnlmn'
  
 str_substring(str,start,len) - will cut off the part from the start 
 counter from the row to the length you want to cut 
 example:
    str_substring("HELLO",1,2)
 execute 'EL'
 str_join(arr,text) - joins all the elements of 
 the array into a single line 
 You can also enter an argument between
 the attached elements of the
 array instead of the text:
   example: 
         str_join(["+7",777,77,77],"-")
    execute '+7-777-77-77'
 str_split(string) - slices the string by spaces 
 and places each word in the string in a 
 separate array element.
    example:
        str_split("hello world !")
    execute '[hello, world, !]'
  thank you all for that.)"<<resbc<<endl;
		    }
		    else if(peer().KEY == TTYPE::STRING && peer().VAL == "ansi") {
		        advanced();
		        cout<<bluec<<R"(      all ansi commands
    paint_<color>("line") - paint line;
    What colors are there?
    black,red,green,blue,yellow,magenta,cyan and white.
    example:
        paint_yellow("lemon");
    execute 'lemon' with yellow color.

    clear_screen() - will clear your terminal of everything.
    clear_line() - will clear line of text. 
    <style>_write() - More unusual styles for text.
    What are the styles?
    dim,underlined,through,neon,blink,italic,invert,hidden
    example:
        dim_write("python");
    will paint the 'python' with a dull shade
    it's all lemons.)"<<resbc<<endl;
		    }else {
		        advanced();
		        cout<<bluec<<R"(use: lib + <arg> see lib list)"<<resbc<<endl;
		    }
		}
		else if (peer().KEY == TTYPE::STRING && peer().VAL == "operators") {
            advanced();
            cout<<bluec<<R"(
        All operators in lmnlang
classic operators:

  +  -  / *  %   

boolea operators:

  &&  ||  !  

        examples:

classic operators:

  5 + 5 - 5 / 5 * 5 % 5

boolea operators: 

  if(x && y || !z ) 

this is all.)"<<resbc<<endl;
		}
		else if (peer().KEY == TTYPE::STRING && peer().VAL == "typeof") {
            advanced();
            cout<<bluec<<R"(
    To determine the data type, use the lmtype function.
examples:
    x = "string";
    y = lmtype(x);
    lmuck(y);
that's all.)"<<resbc<<endl;
		}
		else if (peer().KEY == TTYPE::STRING && peer().VAL == "length") {
		    advanced();
		    cout<<bluec<<R"(
	To measure the length of a string or array, use lmlen:
examples:
    x = "string";
    y = lmlen(x);
    lmuck(y);
or on arrays:
    x = [5,5];
    y = lmlen(x);
    lmuck(y);
WARNING: You cannot measure the length of numbers or the length 
of a dick... oops, the length of a bool.

that's all.)"<<resbc<<endl;
		}
		else if (peer().KEY == TTYPE::STRING && peer().VAL == "while") {
            advanced();
		    cout<<bluec<<R"(
		    Manual to use while

C-style syntax is used for the while loop
example:
    while(true) {
        lmuck(5);
    }
 this is endless loop
    i = 0;
    while(i < 5) {
        lmuck(5);
        i = i + 1;
    }
 This is a loop that outputs a number five times.
 You can also pass a flag into the loop.
    example:
        x = true;
        while(x) {
            inp = lmout("inp: ");
            if(inp == "exit") {
                x = false;
            }
        }
 break and continue are also available:
    while(true) {
        inp = lmout("inp: ");
        if(inp == "exit") {
            break;
        }
        else if(inp == "ruscmi") {
            lmuck(5);
            continue;
        }
    }
that's all.)"<<resbc<<endl;		    
		}
		else if (peer().KEY == TTYPE::STRING && peer().VAL == "types") {
		    advanced();
		    cout<<bluec<<R"(
		        basic data types on lmnlang

  STR - string
example: x = "Hello,world!";

  NUM - number
example: x = 6; or just 6; (this is also number lol yes babe)

  BOOL - bool
example: x = true; 

  ARR - arrays
example: x = [5,5];

that's all data types)"<<resbc<<endl;
		}
		else if (peer().KEY == TTYPE::STRING && peer().VAL == "math") {
		advanced();
		cout<<bluec<<R"(	What can lmnlang do in math?
 Operations: addition, subtraction, multiplication and division
 Examples: 
 	5 + 5 (will output the result)
 	5 * 5
 	5 / 5
 	5 - 5
 Actions with multiple numbers and operations,example:
 	5 + 5 * 5 / 5 (will output the result)
 Actions with parentheses (priorities),example:
 	5 * ( 5 + 5 ) / 5 + 5
 There is also support for floating-point numbers,example:
    5.5 - ( 1.5 / 1 ) / 5.7
 There is support for the unary minus
    5 - -5 * ( 2 - 1 ) + 8.3
 And, of course, variable support
    5 - x * x + 8
 (will work if any value is set for 'x')
 At this time, it's all thanks for viewing this part of the manual.)"<<resbc<<endl;
		}
		else if(peer().KEY == TTYPE::STRING && peer().VAL == "assignment") {
			advanced();
			cout<<bluec<<R"(	How to use assignment?
 You can create variables as follows:
 	x = 5 
     but no 
      5 = x
 cannot assign strings to numbers
 You can assign whole lines if you put them in quotation marks
  examples: 
  	x = "hello,world" 
  	  x = "500" (it will be like a string, not a number)
  You can assign a variable to a different variable
   examples:
  	 y = 5
  	  x = y
  when displayed, it will output the values of the previous variable(y)

  You can also store entire algebraic calculations in variables
    examples:
     x = 5 + 5 - 5
      y = 5 * ( 5 + 5 )
  when the variable is displayed, it will output a calculation

  you can also create several variables with the same value at once
    x,y = 5; lmuck x,y
        nano,micro = "lmnlang";
        lmuck(nano)        
  math also supports finding the remainder using the '%' operator:
    x = 5 % 2;
    lmuck(x);
  Thank you for reading this part of the manual!)"<<resbc<<endl;
		}
		else if(peer().KEY == TTYPE::STRING && peer().VAL == "print") {
			advanced();
			cout<<bluec<<R"(	I want to say right away! 
 In this language, the 'lmuck' + <args> command (short for lmn druck) is used to output strings
 	How do i output variables?
 Let's say you created a variable y with a value of 5
  example:
  	y = 5
  You can withdraw it by simply writing lmuck(y)
   example:
   	lmuck(y)
  How do I output numbers?
   	lmuck(5)
   	 lmuck(50)
   example:
   	  lmuck(500)
   will give you the numbers.
   How do I print strings?
   	use a quotes for print strings
   	 example:
   	  lmuck("hello,world")
   	will bring you 'hello,world'
   It is also possible to output the results of algebraic calculations.
    example:
     lmuck(5 + 5 * 5 / 5)
   will bring you answer
   you can output multiple expressions, variables, or strings
    example:
        x,y = 5; lmuck("my dick ",x," sm "," my anus ",y," sm");
   That's all, thank you for viewing this part of the manual.)"<<resbc<<endl;
		}
		else if(peer().KEY == TTYPE::STRING && peer().VAL == "runner") {
			advanced();
			cout<<bluec<<R"(	How do I run files in lmnlang?
 NOTE: There is also a brief manual available by running './lmnlang --man'
  For the REPL mode, you simply run the built executable ('./lmnlang').
  To launch files, you will need to enter "--file"	 
	  example:
	  	./lmnlang --file <file_name.lmn>
	  NOTE: Only files with the ".lmn" extension can be launched.
	  Additionally, you must either create your source
	   code file within the directory containing the 
	    built binary (`./lmnlang`) or run it from a common directory 
	  as follows:
	  	./build/lmnlang --file <file_name.lmn>
	  	or by specifying an explicit directory
	  	./build/lmnlang --file <directory_name/file_name.lmn>
 What is the difference between the runner and the REPL mode?
   Instead of entering code line by line, 
    you can enter it into a separate source file; 
     this is much more convenient and is considered standard practice.
 Thank you for viewing this part of the manual.
     )"<<resbc<<endl;
		}
        else if(peer().KEY == TTYPE::STRING && peer().VAL == "arrays") {
            advanced();
            cout<<bluec<<R"(    You use square brackets to work with arrays.
  create an array examples:
    x = [5,10]
    x = ["Hello","world"]
 or you can use numbers and strings
    y = [15,"Hello,world"]
    y = ["lmnlang", 20]
 you can store variables in an array.
    example:
        y = 5; x = [y,"lemon"];
        z = "hello"; m = 543; x = [z,m];
  outputting array elements
    example:
        y = [5,2,1,6,0]; lmuck y[1];
    it will output the number for you '2'
  Nested arrays can be used.
    example:
        y = [5,10]; z = ["Hello,world","Lemon"];
        m = [y,z]
  You can create nested arrays in a single row
    example:
        x = [5,[5,5],5,[555,555,[555,555],555],555];  
  Numbers, strings, arrays, and bools can also be pushed into arrays.
  example:
        x = []; x.lmpush(5); x.lmpush(6); lmuck(x[1]) // outputs 6
  that’s all—thanks for reading.
    )"<<resbc<<endl;
        }
        else if(peer().KEY == TTYPE::STRING && peer().VAL == "comparisons") {
            advanced();
            cout<<bluec<<R"(   to use the conditions(if,else) 
 you can use C-like syntax.
 Here are some use cases.
  -Using Regular Blocks and Simple Conditions-
    example:
     x = 5; if(x == 5) {
        lmuck(x)
     };
      will output 5
      
     y = 6; if(y == x) {
        lmuck(y,x)
     }else {
        lmuck("string")
     };
      will output 'string'
     
     m = 6; if(m == y && y != 5 || y != 8) {
        lmuck("Hello,world!")
     }else {
        lmuck("Error :/")
     }
      will output 'Hello,world!' (learn boolea algebric)
  -Using 'else if'  and composing multiple conditions-
    example:
     x = 5; y = 6;
     if(x == 5) { 
        lmuck(x)
     }else if(x == 6) {
        lmuck(y)
     }else if(y == 6) {
        lmuck(x)
     }else {
        lmuck(x,y)
     }
      will output 5
     In general, in order not to make the manual even more, I will say this, 
      in my programming language you can make many more conditions, 
       it depends on your fantasies.)"<<resbc<<endl;
        }
        else if(peer().KEY == TTYPE::STRING && peer().VAL == "input") {
                    advanced();
                    cout<<bluec<<R"( The 'lmout' syntax is used to use input
 Basic examples and logic:
    lmout() <-- prompts you to enter a string
    lmout("how are you?: ") <-- but it will not retain its significance. 
    name = lmout("input your name: ") <-- will request and store the value in the 'name' variable 
    How to use in code?

/-example.lmn-/
name = lmout("name: ");
if(name == "ruscmi") {
    lmuck(name," the owner")
}
else lmuck(name) 
In general, that's all, the use is very simple, 
everything will depend on your fantasies.)"<<resbc<<endl;
        }
		else {
			error("use man + <arg> or write 'man list'");
			advanced();
		}
	}
	return node;
}
