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

TARGET = main
SRC := $(shell find src -name '*.cpp')

# $(info SOURCES: $(SRC))

CXXFLAGS = -std=c++17 -Wall 

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o bin/$(TARGET) \
		-I$(PUBLIC_INCLUDE_PATH) \
		-I$(PRIVATE_INCLUDES) \
		-I$(VULKAN_INCLUDE_PATH) \
		-L$(VULKAN_LIBRARY_PATH) \
		-l$(VULKAN_LIBRARY) \
		-Wl,-rpath,$(VULKAN_LIBRARY_PATH) \
		-I$(BREW_INCLUDE_PATH) \
		-L$(BREW_LIBRARY_PATH) \
		-l$(GLFW_LIBRARY) \
		-Wl,-rpath,$(BREW_LIBRARY_PATH)

clean:
	rm -f bin/$(TARGET)
