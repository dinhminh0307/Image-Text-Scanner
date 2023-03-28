
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "tesseract/baseapi.h"
#include "leptonica/allheaders.h"
#include "opencv2/opencv.hpp"

using std::string;

// Increase image dpi
void processImage(const char* imageLink) {
    cv::Mat img = cv::imread(imageLink);
    int new_dpi = 150;

    
    int new_width = img.cols * new_dpi / 72;
    int new_height = img.rows * new_dpi / 72;

 
    cv::Mat new_img(new_height, new_width, img.type());

    
    cv::resize(img, new_img, cv::Size(new_width, new_height));


    cv::imwrite(imageLink, new_img);
    system("cls");
}
// Recognize text in image
void recognizeImage(const char* imageLink) {
    char* outText;
    auto* api = new tesseract::TessBaseAPI();
    processImage(imageLink);
    

    if (api->Init(NULL, "eng")) {
        fprintf(stderr, "Could not initialize tesseract");
        exit(1);
    }
    Pix* image;
    try {
        image = pixRead(imageLink);
    }
    catch(std::exception const& e) {
        printf("File not found");
        return;
    }
    api->SetImage(image);

    outText = api->GetUTF8Text();
    printf(outText);

    api->End();
    delete api;
    delete[] outText;
    pixDestroy(&image);
};
int main(int argc, char** argv)
{
    recognizeImage("C:\\clubPro\\test3.png");
    std::cout << "Success";



    return 0;
}
