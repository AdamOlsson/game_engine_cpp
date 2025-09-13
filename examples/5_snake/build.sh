mkdir -p build
cmake -B build -G Ninja && cmake --build build -- -j$(nproc)
