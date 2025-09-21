# Try to find LLVM Clang specifically
find_program(LLVM_CLANG_CXX 
    NAMES clang++
    PATHS 
        /usr/local/opt/llvm/bin  # Homebrew on macOS
        /opt/homebrew/opt/llvm/bin  # Homebrew on Apple Silicon
        /usr/bin
        /usr/local/bin
    NO_DEFAULT_PATH
)

if(LLVM_CLANG_CXX)
    set(CMAKE_CXX_COMPILER ${LLVM_CLANG_CXX})
     # Set libc++ for both compilation and linking globally
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++")

    message(STATUS "Using LLVM Clang: ${LLVM_CLANG_CXX}")
else()
    message(FATAL_ERROR "LLVM Clang not found")
endif()

