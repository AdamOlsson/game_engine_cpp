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
