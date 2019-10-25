//X11 Screenshot Utility
//Elliott Nash - v0idx.com
//Compile with g++ -o main main.cpp -O2 -L/usr/X11R6/lib -m -lpthread -lX11
//Place screenExec.sh in /usr/local/bin for easy exec.


#include <X11/Xlib.h>
#include <iostream>
#include <X11/Xutil.h>
#include <X11/X.h>
#include<X11/cursorfont.h>
#include "CImg.h"
#include <vector>
using namespace cimg_library;

//Function that returns the starting coordinates and dimensions of the user selected area
std::vector<int> getArea(Display* display,Window root) {
    Cursor currCursor = XCreateCursor(display, XC_lr_angle)
    XGCValues gcValues;
    int areaX = 0,areaY = 0,areaWidth = 0,areaHeight = 0;
    int rectX = 0,rectY = 0, rectWidth = 0, rectHeight = 0;
    int pressed = 0, done = 0;
    XEvent event;

    gcValues.foreground = XWhitePixel(display,0);
    gcValues.function = GXxor;
    gcValues.background = XBlackPixel(display,0);
    gcValues.plane_mask = gcValues.background ^ gcValues.foreground;
    gcValues.subwindow_mode = IncludeInferiors;

    GC graphContext;
    graphContext = XCreateGC(display,root, GCFunction | GCForeground | GCBackground | GCSubwindowMode, &gcValues);


    if(XGrabPointer(display,root,False,ButtonMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync,root,cursor,CurrentTime) != GrabSuccess) {
        std::cerr << "COULD NOT GRAB CURSOR" << std::endl;
    }
    if(XGrabKeyboard(display,root,False,GrabModeAsync,GrabModeAsync,CurrentTime) != GrabSuccess) {
        std::cerr << "COULD NOT GRAB KEYBOARD" << std::endl;
    }

    while(!done) {
        while(!done && XPending(display)) {
            XNextEvent(display, &event);
            switch (event.type) {
                case MotionNotify:
                    if(pressed) {
                        if(rectWidth) {
                            XDrawRectangle(display,root,graphContext,rectX,rectY,rectWidth,rectHeight);
                        }
                        rectX = areaX;
                        rectY = areaY;
                        rectWidth = event.xbutton.x - rectX;
                        rectHeight = event.xbutton.y - rectY;

                        if(rectWidth < 0) {
                            rectX += rectWidth;
                            rectWidth = 0 - rectWidth;
                        }
                        if(rectHeight < 0) {
                            rectY += rectHeight;
                            rectHeight = 0 - rectHeight;
                        }
                        XDrawRectangle(display,root,graphContext,rectX,rectY,rectWidth,rectHeight);
                        XFlush(display);
                    }
                    break;
                case ButtonPress:
                    pressed = 1;
                    areaX = event.xbutton.x;
                    areaY = event.xbutton.y;
                    break;
                case ButtonRelease:
                    done = 1;
                    break;
            }
        }
    }
    if(rectWidth) {
        XDrawRectangle(display, root, graphContext, rectX, rectY, rectWidth, rectHeight);
        XFlush(display);
    }

    areaWidth = event.xbutton.x - areaX;
    areaHeight = event.xbutton.y - areaY;
    if(areaWidth < 0) {
        areaX += areaWidth;
        areaWidth = 0 - areaWidth;
    }
    if(areaHeight < 0) {
        areaY += areaHeight;
        areaHeight = 0 - areaHeight;
    }
    std::vector<int> returnVector;
    //Push values to the return vector.
    returnVector.push_back(areaX);
    returnVector.push_back(areaY);
    returnVector.push_back(areaWidth);
    returnVector.push_back(areaHeight);

    return returnVector;
}


int main(int argc, char* argv[]) {
    Cursor cursor;
    Display* display = XOpenDisplay(nullptr);
    cursor = XCreateFontCursor(display,XC_crosshair);
    Window root = DefaultRootWindow(display);
    XWindowAttributes windowAttributes;
    XGetWindowAttributes(display, root, &windowAttributes);
    std::vector<int> dimensions;

    dimensions = getArea(display,cursor,root);

    if(argc < 2) {
        std::cerr << "USEAGE " << argv[0] << " FILEPATH" << std::endl;
        return 1;
    }

    //Get pixel data for the dimensions specified
    XImage *ximg = XGetImage(display,root,dimensions[0],dimensions[1],(uint)dimensions[2],(uint)dimensions[3],AllPlanes,ZPixmap);

    unsigned char *pixels = new unsigned char[dimensions[2] * dimensions[3] * 3];

    //Create rgb masks for pixel data
    unsigned long red_mask = ximg->red_mask;
    unsigned long green_mask = ximg->green_mask;
    unsigned long blue_mask = ximg->blue_mask;

    //Create an image with CImg
    CImg<unsigned char> pic(pixels,dimensions[2],dimensions[3],1,3);

    for(int x = 0; x < dimensions[2]; x++) {
        for(int y = 0; y < dimensions[3]; y++) {
            //Set rgb values for each pixel and add to the CImg variable
            unsigned long pixel = XGetPixel(ximg,x,y);
            unsigned char blue = pixel & blue_mask;
            unsigned char green = (pixel & green_mask) >> 8;
            unsigned char red = (pixel & red_mask) >> 16;

            
            pic(x,y,0) = red;
            pic(x,y,1) = green;
            pic(x,y,2) = blue;
        }
    }
    //Save capture as a png
    pic.save_png(argv[1]);
    //Need to add clipboard support, currently added via shell script

    return 0;
}