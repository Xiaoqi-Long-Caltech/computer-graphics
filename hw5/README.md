### CS171 HW5

To compile, run “make” and run the executable with given arguments in the homework description:

```bash
make
./opengl [scene-description.txt] [xres] [yres] [h]
```

h stands for the time step.

The additional implementations of HW5 are mostly in the parse_scene.cpp file.

To toggle between the smooth mesh and the regular mesh, press ’s' on the keyboard.

Building $F$:

The discrete laplacian

$(\Delta x)_{i}=\frac{1}{2 A} \sum_{j}\left(\cot \alpha_{j}+\cot \beta_{j}\right)\left(x_{j}-x_{i}\right)$ could be rewritten as

$(\Delta x)_{i}=\frac{1}{2 A}( \sum_{j}\left(\cot \alpha_{j}+\cot \beta_{j}\right)x_{j} - x_i\sum_j(\cot\alpha_j+\cot\beta_j))$

So on the diagonal of $\Delta$, we just have $-\sum_j(\cot \alpha_j + \cot \beta_j)$ and in other locations of $\Delta$ , the $j-th$ column will have the value $\cot(\alpha_j) + \cot(\beta_j)$. To find the $F$ matrix, we just compute $F = I - h\Delta$ where $I$ stands for the identity.
