BUILD_DIR := build
INCLUDE_DIR := include
SRC_DIR := src

CXX := g++

CXXFLAGS := -Wall -Wextra -MMD -MP -std=c++20 -I$(INCLUDE_DIR)

ENGINE_SRC := $(SRC_DIR)/engine
ENGINE_SRC_FILES := $(shell find $(ENGINE_SRC) -name '*.cpp' )
ENGINE_OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(ENGINE_SRC_FILES))
ENGINE_D_FILES := $(ENGINE_OBJ_FILES:.o=.d)

PARSER_SRC := $(SRC_DIR)/parser
PARSER_SRC_FILES := $(shell find $(PARSER_SRC) -name '*.cpp' )
PARSER_OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(PARSER_SRC_FILES))
PARSER_D_FILES := $(ENGINE_OBJ_FILES:.o=.d)

ENGINE_TARGET := engine.exe
PARSER_TARGET := parser.exe

ARGS ?= 	#Default command line arguments to executables is empty

$(ENGINE_TARGET): $(ENGINE_OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $(ENGINE_OBJ_FILES) 	
$(PARSER_TARGET): $(PARSER_OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $(PARSER_OBJ_FILES) 	

run-engine: $(ENGINE_TARGET)
	./$(ENGINE_TARGET) $(ARGS)
run-parser: $(PARSER_TARGET)
	./$(PARSER_TARGET) $(ARGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(ENGINE_TARGET) $(PARSER_TARGET)

.PHONY: run-engine run-parser clean

-include $(ENGINE_D_FILES)
-include $(PARSER_D_FILES)
