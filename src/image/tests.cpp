#include <iostream>
#include "CameraHandler.h"


int main(int argc, char ** argv) {

    std::string OUTPUT_FILENAME = std::string("/home/anton/Desktop/files/capture");


    std::cout << "1. Opening Capture device" << std::endl;
    std::cout << "2. Capturing is avalable" << std::endl;

    CameraHandler * handler = new CameraHandler();

    handler->setWidth(640);
    handler->setHeight(480);

    handler->setFilename(OUTPUT_FILENAME);
    handler->setFrameDelay(2000);

    handler->configure();

    handler->captureFrame();

    unsigned char * buffer = handler->getBuffer();

    for(int i = 0; i < 640 * 480 * 4; i++) {
        std::cout << (int) buffer[i] << std::endl;


    }



    return 0;
}