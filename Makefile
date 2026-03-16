BUILD_DIR := build

INCLUDE_DIR := include
SRC_DIR := src

CXX := g++
CXXFLAGS := -Wall -Wextra -MMD -MP -Iinclude/engine -Iinclude/parser -Iinclude/module -std=c++20


ENGINE_SRC := $(SRC_DIR)/engine
ENGINE_HEADERS := $(INCLUDE_DIR)/engine


PARSER_SRC :=$(SRC_DIR)/parser
PARSER_HEADERS :=$(INCLUDE_DIR)/parser


ifeq ($(OS),Windows_NT)
	ENGINE_SRC_FILES := $(wildcard $(ENGINE_SRC)/*.cpp $(ENGINE_SRC)/**/*.cpp)
    PARSER_SRC_FILES := $(wildcard $(PARSER_SRC)/*.cpp $(PARSER_SRC)/**/*.cpp)
else
	ENGINE_SRC_FILES := $(shell find $(ENGINE_SRC) -name '*.cpp' )
	PARSER_SRC_FILES :=$(shell find $(PARSER_SRC) -name '*.cpp')
endif
ENGINE_H_FILES := $(patsubst $(ENGINE_SRC)/%.cpp,$(ENGINE_HEADERS)/%.hpp,$(ENGINE_SRC_FILES))
ENGINE_OBJ_FILES := $(patsubst $(ENGINE_SRC)/%.cpp,$(BUILD_DIR)/%.o,$(ENGINE_SRC_FILES))
ENGINE_D_FILES := $(ENGINE_OBJ_FILES:.o=.d)

PARSER_H_FILES :=$(patsubst $(PARSER_SRC)/%.cpp,$(PARSER_HEADERS)/%.hpp,$(PARSER_SRC_FILES))
PARSER_OBJ_FILES := $(patsubst $(PARSER_SRC)/%.cpp,$(BUILD_DIR)/%.o,$(PARSER_SRC_FILES))
PARSER_D_FILES :=$(PARSER_OBJ_FILES:.o=.d)



TARGET := main
PARSER_TARGET := parser

all: $(TARGET) $(PARSER_TARGET)

run: all
	./$(PARSER_TARGET)
	./$(TARGET) 

$(TARGET): $(ENGINE_OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $(ENGINE_OBJ_FILES) 	

$(PARSER_TARGET): $(PARSER_OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $(PARSER_OBJ_FILES) 	

$(BUILD_DIR)/%.o: $(ENGINE_SRC)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(PARSER_TARGET)

.PHONY: all clean


-include $(ENGINE_D_FILES)
-include $(PARSER_D_FILES)
