#include <iostream>
#include "CameraHandler.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char ** argv) {

    const char * OUTPUT_FILENAME = "/home/anton/Desktop/myimage.jpeg";


    std::cout << "1. Opening Capture device" << std::endl;
    std::cout << "2. Capturing is avalable" << std::endl;

    CameraHandler * handler = new CameraHandler();

    handler->setWidth(640);
    handler->setHeight(480);
    handler->setFilename(OUTPUT_FILENAME);

    std::cout << "3. specifying the format of the image" << std::endl;

    handler->specifyFormat();

    std::cout << "Width: " << handler->getWidth() << std::endl;
    std::cout << "Height: " << handler->getHeight() << std::endl;

    std::cout << "4. requesting a buffer from the driver" << std::endl;

    handler->requestDeviceBuffer();

    
    std::cout << "5. queried the buffer" << std::endl;
    handler->queryDeviceBuffer();


    handler->captureFrame();

    std::cout << "6. captured the image" << std::endl;

    std::cout << "Yay!" << std::endl;

    return 0;
}