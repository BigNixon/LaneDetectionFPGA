# LaneDetectionFPGA

------------
> Original C-code for edge detection by Marco Winzker

From his edge detection algorithm, this is my attempt to implement a line detector based on the hough transform.

------------


## How to run C code
Enter the C-files folder and run the C code with the following command:
```bash
 .\hough_fixed.exe ..\Test_Images\street_B
```

------------

## Hough Transform

The hough transform maps a point in the X-Y coordinates to the Rho-Theta parameter space, where each point transforms into a sinusoid. Each intersection of n sinusoids tell us that this N edges belongs to the same lane.

** ALGORITHM **
- Limit the region of interest of the edged image.
- Map each pixel wich is an edge to the rho-theta parameter space.
- Accumulate all the intersections of the sinusoid.
- Detect the larger values of the acumulator (this are our lines).
