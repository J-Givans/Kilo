# Kilo
Minimalistic, modal text editor

# Dependencies
1. CMake
2. Conan

# Build Instructions
- Clone the repo.
- Install the library dependencies by running `conan install .-if=build --build=missing -pr:b=default`.
- Configure the project by running `cmake -S -B build`
- Compile it by running `cmake --build build`.