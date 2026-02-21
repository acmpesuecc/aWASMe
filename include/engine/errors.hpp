#ifndef ERRORS_H
#define ERRORS_H

#include<string>
#include<vector>

#include"value.hpp"

	class ExpectStackError: public std::exception {
		private:
			std::string message;
		public:
			ExpectStackError(std::string& msg): message(msg) {};
			ExpectStackError(std::vector<ValueType> expected, std::vector<ValueType> got);
			ExpectStackError(std::vector<ValueType> expected, std::vector<Value> got);
			const char* what() const noexcept; 
	};
#endif
