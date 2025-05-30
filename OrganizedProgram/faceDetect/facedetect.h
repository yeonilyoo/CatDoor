#ifndef facedetect_H
#define facedetect_H

#include "opencv2/objdetect.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"

#include "opencv2/videoio/videoio_c.h"
#include "opencv2/highgui/highgui_c.h"

#include </usr/local/include/opencv/highgui.h>
#include </usr/local/include/opencv/cv.h>
#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>
using namespace cv;
using namespace std;

class facedetect {

public:
//	const string cascadeName = "../data/haarcascade_frontalcatface_extended.xml";

	facedetect();
	int catDetect();
	bool detectAndDraw( Mat& img, CascadeClassifier& cascade,
	                    CascadeClassifier& nestedCascade,
	                    double scale, bool tryflip );
    // bool tryflip = false;
private:

};
#endif
