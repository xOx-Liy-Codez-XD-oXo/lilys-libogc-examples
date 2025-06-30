## Basic skinning example - Twisting Cube
  Many Wii and Gamecube applications perform skinning deformations on meshes with the CPU, which allows for fine control, but is costly. The GP supports simple skinning, that is, 1 weight per vertex, where the user loads multiple modelview matricies at the same time, then passes an index into the matrix memory for every vertex. The GP then generates deformed triangles for those whose vertices have different matrix indecies. This comes at nearly 0 cost compared to not skinning at all, and uses nearly 0 CPU time. 

In this example, we create and load 2 matricies, one of them spinning, and we pass indecies with every vertex of a cube to the GP such that the bottom half of the cube accesses the stationary matrix, and the top half accesses the spinning matrix. The middle of the cube twists.

![](/assets/twistingcube.png)
