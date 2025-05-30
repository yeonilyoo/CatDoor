#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include "rpiServo.h"
#include "temp.h"
#include <bcm2835.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <sstream>
//Pthread
#include <pthread.h>
#include <cstdlib>
//Signal
#include <signal.h>     /* signal, raise, sig_atomic_t */
#include <unistd.h> /* alarm */


using namespace std;
using namespace cv;

bool initializeCatDetect();

bool detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip );

string cascadeName = "data/haarcascade_frontalcatface_extended.xml";

Mat frame, frameCopy, image;
string inputName = "cat.jpg";

CascadeClassifier cascade, nestedCascade;

#define SENSOR RPI_GPIO_P1_11
//#define LED RPI_GPIO_P1_11
#define ACTIVE_VALUE 1

rpiServo servo;
temp temperature;
int count1 = 0;
const unsigned int time1 = 1;

void *status;
pthread_t pirt;
pthread_t tempt;

//PTHREAD
void *pirthread(void *)
{
   uint8_t state = ACTIVE_VALUE; //Init state to HIGH
   state = bcm2835_gpio_lev(SENSOR); //HIGH or LOW?
        //printf("State = %d\n", state);
        if(state != ACTIVE_VALUE)
        {
            //Sensor not active
        }
        else
        {
            printf("Movement at %d\n",(int)time(NULL));
            system("raspistill -t 250 -vf -w 320 -h 240 -o cat.jpg -q 25");
            bool isCat = detectAndDraw( image, cascade, nestedCascade, 1.0, false );
            if(isCat) {
                servo.setAngle(0);
                printf("is a cat\n");
            } else {
                servo.setAngle(180);
                printf("not a cat\n");
            }
        }
   pthread_exit(NULL);
}

void *tempthread(void *) {
    printf("Temp: %f\n",(float)temperature.getTemp());
    pthread_exit(NULL);
}

//time interrupt
void sig_alarmhandler(int sig) {
    //printf("alarmed\n");
    count1++;
    if(count1 >= 5) {
        count1 = 0;
        pthread_join(tempt, &status);
        pthread_create(&tempt, NULL, tempthread, NULL);
    }
    pthread_join(pirt, &status);
    pthread_create(&pirt, NULL, pirthread, NULL);
    alarm(time1);
}

int main( int argc, const char** argv )
{
    bool initialize = initializeCatDetect();
    if(!initialize) cout << "Couldn't initialize" <<endl;
    
    if (!bcm2835_init()) {
        printf("Please run this with sudo\n");
        return EXIT_FAILURE;
    }
    
    bcm2835_gpio_fsel(SENSOR, BCM2835_GPIO_FSEL_INPT);//SENSOR as input
    //bcm2835_gpio_fsel(LED, BCM2835_GPIO_FSEL_OUTP);      //LED as output
    bcm2835_gpio_set_pud(SENSOR, BCM2835_GPIO_PUD_UP);

    //Pthread
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&tempt, NULL, tempthread, NULL);
    pthread_create(&pirt, NULL, pirthread, NULL);

    pthread_attr_destroy(&attr);

    signal(SIGALRM, &sig_alarmhandler);
    alarm(time1);
    while (1) {
    }
    bcm2835_close();
        
    
    return EXIT_SUCCESS;

}





bool initializeCatDetect(){
    
   // CvCapture* capture = 0;
    
    if( !cascade.load( cascadeName ) )
    {
        cerr << "ERROR: Could not load classifier cascade" << endl;
        return false;
    }
    
    return true;
}




bool detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip )
{
    image = imread( inputName, 1 );
    if(image.empty()) cout << "Couldn't read cattest.jpg" <<endl;
    
    cout << "In image read" << endl;
    
    int i = 0;
    double t = 0;
    bool isFace = false;
    vector<Rect> faces, faces2;
    const static Scalar colors[] =  { CV_RGB(0,0,255),
        CV_RGB(0,128,255),
        CV_RGB(0,255,255),
        CV_RGB(0,255,0),
        CV_RGB(255,128,0),
        CV_RGB(255,255,0),
        CV_RGB(255,0,0),
        CV_RGB(255,0,255)} ;
    Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );

    cvtColor( img, gray, CV_BGR2GRAY );
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    t = (double)cvGetTickCount();
    cascade.detectMultiScale( smallImg, faces,
        1.1, 2, 0
        //|CV_HAAR_FIND_BIGGEST_OBJECT
        //|CV_HAAR_DO_ROUGH_SEARCH
        |CV_HAAR_SCALE_IMAGE
        ,
        Size(30, 30) );
    t = (double)cvGetTickCount() - t;
    printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );
    for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++ )
    {
        printf("test %d\n", i);
        isFace = true;
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
        Scalar color = colors[i%8];
        int radius;

        double aspect_ratio = (double)r->width/r->height;
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
        {
            center.x = cvRound((r->x + r->width*0.5)*scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
        else
            rectangle( img, cvPoint(cvRound(r->x*scale), cvRound(r->y*scale)),
                       cvPoint(cvRound((r->x + r->width-1)*scale), cvRound((r->y + r->height-1)*scale)),
                       color, 3, 8, 0);
        if( nestedCascade.empty() )
            continue;
    }
    imwrite("../catoutput.jpg", img);
    return isFace;
}
#undef SENSOR
//#undef LED
#undef ACTIVE_VALUE
