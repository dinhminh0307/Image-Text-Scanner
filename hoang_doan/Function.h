#include <string>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int imageRec(const char* path);

int getComponent(const char* path);

int resultIter(const char* path);

int osdExample(const char* path);

int hocrExample(const char* path);