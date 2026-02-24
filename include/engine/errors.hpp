#ifndef ERRORS_H
#define ERRORS_H

#include<string>
#include<exception>
#include<vector>

#include"value.hpp"
#include"instruction.hpp"

class ExpectStackError: public std::exception {
	private:
		std::string message;
	public:
		ExpectStackError(std::string& msg): message(msg) {};
		ExpectStackError(std::vector<ValueType> expected, std::vector<ValueType> got);
		ExpectStackError(std::vector<ValueType> expected, std::vector<Value> got);
		const char* what() const noexcept; 
};

/// Arises when you try to pop elements more than the base of the stack
class StackUnderflowError: public std::exception {
	private:
		std::string message;
	public:
		StackUnderflowError(): message("Stack pointer exceed lower bound") {};
		StackUnderflowError(std::string& msg): message(msg) {};
		const char* what() const noexcept; 
};

/// Thrown when the instruction wasn't expected here
class UnexpectedInstruction: public std::exception {
	private:
		std::string message;
	public:
		UnexpectedInstruction(std::string& msg): message(msg) {};
		UnexpectedInstruction(Instruction i,size_t ip);
		const char* what() const noexcept; 
};

class VMError: public std::exception {
	private:
		std::string message;
		std::exception_ptr inner;
		std::string what_string; // what will be finally sent when what() is called. Combines the messages passed as well as the what() message of the inner exception.
	public:
		VMError(std::string msg): message(msg), inner(nullptr) {};
		VMError(std::exception_ptr e);
		VMError(std::string msg, std::exception_ptr e);

		void throw_inner() const;
		const char* what() const noexcept;
};
#endif
