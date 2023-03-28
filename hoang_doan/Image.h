#include <string>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int imageRec(Mat* mat);

int getComponent(Mat* mat);

int resultIter(Mat* mat);

int osdExample(Mat* mat);

int hocrExample(Mat* mat);