/* to execute:
 *   make main && ./main.exe <path_to_wasm_file>
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <span>
#include <optional>

#include "module/module.hpp"
#include "module/value.hpp"
#include "parser/parser.hpp"
#include "engine/vm.hpp"
#include "engine/errors.hpp"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "Error: Expected .wasm file path" << std::endl;
		return 1;
	}

	std::vector<uint8_t> data = Loadfile(argv[1]);
	size_t filesize = data.size();
	if (filesize < 8) {
		std::cout << "Error: File too small" << std::endl;
		return 1;
	}

	size_t offset = 8;
	std::span<const uint8_t> dataspan = data;
	std::span<const uint8_t> sectionData;
	size_t secSize;

	Module module{};

	while (offset < filesize) {
		uint8_t id = data[offset];
		++offset;
		secSize = leb128_decode(dataspan, filesize, offset);
		sectionData = dataspan.subspan(offset, secSize);

		switch (id) {
			case 1:
				parse_type_section(sectionData, module);
				break;
			case 2:
				parse_import_section(sectionData, module);
				break;
			case 3:
				parse_func_section(sectionData, module);
				break;
			case 4:
				parse_table_section(sectionData, module);
				break;
			case 5:
				parse_mem_section(sectionData, module);
				break;
			case 6:
				parse_global_section(sectionData, module);
				break;
			case 7:
				parse_export_section(sectionData, module);
				break;
			case 8:
				parse_start_section(sectionData, module);
				break;
			case 9:
				parse_element_section(sectionData, module);
				break;
			case 10:
				parse_code_section(sectionData, module);
				break;
			case 11:
				parse_data_section(sectionData, module);
				break;
			default:
				break;
		}

		offset += secSize;
	}

	try {
		if (module.functions.empty()) {
			std::cout << "Module has no functions" << std::endl;
			return 0;
		}

		const Module::Function& mf = module.functions[0];
		const Module::Type& sig = module.types[mf.typeIndex];

		FunctionInfo fi{};
		fi.block_info.block_start = 0;
		fi.block_info.block_end = mf.code.empty() ? 0 : mf.code.size() - 1;
		if (!sig.returns.empty()) {
			fi.block_info.return_type = sig.returns.front();
		} else {
			fi.block_info.return_type = std::nullopt;
		}
		fi.args = sig.params;
		fi.locals.clear();
		for (int n = 0; n < mf.localCounts.i32; ++n) fi.locals.push_back(ValueType::i32);
		for (int n = 0; n < mf.localCounts.i64; ++n) fi.locals.push_back(ValueType::i64);
		for (int n = 0; n < mf.localCounts.f32; ++n) fi.locals.push_back(ValueType::f32);
		for (int n = 0; n < mf.localCounts.f64; ++n) fi.locals.push_back(ValueType::f64);

		VM vm;
		vm.load(mf.code);

		float a;
		float b;
		int32_t c;

		std::cout << "Enter a (f32): ";
		std::cin >> a;
		std::cout << "Enter b (f32): ";
		std::cin >> b;
		std::cout << "Operation (0 = add, 1 = sub, 2 = mul): ";
		std::cin >> c;

		std::vector<Value> args;
		args.emplace_back(a);
		args.emplace_back(b);
		args.emplace_back(static_cast<int32_t>(c));

		auto result = vm.run_function(fi, args);
		if (!result.has_value()) {
			std::cout << "Function returned no value" << std::endl;
			return 0;
		}

		std::cout << "Result: " << std::get<float>(result.value()) << std::endl;
	} catch (const VMError& e) {
		std::cout << "VM error: " << e.what() << std::endl;
		return 1;
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

