#include"funcs.hpp"


std::optional<ValueType> ActivationRecord::get_return_type() {
	return this->info.block_info.return_type;	
}
