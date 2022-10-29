#include "gtest/gtest.h"
#include "SDL2/SDL.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// https://stackoverflow.com/questions/10167534/how-to-find-out-what-type-of-a-mat-object-is-with-mattype-in-opencv
static string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return "CV_" + r;
}

static string fourcc2str (int fourcc) {
    return format("%c%c%c%c", fourcc & 255, (fourcc >> 8) & 255, (fourcc >> 16) & 255, (fourcc >> 24) & 255);
}

// ./bin/linux/x86_64/generic/haicam-test --gtest_filter=haicam_opencv_test.camera_test
TEST(haicam_opencv_test, camera_test)
{
    Mat frame;
    cout << "Opening camera..." << endl;
    VideoCapture capture(1); // open the first camera
    if (!capture.isOpened())
    {
        cerr << "ERROR: Can't initialize camera capture" << endl;
        return;
    }

    // v4l2-ctl --list-formats-ext --device /dev/video1
    capture.set(CAP_PROP_CONVERT_RGB, false); //Request raw camera data
    //capture.set(CAP_PROP_FORMAT, -1);//Set value -1 to fetch undecoded RAW video streams (as Mat 8UC1).
    capture.set(CAP_PROP_FOURCC, VideoWriter::fourcc('Y','U','Y','V'));
    //capture.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M','J','P','G'));

    cout << "Frame width: " << capture.get(CAP_PROP_FRAME_WIDTH) << endl;
    cout << "     height: " << capture.get(CAP_PROP_FRAME_HEIGHT) << endl;
    cout << "Capturing FPS: " << capture.get(CAP_PROP_FPS) << endl;
    //cout << "Capturing FORMAT: " << type2str(capture.get(CAP_PROP_FORMAT)) << endl;
    cout << "Capturing FOURCC: " << fourcc2str(capture.get(CAP_PROP_FOURCC)) << endl;

    for(;;) {
        capture >> frame; // read the next frame from camera
        if (frame.empty())
        {
            cerr << "ERROR: Can't grab camera frame." << endl;
        } else {
            /*uint8_t *new_data = new uint8_t[frame.rows*frame.cols*2]; // for YUYV
            if(frame.isContinuous()) {
                memcpy(new_data, frame.ptr(0), frame.rows*frame.cols*2); 
            } else {
                uint8_t *p = new_data;
                for(int i  = 0; i < frame.rows; i++){
                    memcpy(p, frame.ptr(i), frame.cols*2); 
                    p += frame.cols * 2;
                }
            }*/

            //cout << "Frame captured: " << frame.cols << " " << frame.rows << " " << type2str(frame.type()) << "\n";
            if(fourcc2str(capture.get(CAP_PROP_FOURCC)) == "YUYV")cvtColor(frame, frame, COLOR_YUV2BGR_YUYV);
            if(fourcc2str(capture.get(CAP_PROP_FOURCC)) == "MJPG")frame = imdecode(frame, IMREAD_COLOR);
            imshow("Live", frame);
        }

        if (waitKey(10) >= 0)
            break;
    }
    capture.release();

}