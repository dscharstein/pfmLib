/*  Name:
 *      ImageIOpfm.h
 *
 *  Description:
 *      Used to read/write pfm images to and from 
 *      opencv Mat image objects
 *      
 *      Works with PF color pfm files and Pf grayscale
 *      pfm files
 */


#ifndef __ImageIOpfm_H_INCLUDED__ 
#define __ImageIOpfm_H_INCLUDED__ 

#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <cmath>

using namespace cv;
using namespace std;

int ReadFilePFM(Mat &im, string path);
int WriteFilePFM(const Mat &im, string path, float scalef);

#endif