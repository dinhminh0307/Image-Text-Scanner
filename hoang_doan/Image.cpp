#include "Image.h"

Pix* mat8ToPix(cv::Mat* mat8)
{
    Pix* pixd = pixCreate(mat8->size().width, mat8->size().height, 8);
    for (int y = 0; y < mat8->rows; y++) {
        for (int x = 0; x < mat8->cols; x++) {
            pixSetPixel(pixd, x, y, (l_uint32)mat8->at<uchar>(y, x));
        }
    }
    return pixd;
}

cv::Mat pix8ToMat(Pix* pix8)
{
    cv::Mat mat(cv::Size(pix8->w, pix8->h), CV_8UC1);
    uint32_t* line = pix8->data;
    for (uint32_t y = 0; y < pix8->h; ++y) {
        for (uint32_t x = 0; x < pix8->w; ++x) {
            mat.at<uchar>(y, x) = GET_DATA_BYTE(line, x);
        }
        line += pix8->wpl;
    }
    return mat;
}

int imageRec(Mat* mat) {
    char* outText;

    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    // Open input image with leptonica library
    Pix* image = mat8ToPix(mat);
    api->SetImage(image);
    // Get OCR result
    outText = api->GetUTF8Text();
    printf("OCR output:\n%s", outText);

    // Destroy used object and release memory
    api->End();
    delete api;
    delete[] outText;
    pixDestroy(&image);

    return 0;
}

int getComponent(Mat* mat) {
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    // Open input image with leptonica library
    Pix* image = mat8ToPix(mat);
    api->SetImage(image);
    Boxa* boxes = api->GetComponentImages(tesseract::RIL_TEXTLINE, false, NULL, NULL);
    printf("Found %d textline image components.\n", boxes->n);
    for (int i = 0; i < boxes->n; i++) {
        BOX* box = boxaGetBox(boxes, i, L_CLONE);
        api->SetRectangle(box->x, box->y, box->w, box->h);
        char* ocrResult = api->GetUTF8Text();
        int conf = api->MeanTextConf();
        fprintf(stdout, "Box[%d]: x=%d, y=%d, w=%d, h=%d, confidence: %d, text: %s",
            i, box->x, box->y, box->w, box->h, conf, ocrResult);

        // draw box on image
        Point p1(box->x, box->y);
        Point p2(box->x + box->w, box->y + box->h);
        rectangle(*mat, p1, p2, Scalar(255, 0, 0), 2, LINE_8);

        boxDestroy(&box);
    }
    // Destroy used object and release memory
    imshow("Output", *mat);
    waitKey(0);
    api->End();
    delete api;
    pixDestroy(&image);

    return 0;
}

int resultIter(Mat* mat) {
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
    Pix* image = mat8ToPix(mat);
    api->SetImage(image);
    api->Recognize(0);
    tesseract::ResultIterator* ri = api->GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
    if (ri != 0) {
        do {
            const char* word = ri->GetUTF8Text(level);
            float conf = ri->Confidence(level);
            int x1, y1, x2, y2;
            ri->BoundingBox(level, &x1, &y1, &x2, &y2);
            printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
                word, conf, x1, y1, x2, y2);
            //delete[] word;
        } while (ri->Next(level));
    }
    // Destroy used object and release memory
    api->End();
    delete api;
    pixDestroy(&image);
    return 0;
}

int osdExample(Mat* mat) {
    PIX* image = mat8ToPix(mat);
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    api->Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "osd");
    // Get OSD - old example code
    tesseract::Orientation orientation;
    tesseract::WritingDirection direction;
    tesseract::TextlineOrder order;
    float deskew_angle;
    api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
    api->SetImage(image);
    api->Recognize(0);
    tesseract::PageIterator* it = api->AnalyseLayout();
    it->Orientation(&orientation, &direction, &order, &deskew_angle);
    printf("Orientation: %d;\nWritingDirection: %d\nTextlineOrder: %d\n" \
        "Deskew angle: %.4f\n",
        orientation, direction, order, deskew_angle);
    //Get OSD - new code
    int orient_deg;
    float orient_conf;
    const char* script_name;
    float script_conf;
    api->DetectOrientationScript(&orient_deg, &orient_conf, &script_name, &script_conf);
    printf("************\n Orientation in degrees: %d\n Orientation confidence: %.2f\n"
        " Script: %s\n Script confidence: %.2f\n",
        orient_deg, orient_conf,
        script_name, script_conf);
    // Destroy used object and release memory
    api->End();
    pixDestroy(&image);
    return 0;
}

int hocrExample(Mat* mat) {
    char* outText;

    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    // Open input image with leptonica library
    Pix* image = mat8ToPix(mat);
    api->SetImage(image);
    api->SetVariable("lstm_choice_mode", "2");
    // Get HOCR result
    outText = api->GetHOCRText(0);
    printf("HOCR alternative symbol choices  per character :\n%s", outText);

    // Destroy used object and release memory
    api->End();
    pixDestroy(&image);

    return 0;
}