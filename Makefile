BUILD_DIR := build
INCLUDE_DIR := include
SRC_DIR := src

CXX := g++

CXXFLAGS := -Wall -Wextra -MMD -MP -std=c++20 -I$(INCLUDE_DIR)

ENGINE_SRC := $(SRC_DIR)/engine
ENGINE_SRC_FILES := $(shell find $(ENGINE_SRC) -name '*.cpp' )
ENGINE_OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(ENGINE_SRC_FILES))
ENGINE_D_FILES := $(ENGINE_OBJ_FILES:.o=.d)

ENGINE_MAIN_OBJ := $(BUILD_DIR)/engine/main.o
ENGINE_OBJ_NO_MAIN := $(filter-out $(ENGINE_MAIN_OBJ),$(ENGINE_OBJ_FILES))

PARSER_SRC := $(SRC_DIR)/parser
PARSER_SRC_FILES := $(shell find $(PARSER_SRC) -name '*.cpp' )
PARSER_OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(PARSER_SRC_FILES))
PARSER_D_FILES := $(ENGINE_OBJ_FILES:.o=.d)

PARSER_MAIN_OBJ := $(BUILD_DIR)/parser/main.o
PARSER_OBJ_NO_MAIN := $(filter-out $(PARSER_MAIN_OBJ),$(PARSER_OBJ_FILES))

MAIN_OBJ := $(BUILD_DIR)/main.o
MAIN_D_FILES := $(MAIN_OBJ:.o=.d)

ENGINE_TARGET := engine.exe
PARSER_TARGET := parser.exe
MAIN_TARGET := main.exe

ARGS ?= 	#Default command line arguments to executables is empty

engine: $(ENGINE_TARGET)
parser: $(PARSER_TARGET)

$(ENGINE_TARGET): $(ENGINE_OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $(ENGINE_OBJ_FILES) 	
$(PARSER_TARGET): $(PARSER_OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $(PARSER_OBJ_FILES) 	
$(MAIN_TARGET): $(MAIN_OBJ) $(ENGINE_OBJ_NO_MAIN) $(PARSER_OBJ_NO_MAIN)
	$(CXX) $(CXXFLAGS) -o $@ $(MAIN_OBJ) $(ENGINE_OBJ_NO_MAIN) $(PARSER_OBJ_NO_MAIN)

run-engine: $(ENGINE_TARGET)
	./$(ENGINE_TARGET) $(ARGS)
run-parser: $(PARSER_TARGET)
	./$(PARSER_TARGET) $(ARGS)
main: $(MAIN_TARGET)

WEB_DIR := demo/web
_ALL_WEB_SRC := main.cpp $(shell find $(ENGINE_SRC) $(PARSER_SRC) -name '*.cpp')
WEB_SRC_FILES := $(filter-out $(ENGINE_SRC)/main.cpp $(PARSER_SRC)/main.cpp,$(_ALL_WEB_SRC))
EMCC := emcc
EMCCFLAGS := -std=c++20 -I$(INCLUDE_DIR) \
             --pre-js demo/args.js \
             --preload-file demo/calculator.wasm@calculator.wasm \
             -s FORCE_FILESYSTEM=1

web: $(WEB_DIR)/index.html

$(WEB_DIR)/index.html: $(WEB_SRC_FILES)
	@mkdir -p $(WEB_DIR)
	$(EMCC) $(EMCCFLAGS) $(WEB_SRC_FILES) -o $(WEB_DIR)/index.html


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: main.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(ENGINE_TARGET) $(PARSER_TARGET) $(MAIN_TARGET) $(WEB_DIR)

.PHONY: run-engine run-parser clean main web

-include $(ENGINE_D_FILES)
-include $(PARSER_D_FILES)
-include $(MAIN_D_FILES)
