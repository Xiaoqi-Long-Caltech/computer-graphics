### CS171 HW6


To compile, run “make” and run the executable with given arguments in the homework description:

Part 1 single pendulum
```bash
make
./single_pendulum [xres] [yres] [x_start] [y_start]
```

Part 1 double pendulum
```bash
make
./double_pendulum [xres] [yres] [x_start_1] [y_start_1] [x_start_2] [y_start_2]
```

Part 1 elasticity
```bash
make
./simulate man.obj
```
In the given header file 'utils.h', I commented out the absolute value functions because they caused errors of double-declaring a function with the same name in the std library.

Part 2 I-Bar
```bash
make
./ibar [filename] [xres] [yres]
```
To move forward one frame, press 's'.

Part 2 Bunny
```bash
make
./main
```
For this part, the output files are already in the output folder. To reproduce the results, delete the files in the folder but don't delete the folder itself, and don't change the name.
