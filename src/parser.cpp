/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/parser.hpp"
#include "../include/utf8_win.hpp"
#include "../include/ast.hpp"
bool is_runner = false;
#include <iostream>
void Parser::setTokens(const vector<Token>& tokenize) {
	this->tokenize = tokenize;
	this->position = 0;
}
Token Parser::peer() {
	if(position >= tokenize.size()) {
		return Token{TTYPE::END,""};
	}
	return tokenize[position];
}
Token Parser::advanced() {
	if(position < tokenize.size()) {
		position++; 
	}
	return tokenize[position -1];
}
Value Parser::evaluate(Node* node) {
	setup_utf8();
	if(!node) { return AcceptValue{}; }
	if(node->KEY == ST_NUMBER) {
		return stod(node->VAL);
	}
	else if(node->KEY == ST_NOP) {
		return AcceptValue{};
	}
	else if(node->KEY == ST_OPERATOR && node->VAL == "u-") {
		Value val = evaluate(node->right_index);
        if(holds_alternative<double>(val)) {
            return -get<double>(val);
        }else {
            cout<<"\033[1;31mE: unary minus requires a number\033[0m"<<endl;
            return ErrorValue{"E: unary minus requires a number"};
        }
	}
	else if(node->KEY == ST_ARRAY) {
        auto arr = make_shared<ArrayValue>();
        for(Node* element : node->children) {
            Value val = (evaluate(element));
            if(holds_alternative<ErrorValue>(val)) {return val;}
            arr->elements.push_back(val);
        }
        return arr;
    }
    else if(node->KEY == ST_INDEX) {
        const Value left_val = evaluate(node->left_index);
        const Value right_val = evaluate(node->right_index);
        if(!holds_alternative<shared_ptr<ArrayValue>>(left_val)) {
           cout<<"\033[1;31mE: dont have ST_ARRAY\033[0m"<<endl;
           return ErrorValue {"E: dont have ST_ARRAY"};
        }
        if(!holds_alternative<double>(right_val)) {
            cout<<"\033[1;31mE: dont have ST_INDEX in ST_ARRAY\033[0m"<<endl;
            return ErrorValue {"E: dont have ST_INDEX in ST_ARRAY"};
        }
        auto& left = get<shared_ptr<ArrayValue>>(left_val);
        double index_double = get<double>(right_val);
        if(index_double != (int)index_double) {
            cout<<"\033[1;31mE: array index must be integer fucked kid\033[0m"<<endl;
            return ErrorValue{"E: array index must be integer fucked kid"};
        }
        int right = (int)index_double;
        if(right < 0 || (size_t)right >= left->elements.size()) {
            cout<<"\033[1;31mE: ST_INDEX < 0 || ST_INDEX > ST_ARRAY.size()\033[0m"<<endl;
            return ErrorValue {"E: ST_INDEX < 0 || ST_INDEX > ST_ARRAY.size()"};
        }
        return left->elements[right];
    }
    else if(node->KEY == ST_STRING) {
        return node->VAL;
    }
	else if(node->KEY == ST_VARIABLE) {
		const string name = node->VAL;
		const auto it = vars.find(name);
		if (it != vars.end()) {
		    return (it->second);
		}
		cout<<"\033[1;31mE: Variable not found\033[0m"<<endl;
		return ErrorValue {"E: Variable not found"};
	}
	else if(node->KEY == ST_OPERATOR) {
		const Value left_val = evaluate(node->left_index);
		const Value right_val = evaluate(node->right_index);
		if(holds_alternative<ErrorValue>(left_val)) { return left_val; }
		if(holds_alternative<ErrorValue>(right_val)) { return right_val; }
		if(!holds_alternative<double>(left_val) || !holds_alternative<double>(right_val)) {
		    cout<<"\033[1;31mE: operator requires numbers stupid people\033[0m"<<endl;
		    return ErrorValue {"E: operator requires numbers stupid people"};
		}
        double left = get<double>(left_val);
        double right = get<double>(right_val);
		string op = node->VAL;
		if(op == "+") { double mo = left + right; return mo; }
		else if(op == "-") { double mo = left - right; return mo; }
		else if(op == "/") {
			if(right != 0) {
				double mo = left / right;  return mo; 
			}
			else {
			    cout<<"\033[1;31mE: cannot be divided by fucked zero\033[0m"<<endl;
				return ErrorValue{"E: cannot be divided by fucked zero"};
			}
		}				
		else if(op == "*") { double mo = left * right;  return mo;  }
	}
	else if(node->KEY == ST_ASSIGNMENT) {
        Value result = evaluate(node->right_index);
        if(holds_alternative<ErrorValue>(result)) { return result; }
        if(!node->children.empty()) {
            for(const auto& node : node->children) {
                vars[node->VAL] = result;
            }
        }else {
            cout<<"\033[1;31mE: node->children is empty() fucking mudda\033[0m"<<endl;
            return ErrorValue {"E: node->children is empty() fucking mudda"};
        }
        return AcceptValue{};
    }
	else if(node->KEY == ST_PRINT) {
	    for(size_t i = 0 ; i < node->children.size(); ++i) {
    	    Value val = evaluate(node->children[i]);
            if(holds_alternative<ErrorValue>(val)) { return val ; }
    	    print_array(val);
        }
        cout<<endl;
        return AcceptValue{};
	}
	else {
	    cout<<"\033[1;31mE: unknown Value Parser::evaluate() type\033[0m"<<endl;
		return ErrorValue{"E: unknown Value Parser::evaluate() type"};
	}
	return ErrorValue{"C.E: evaluate() return critical error"};
}
Node* Parser::parse_program() {
	setup_utf8();
	for(const auto& token : tokenize ) {
		if(token.KEY == TTYPE::UNKNOWN ) {
			cout<<"\033[1;31mE: Deer,my parser doesn't understand this shit\033[0m"<<endl;
			return nullptr;
		}
	}
	while(true) {
		Node* expr = parse_statement();
		if(!expr) {
			if(peer().KEY == TTYPE::END) {
				break;
			}
			cout<<"\033[1;33mE: excepted expression\033[0m"<<endl;
			return nullptr;
		}
		bool s_ok = false;
		if(peer().KEY == TTYPE::END_EX) {
		    advanced();
		    s_ok = true;
		}
		else if(peer().KEY == TTYPE::END) {
		    s_ok = true;
		}
		else {
		    cout<<"\033[1;33mE: excepted ';' or end of input\033[0m"<<endl;
            delete expr;
		    return nullptr;
		}
		if(s_ok) {
		    Value res = evaluate(expr);
		    if(holds_alternative<ErrorValue>(res)) {
		        if(peer().KEY == TTYPE::END) {
		            break; continue;
		        }
		    }
		    if(holds_alternative<AcceptValue>(res)) {
		        if(peer().KEY == TTYPE::END) {
		            break; continue;
		        }
		    }
		    print_array(res);
		    cout<<endl;
		}
	}
	return nullptr;
}
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
 arrays - see examples of working with arrays)"<<resbc<<endl;
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
		else {
			cout<<"\033[1;33mW: use man <arg> and <list> to see args\033[0m"<<endl;
			advanced();
		}
	}
	return node;
}
Node* Parser::parse_print() {
	setup_utf8();
	advanced();
	Node* print_node = new Node();
	print_node->KEY = ST_PRINT;
	print_node->VAL = "lmuck";
	print_node->left_index = nullptr;
	print_node->right_index = nullptr;
	Node* expr = parse_expression();
	if(expr == nullptr) {
		cout<<"\033[1;33mE: you didn't add the arguments fucked mudda\033[0m"<<endl;
        delete print_node;
		return nullptr;
	}
	print_node->children.push_back(expr);
	while(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
        advanced();
        Node* t_expr = parse_expression();
        if(!t_expr) {
            cout<<"\033[1;33mE: you didn't add the arguments fucked mudda\033[0m"<<endl;
            return nullptr;
        }
        print_node->children.push_back(t_expr);
    }
	return print_node;
}
Node* Parser::parse_statement() {
	Token current = peer();
	if (current.KEY == TTYPE::STRING && current.VAL == "lmuck") {
	    return parse_print();
	}
	else if (current.KEY == TTYPE::STRING && current.VAL == "man") {
		return parse_manual();
	}
	else if(current.KEY == TTYPE::STRING) {
	    bool is_assignment = false;
	    size_t pos_expr = position;
	    while(pos_expr < tokenize.size() && tokenize[pos_expr].KEY != TTYPE::END_EX) {
	        if(tokenize[pos_expr].KEY == TTYPE::OPERATOR && tokenize[pos_expr].VAL == "=") {
	            is_assignment = true;
	            break;
	        }
	        pos_expr++;
	    }
	    if(is_assignment) {
	        return parse_assignment();
	    }else {
	        return parse_expression();
	    }
	}
	else {
		return parse_expression();
	}
}
Node* Parser::parse_assignment() {
    vector<Node*>elements;
    while(true) {
        if(peer().KEY != TTYPE::STRING) {
            cout<<"\033[1;33mE: peer().KEY != TTYPE::STRING(thong)\033[0m"<<endl;
            return nullptr;
        }
        Node* var_node = new Node();
        var_node->KEY = ST_VARIABLE;
        var_node->VAL = peer().VAL;
        advanced();
        elements.push_back(var_node);
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
            advanced();
            continue;
        }
        else if(peer().KEY == TTYPE::OPERATOR && peer().VAL == "=") {
            break;
        }
        else {
            cout<<"\033[1;33mE: UNKNOWN::TTYPE in assignmentation  \033[0m"<<endl;
            return nullptr; // пиздец ебаный в рот че
        }
    }
    advanced();
    Node* right_expr = parse_expression();
    if(!right_expr) {
        cout<<"\033[1;33mE: excepted TTYPE::OPERATOR \"=\" \033[0m"<<endl;
        for(Node* node : elements) {
            delete node;
        }
        return nullptr;
    }
    Node* assign = new Node();
    assign->KEY = ST_ASSIGNMENT;
    assign->VAL = "=";
    assign->left_index = nullptr;
    assign->right_index = right_expr;
    assign->children = elements;
    return assign;
}
Node* Parser::parse_factor() {
	setup_utf8();
	Token current = peer();
	Node* left = nullptr;
	if(current.KEY == TTYPE::NUMBER) {
		Node* node = new Node();
		node->KEY = ST_NUMBER;
		node->VAL = current.VAL;
		advanced();
		left = node;
	}
	else if(current.KEY == TTYPE::STRING) {
		Node* node = new Node();
		node->KEY = ST_VARIABLE;
		node->VAL = current.VAL;
		advanced();
		left = node;
	}
    else if(current.KEY == TTYPE::SEPARATOR && current.VAL == "[") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
            advanced();
            Node* node = new Node();
            node->KEY = ST_ARRAY;
            node->children = {};
            left = node;
            return left;
        }
        if(peer().KEY == TTYPE::END) {
            return nullptr;
        }
        vector<Node*>elements;
        while(true) {
            Node* element = parse_expression();
            if(!element) {
                cout<<"\033[1;33mE: cant parsing array,detection TTYPE::UNKNOWN in array\033[0m"<<endl;
                return nullptr;
            }
            elements.push_back(element);
            
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
                advanced();
                continue;
                if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
                    cout<<"\033[1;33mE: you shit it, the next comma in the array is forbidden\033[0m"<<endl;
                    return nullptr;
                }
            }
            else if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
                break;
            }else {
                cout<<"\033[1;33mE: excepted stupid parenthesis in fucked ST_ARRAY\033[0m"<<endl;
                return nullptr;
            }
        }
        Node* node = new Node();
        node->KEY = ST_ARRAY;
        node->children = elements;
        advanced();
        left = node;
    }
	else if(current.KEY == TTYPE::STRING_LIT) {
		Node* node = new Node();
		node->KEY = ST_STRING;
		node->VAL = current.VAL;
		advanced();
		left = node;
	}
	else if(current.KEY == TTYPE::END_EX) {
		advanced();
		return nullptr;
	}
	else if(current.KEY == TTYPE::OPERATOR && current.VAL == "-") {
		advanced();
		Node* right = parse_factor();
		Node* node = new Node();
		node->KEY = ST_OPERATOR;
		node->VAL = "u-";
		node->left_index = nullptr;
		node->right_index = right;
		left = node;
	}
	else if(current.KEY == TTYPE::SEPARATOR && current.VAL == "\"") {
		advanced();
		Node* inner = parse_expression();
		if(peer().KEY == TTYPE::SEPARATOR && current.VAL == "\"") {
			advanced();
			return inner;
		}
		else {
			cout<<"\033[1;33mE: small dick on the quotes, excepted '\"'\033[0m"<<endl;
			return nullptr;
		}
	}
	else if(current.KEY == TTYPE::SEPARATOR && current.VAL == "(" ) {
		advanced();
		Node* inner = parse_expression();
		if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")" ) {
			advanced();
			return inner;
		}
		else {
			cout<<"\033[1;33mE: small tits on the brackets, excepted ')'\033[0m"<<endl;
			return nullptr;
		}
	}
	else {
		return nullptr;
	}
	if(!left) {return nullptr;}
	while(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "[") {
	    advanced();
	    Node* expr = parse_expression();
	    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
	        advanced();
     	    Node* node = new Node();
     	    node->KEY = ST_INDEX;
     	    node->left_index = left;
     	    node->right_index = expr;
     	    left = node;
	    }else {
	        cout<<"\033[1;33mE: excepted your brain or ']' in index\033[0m"<<endl;
	        return nullptr;
	    }
	}
	return left;
}
Node* Parser::parse_term() {
	Node* left = parse_factor();
	if(left == nullptr) {
		return nullptr;
	}
	Token current = peer();
	while(current.KEY == TTYPE::OPERATOR &&  (current.VAL == "*" || current.VAL == "/")) {
		const string current_op = current.VAL;
		advanced();
		Node* right = parse_factor();
		if(right == nullptr) {
		    cout <<"\033[1;33mE: missing right operand for operator '" << current_op << "'\033[0m" << endl;
		    if(left != nullptr) {
		        delete left;
		    }
			return nullptr;
		}
		Node* node = new Node();
		node->KEY = ST_OPERATOR;
		node->left_index = left;
		node->right_index = right;
		node->VAL = current_op;
		left = node;
		current = peer();
	}
	return left;
}
Node* Parser::parse_expression() {
	Node* left = parse_term();
	if(left == nullptr) {
		return nullptr;
	}
	Token current = peer();
	while(current.KEY == TTYPE::OPERATOR && (current.VAL == "+" || current.VAL == "-")) {
		const string current_op = current.VAL;
		advanced();
		Node* right = parse_term();
		if(right == nullptr) {
		    cout <<"\033[1;33mE: missing right operand for operator '" << current_op << "'\033[0m" << endl;
		    if(left != nullptr) {
		        delete left;
		    }
			return nullptr;
		}
		Node* node = new Node();
		node->KEY = ST_OPERATOR;
		node->left_index = left;
		node->right_index = right;
		node->VAL = current_op;
		left = node;
		current = peer();
	}
	return left;
}
