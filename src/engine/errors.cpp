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
		out += to_string(*it);
		if(i+1 != got.size()) out += ",";
		i++;
	}

	out += "]";
	this->message = out;
}
const char* ExpectStackError::what() const noexcept {
	return this->message.c_str();
}

