git submodule update --init --recursive

cmake --preset clang-x86

cmake --build --preset clang-x86-relwithdebinfo
