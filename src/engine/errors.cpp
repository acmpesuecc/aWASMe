#include"errors.hpp"

ExpectStackError::ExpectStackError(std::vector<ValueType> expected, std::vector<ValueType> got) { 
	std::string out = "Expected [";

	for(auto it = expected.begin(); it != expected.end(); it++) {
		out += to_string(*it);
		if((it+1) != expected.end()) out += ",";
	}
	out += "], got [";

	for(auto it = got.begin(); it != got.end(); it++)  {
		out += to_string(*it);
		if((it+1) != got.end()) out += ",";
	}

	out += "]";
	
	this->message = out;
}

ExpectStackError::ExpectStackError(std::vector<ValueType> expected, std::vector<Value> got) {
	std::string out = "Expected [";

	int i =0;
	for(auto it = expected.begin(); it != expected.end(); it++) {
		out += to_string(*it);
		if(i+1 != expected.size()) out += ",";
		i++;
	}
	out += "], got [";
	i = 0;

	for(auto it = got.begin(); it != got.end(); it++)  {
		out += to_string(to_value_type(*it));
		if(i+1 != got.size()) out += ",";
		i++;
	}

	out += "]";
	this->message = out;
}
const char* ExpectStackError::what() const noexcept {
	return this->message.c_str();
}

UnexpectedInstruction::UnexpectedInstruction(Instruction i,size_t ip) {
	 this->message = "Unexpected instruction " + i.to_string() + " at index " + std::to_string(ip);
	
}

const char * UnexpectedInstruction::what() const noexcept {
	return this->message.c_str();
}

const char * StackUnderflowError::what() const noexcept {
	return this->message.c_str();
}


VMError::VMError(std::exception_ptr e): message(""), inner(e) {

	try { this->throw_inner(); } catch(std::exception& e) {
	this->what_string = e.what();
	}
}

VMError::VMError(std::string msg,std::exception_ptr e): message(msg), inner(e) {
	try { this->throw_inner(); } catch(std::exception& e) {
		this->what_string = msg+":\n\t\t"+e.what();
	}
}

void VMError::throw_inner() const  {
	if(this->inner) std::rethrow_exception(this->inner);
}

const char* VMError::what() const noexcept {
	return this->what_string.c_str();
}
