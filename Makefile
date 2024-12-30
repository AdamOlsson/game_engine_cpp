CXX = g++
VULKAN_SDK = /Users/adamolsson/VulkanSDK/1.3.296.0/macOS
VULKAN_INCLUDE_PATH = $(VULKAN_SDK)/include
VULKAN_LIBRARY_PATH = $(VULKAN_SDK)/lib
VULKAN_LIBRARY = vulkan.1

BREW_PATH = /opt/homebrew
BREW_INCLUDE_PATH = $(BREW_PATH)/include
BREW_LIBRARY_PATH = $(BREW_PATH)/lib
GLFW_LIBRARY = glfw 

PUBLIC_INCLUDE_PATH = include/
PRIVATE_INCLUDES = src/

SRC := $(shell find src -name '*.cpp')
HEADERS := $(shell find include -name '*.h')
OBJ := $(patsubst src/%.cpp,obj/game_engine/%.o,$(SRC))
DEP := $(patsubst src/%.cpp,obj/game_engine/%.d,$(SRC))

-include $(DEP)

INCLUDES = -I$(PUBLIC_INCLUDE_PATH) -I$(PRIVATE_INCLUDES) -I$(VULKAN_INCLUDE_PATH) -I$(BREW_INCLUDE_PATH)
LDFLAGS = -L$(VULKAN_LIBRARY_PATH) -l$(VULKAN_LIBRARY) -Wl,-rpath,$(VULKAN_LIBRARY_PATH) \
          -L$(BREW_LIBRARY_PATH) -l$(GLFW_LIBRARY) -Wl,-rpath,$(BREW_LIBRARY_PATH)


CXXFLAGS = -std=c++17 -Wall 

all: game_engine 

game_engine: $(OBJ)
	mkdir -p lib 
	ar rcs lib/libgame_engine.a $(OBJ)

obj/game_engine/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

obj/game_engine/%.d: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -M $< > $@

DEV_DIR = examples/dev/
DEV_SRC := $(shell find $(DEV_DIR) -name '*.cpp')
DEV_OBJ := $(patsubst $(DEV_DIR)%.cpp,obj/dev/%.o,$(DEV_SRC))
dev: game_engine $(DEV_OBJ)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -o bin/dev $(DEV_OBJ) -Llib -lgame_engine $(INCLUDES) $(LDFLAGS)

obj/dev/%.o: examples/dev/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

clean:
	rm -rf bin/ obj/ lib/
