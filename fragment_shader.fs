#version 330 core

// Ouput data
out vec3 color;

// Variables passed from CPU
uniform float minx; // x-coordinate lower left corner of camera
uniform float miny; // y-cooridnate lower left corner of camera
uniform float dx;   // width camera
uniform float dy;   // height camera
uniform float resx; // width of window in pixels
uniform float resy; // height of window in pixels

uniform int drawMandelbrot = 1;

void main() {
    float x, y, cx ,cy;

    // initialize point
    if(drawMandelbrot == 1) {
        // initialize mandelbrot fractal
        cx = minx + gl_FragCoord.x*dx/resx;  // world x-coordinate corresponding to pixel
        cy = miny + gl_FragCoord.y*dy/resy;  // world y-coordinate corresponding to pixel
        x = 0, y = 0;                        // z = x + i*y = 0 initially
    } else {
        // initialize julia fractal
        cx = minx + dx/2;  // world x-coordinate corresponding to pixel
        cy = miny + dy/2;  // world y-coordinate corresponding to pixel
        x = (-1.0 + gl_FragCoord.x*2.0/resx)*resx/resy;
        y = -1.0 + gl_FragCoord.y*2.0/resy; 
    }

    // mandelbrot iteration
    int maxiter = 300;
    int iter = 0;
    while (x*x + y*y < 4.0 && iter < maxiter) {      // |z| > 2 or too many iterations
        float tmp = x*x - y*y + cx;            // z <- z^2 + c
        y = 2*x*y + cy;
        x = tmp;
        iter++;
    }

    if (iter == maxiter) {
            color = vec3(0,0,0);
    } else {
        color = vec3(sin(1.0*iter),cos(1.0*iter),tan(1.0*iter));
    }

    // actual coloring

    if (iter == maxiter) {
        color = vec3(0,0,0);
    } else {
        color = vec3(sin(1.0*iter),cos(1.0*iter),tan(1.0*iter));
    }

}