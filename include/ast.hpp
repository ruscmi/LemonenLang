/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <unordered_map>
extern bool is_runner;
enum TTYPE { OPERATOR, SEPARATOR, NUMBER, STRING, UNKNOWN,END,STRING_LIT,END_EX,LOGIC_OPERATOR,BOOLEA_OPERATOR,NOT,SPECSYMB};
struct Token {
  TTYPE KEY;
  std::string VAL;
  size_t LINE;
  size_t COL;
};
enum ASTTAB { ST_ASSIGNMENT, ST_NUMBER , ST_OPERATOR, ST_VARIABLE, ST_SEPARATOR, ST_PRINT, ST_STRING, 
ST_NOP, ST_INDEX, ST_ARRAY,ST_INPUT,ST_BLOCK,ST_IF,ST_LOGIC_OPERATOR,ST_STOD,ST_BOOLEA_OPERATOR,ST_BOOL,
ST_PROGRAM,ST_NOT,ST_WHILE,ST_CONTINUE,ST_BREAK,ST_TYPEOF,ST_LEN,ST_ARRAY_PUSH,ST_INCLUDE,ST_INCLUDE_LIBS,
ST_FUNC,ST_CALL,ST_RETURN,ST_WAIT,ST_DICTIONARY,ST_FOR,ST_EMPTY,ST_TOSTR};
struct Node {
  ASTTAB KEY;
  std::string VAL;
  std::unique_ptr<Node>left_index = nullptr;
  std::unique_ptr<Node>right_index = nullptr;
  std::unique_ptr<Node>if_index = nullptr;
  std::unique_ptr<Node>else_index = nullptr;
  std::unique_ptr<Node>block_while = nullptr;
  std::vector<std::unique_ptr<Node>> children;
  std::vector<std::unique_ptr<Node>> right_children;
  size_t line; size_t col;
  Node(Token tok) : line(tok.LINE),col(tok.COL) {};
};
struct ForFunction {
    std::vector<std::string>par_names;
    std::shared_ptr<Node> body;
    bool is_system = false;
    bool operator==(const ForFunction& other) const = default;
};
struct ErrorValue {
    std::string message;
    bool operator==(const ErrorValue& other) const = default;
};
struct ReturnFunc;
struct Continuer { bool operator==(const Continuer& other) const = default; };
struct Breaker { bool operator==(const Breaker& other) const = default; };
struct AcceptValue { bool operator==(const AcceptValue& other) const = default; };
struct ArrayValue;
struct DictValue;
using Value = 
std::variant<
double,
bool,
std::string,
std::shared_ptr<DictValue>,
std::shared_ptr<ArrayValue>,
std::shared_ptr<ReturnFunc>,
ErrorValue,
AcceptValue,
Continuer,
Breaker,
ForFunction>;
struct ReturnFunc {
    Value value;
    bool operator==(const ReturnFunc& other) const {
        return value == other.value;
    }
    bool operator!=(const ReturnFunc& other) const {
        return !(*this == other);
    }
};
struct DictValue {
    std::unordered_map<std::string,Value>dict_val;
    bool operator==(const DictValue& other) const {
        return dict_val == other.dict_val;
    }
    bool operator!=(const DictValue& other) const {
        return !(*this == other);
    }    
};
struct ArrayValue {
    std::vector<Value>elements;
    void push(Value& val) {
        elements.push_back(std::move(val));
    }
    bool operator==(const ArrayValue& other) const {
        return elements == other.elements;
    }
    bool operator!=(const ArrayValue& other) const {
        return !(*this == other);
    }
};
void print_tree(const Node* node,unsigned int level = 0);
void print_array(const Value& val);
