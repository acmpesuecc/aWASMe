BUILD_DIR := build

INCLUDE_DIR := include
SRC_DIR := src

CXX := g++
CXXFLAGS := -Wall -Wextra -MMD -MP -Iinclude/engine -std=c++20

ENGINE_SRC := $(SRC_DIR)/engine
ENGINE_HEADERS := $(INCLUDE_DIR)/engine

ENGINE_SRC_FILES := $(shell find $(ENGINE_SRC) -name '*.cpp' )
ENGINE_H_FILES := $(patsubst $(ENGINE_SRC)/%.cpp,$(ENGINE_HEADERS)/%.hpp,$(ENGINE_SRC_FILES))
ENGINE_OBJ_FILES := $(patsubst $(ENGINE_SRC)/%.cpp,$(BUILD_DIR)/%.o,$(ENGINE_SRC_FILES))
ENGINE_D_FILES := $(ENGINE_OBJ_FILES:.o=.d)

TARGET := main

all: $(TARGET)

run: all
	./$(TARGET)

$(TARGET): $(ENGINE_OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $(ENGINE_OBJ_FILES) 	

$(BUILD_DIR)/%.o: $(ENGINE_SRC)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONE: all clean


-include $(ENGINE_D_FILES)
