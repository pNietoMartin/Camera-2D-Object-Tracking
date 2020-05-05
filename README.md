# Camera-2D-Object-Tracking

<img src="images/keypoints.png" width="820" height="248" />

The goal of this project is to build the feature tracking part of a time-to-collision system and test various detector / descriptor combinations to see which ones perform the best. This project consists of four parts:

* First, it is instanciated a `DataFrame` variable to setting the images into a ring buffer to optimize memory load. 
* Then,it has been implemented several keypoint detectors at `matching2D_Student.cpp` such as HARRIS, FAST, BRISK and SIFT. It is an interesting exercise for the user to compare them with regard to number of keypoints identified and computational speed. 
* To continue, it has been implemented a descriptor extraction and matching using brute force and also the FLANN approach. 

## Dependencies for Running Locally
* cmake >= 2.8
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* OpenCV >= 4.1
  * This must be compiled from source using the `-D OPENCV_ENABLE_NONFREE=ON` cmake flag for testing the SIFT and SURF detectors.
  * The OpenCV 4.1.0 source code can be found [here](https://github.com/opencv/opencv/tree/4.1.0)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./2D_feature_tracking`.