/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/parser.hpp"
#include <iostream>
#include "../include/utf8_win.hpp"
/*
//ru//
перенес мануал сюда потому что он разрастается и мешает в парсере,
а так его можно будет просто инклудить и вызывать в рекурсионном спуске внутри парсера
//en//
moved the manual here because it grows and interferes in the parser, 
otherwise it can simply be inserted and called in recursion descent inside the parser
*/
Node* Parser::parse_manual() {
	setup_utf8();
	Token current = peer();
	const char* bluec = "\033[34m";
	const char* resbc = "\033[0m";
	Node* node = new Node();
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
 input - see examples using input)"<<resbc<<endl;
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
        lmuck nano        
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
  You can withdraw it by simply writing lmuck y
   example:
   	lmuck y
  How do I output numbers?
   	lmuck 5 
   	 lmuck 50
   example:
   	  lmuck 500
   will give you the numbers.
   How do I print strings?
   	use a quotes for print strings
   	 example:
   	  lmuck "hello,world"
   	will bring you 'hello,world'
   It is also possible to output the results of algebraic calculations.
    example:
     lmuck 5 + 5 * 5 / 5
   will bring you answer
   you can output multiple expressions, variables, or strings
    example:
        x,y = 5; lmuck "my dick"," ",x," ","sm"," ","my anus"," ",y," ","sm";
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
  that’s all—thanks for reading.
    )"<<resbc<<endl;
        }
        else if(peer().KEY == TTYPE::STRING && peer().VAL == "comparisons") {
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
			cout<<"\033[1;33mW: use man <arg> and <list> to see args\033[0m"<<endl;
			advanced();
		}
	}
	return node;
}
