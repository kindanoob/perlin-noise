# perlin_noise
C++ implementation of perlin noise using SFML

Requirements: 1) C++11, 2) SFML 2.6.1

The program was tested on Ubuntu 22.04 LTS Jammy, gcc 11.4.0.

Instructions for an out-or-place build:
1) Go to the directory containing the CMakeLists.txt file.
2) Run `cmake -S . -B build`. This will create a `build` directory and put CMake files there.
3) Go to the `build` directory.
4) Build the project (e.g. for Unix makefiles it is achieved by running "make" command in the terminal).
5) Launch the executable file "perlin_noise".


Implementation of perlin noise in C++ using SFML. Algorithm can be tuned to produce variations of perlin noise (like wood texture and others).


If you want to learn more about perlin noise please refer to the following wikipedia article: https://en.wikipedia.org/wiki/Perlin_noise
