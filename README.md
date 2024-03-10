# Drift Simulation

The drift simulation uses 3D [Perlin noise](https://en.wikipedia.org/wiki/Perlin_noise) to randomly generate a landscape of colors and shapes that evolves over time. This project is inspired by the [macOS Drift screensaver](https://www.youtube.com/watch?v=7WXzRhebz1g&ab_channel=NobelTech). Each line's rotation, length, width, and color are derived from 3D Perlin noise implemented in the shaders.

https://user-images.githubusercontent.com/28791222/223185519-3d3de2b3-48d0-40cd-bcfe-7a30d9a7ac29.mp4

# Building

## Ubuntu 22.04

### Required Libraries 

1) CMake version 3.24.2
2) C/C++ compiler (build-essentials)
3) libglfw3
4) libglfw3-dev
5) libglew
6) libglm-dev

### Steps
1) Open a terminal window
2) Clone this repository using `git clone https://github.com/mikee478/drift-simulation.git`
3) Install the required libraries above using 
`sudo apt-get install cmake build-essential libglfw3 libglfw3-dev libglew-dev libglm-dev`
4) Run the command `cmake -B build -S .` to create the `build` folder and to specify where the source directory is
5) Run the command `cmake --build build` to build the application
6) Finally go to the folder `build` and double click on `drift-simulation` to run the application
