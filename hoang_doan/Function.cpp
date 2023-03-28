#include "Function.h"

int imageRec(const char* path) {
    //string outText, imPath = argv[1];
    //Mat im = cv::imread(imPath, IMREAD_COLOR);

    //tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
    //if (ocr->Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "eng")) {
    //	fprintf(stderr, "Could not initialize tesseract.\n");
    //	exit(1);
    //}

    //ocr->SetPageSegMode(tesseract::PSM_AUTO);
    //ocr->SetImage(im.data, im.cols, im.rows, 3, im.step);
    //outText = string(ocr->GetUTF8Text());
    //cout << outText;
    //ocr->End();

    char* outText;

    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    // Open input image with leptonica library
    Pix* image = pixRead(path);
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

int getComponent(const char* path) {
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    // Read image by opencv
    Mat img = imread(path, IMREAD_COLOR);

    // Open input image with leptonica library
    Pix* image = pixRead(path);
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
        rectangle(img, p1, p2, Scalar(255, 0, 0), 2, LINE_8);

        boxDestroy(&box);
    }
    // Destroy used object and release memory
    imshow("Output", img);
    waitKey(0);
    api->End();
    delete api;
    pixDestroy(&image);

    return 0;
}

int resultIter(const char* path) {
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
    Pix* image = pixRead(path);
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

int osdExample(const char* path) {
    PIX* image = pixRead(path);
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

int hocrExample(const char* path) {
    char* outText;

    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    // Open input image with leptonica library
    Pix* image = pixRead(path);
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