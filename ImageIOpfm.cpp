/*  Name:
 *      ImageIOpfm.cpp
 *
 *  Description:
 *      Used to read/write pfm images to and from 
 *      opencv Mat image objects
 *      
 *      Works with PF color pfm files and Pf grayscale
 *      pfm files
 */

#include "ImageIOpfm.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <cmath>

using namespace cv;
using namespace std;

void skip_space(fstream& fileStream)
{
    // skip white space in the headers or pnm files

    char c;
    do {
        c = fileStream.get();
    } while (c == '\n' || c == ' ' || c == '\t' || c == '\r');
    fileStream.unget();
}

// check whether machine is little endian
int littleendian()
{
    int intval = 1;
    uchar *uval = (uchar *)&intval;
    return uval[0] == 1;
}

void swapBytes(float* fptr) { // if endianness doesn't agree, swap bytes
	uchar* ptr = (uchar *) fptr;
	uchar tmp = 0;
	tmp = ptr[0]; ptr[0] = ptr[3]; ptr[3] = tmp;
	tmp = ptr[1]; ptr[1] = ptr[2]; ptr[2] = tmp;
}

/*
 *  Reads a .pfm file image file into an 
 *  opencv Mat structure with type
 *  CV_32F, handles either 1 band or 3 band 
 *  images
 *
 *  Params:
 *      im:     type: Mat       description: image destination
 *      path:   type: string    description: file path to pfm file
 */
int ReadFilePFM(Mat &im, string path){

    // create fstream object to read in pfm file 
    // open the file in binary
    fstream file(path.c_str(), ios::in | ios::binary);
    
    // init variables 
    string bands;           // what type is the image   "Pf" = grayscale    (1-band)
                            //                          "PF" = color        (3-band)
    int width, height;      // width and height of the image
    float scalef, fvalue;   // scale factor and temp value to hold pixel value
    Vec3f vfvalue;          // temp value to hold 3-band pixel value

    // extract header information, skips whitespace 
    file >> bands;
    file >> width;
    file >> height;
    file >> scalef;

    // determine endianness 
    int littleEndianFile = (scalef < 0);
    int littleEndianMachine = littleendian();
    int needSwap = (littleEndianFile != littleEndianMachine);

    cout << setfill('=') << setw(19) << "=" << endl;
    cout << "Reading image to pfm file: " << path << endl;
    cout << "Little Endian?: "  << ((needSwap) ? "false" : "true")   << endl;
    cout << "width: "           << width                             << endl;
    cout << "height: "          << height                            << endl;
    cout << "scale: "           << scalef                            << endl;

    // skip SINGLE newline character after reading third arg
    char c = file.get();
    if (c == '\r')      // <cr> in some files before newline
        c = file.get();
    if (c != '\n') {
        if (c == ' ' || c == '\t' || c == '\r'){
            cout << "newline expected";
            return -1;
        }
        else{
        	cout << "whitespace expected";
            return -1;
        }
    }
    
    if(bands == "Pf"){          // handle 1-band image 
        cout << "Reading grayscale image (1-band)" << endl; 
        cout << "Reading into CV_32FC1 image" << endl;
        im = Mat::zeros(height, width, CV_32FC1);
        for (int i=height-1; i >= 0; --i) {
            for(int j=0; j < width; ++j){
                file.read((char*) &fvalue, sizeof(fvalue));
                if(needSwap){
                	swapBytes(&fvalue);
                }
                im.at<float>(i,j) = (float) fvalue;
            }
        }
    }else if(bands == "PF"){    // handle 3-band image
        cout << "Reading color image (3-band)" << endl;
        cout << "Reading into CV_32FC3 image" << endl; 
        im = Mat::zeros(height, width, CV_32FC3);
        for (int i=height-1; i >= 0; --i) {
            for(int j=0; j < width; ++j){
                file.read((char*) &vfvalue, sizeof(vfvalue));
                if(needSwap){
                	swapBytes(&vfvalue[0]);
                	swapBytes(&vfvalue[1]);
                	swapBytes(&vfvalue[2]);
                }
                im.at<Vec3f>(i,j) = vfvalue;
            }
        }
    }else{
        cout << "unknown bands description";
        return -1;
    }
    cout << setfill('=') << setw(19) << "=" << endl << endl;
    return 0;
}

/*
 *  Writes a .pfm file image file from an 
 *  opencv Mat structure with type
 *  CV_32F, handles either 1 band or 3 band 
 *  images
 *
 *  Params:
 *      im:     type: Mat       description: image destination
 *      path:   type: string    description: file path to pfm file
 *      scalef: type: float     description: scale factor and endianness 
 */
int WriteFilePFM(const Mat &im, string path, float scalef=1/255.0){

    // create fstream object to write out pfm file 
    // open the file in binary
    fstream file(path.c_str(), ios::out | ios::binary);

    
    // init variables 
    int type = im.type();
    string bands;
    int width = im.size().width, height = im.size().height;     // width and height of the image 
    float fvalue;       // scale factor and temp value to hold pixel value
    Vec3f vfvalue;      // temp value to hold 3-band pixel value


    switch(type){       // determine identifier string based on image type
        case CV_32FC1:
            bands = "Pf";   // grayscale
            break;
        case CV_32FC3:
            bands = "PF";   // color
            break;
        default:
            cout << "Unsupported image type, must be CV_32FC1 or CV_32FC3";
            return -1;
    }

    // sign of scalefact indicates endianness, see pfms specs
    if(littleendian())
        scalef = -scalef;

    // insert header information 
    file << bands   << "\n";
    file << width   << "\n";
    file << height  << "\n";
    file << scalef  << "\n";

    cout << setfill('=') << setw(19) << "=" << endl;
    cout << "Writing image to pfm file: " << path << endl;
    cout << "Little Endian?: "  << ((littleendian()) ? "true" : "false")   	<< endl;
    cout << "width: "           << width                             		<< endl;
    cout << "height: "          << height                            		<< endl;
    cout << "scale: "           << scalef                            		<< endl;
    
    if(bands == "Pf"){          // handle 1-band image 
        cout << "Writing grayscale image (1-band)" << endl; 
        cout << "Writing into CV_32FC1 image" << endl;
        for (int i=height-1; i >= 0; --i) {
            for(int j=0; j < width; ++j){
                fvalue = im.at<float>(i,j);
                file.write((char*) &fvalue, sizeof(fvalue));
                
            }
        }
    }else if(bands == "PF"){    // handle 3-band image
        cout << "writing color image (3-band)" << endl;
        cout << "writing into CV_32FC3 image" << endl; 
        for (int i=height-1; i >= 0; --i) {
            for(int j=0; j < width; ++j){
                vfvalue = im.at<Vec3f>(i,j);
                file.write((char*) &vfvalue, sizeof(vfvalue));
            }
        }
    }else{
        cout << "unknown bands description";
        return -1;
    }
    cout << setfill('=') << setw(19) << "=" << endl << endl;
    return 0;
}

/*
int main(int argc, char ** argv){
    Mat I, M;

    ReadFilePFM(I, "disp0GTplaytable.pfm");  
    WriteFilePFM(I, "disp0RW.pfm");
    ReadFilePFM(M, "disp0RW.pfm");

    Mat Iresult;
    //I.convertTo(Iresult, CV_8UC1);
    Iresult = I / 255.0;

    Mat Mresult;
 	//M.convertTo(Mresult, CV_8UC1);
 	Mresult = M / 255.0;

    namedWindow( "pfm_disp_after", CV_WINDOW_AUTOSIZE );
    imshow( "pfm_disp_after", Mresult );

    namedWindow( "pfm_disp_before", CV_WINDOW_AUTOSIZE );
    imshow( "pfm_disp_before", Iresult );
    
    waitKey(0);
    return 0;
}
*/

 
