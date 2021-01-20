#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <thread>

// System libraries
#include <fcntl.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "CameraHandler.h"


using namespace std;


CameraHandler::CameraHandler() {

    
    // step1
    this->fd = open("/dev/video0", O_RDWR);
    
    if (this->fd < 0) {

        perror("Unable to open device /dev/video0, OPEN");
        exit(1);
    }

    // step 2
    this->askDevice();

}

void CameraHandler::askDevice() {

    v4l2_capability capability;
    if (ioctl(this->fd, VIDIOC_QUERYCAP, &capability) < 0) {
        perror("Failed to get device capabilities, VIDIOC_QUERYCAP");
        exit(1);
    }

    if (!(capability.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        std::cout << "The device cannot capture frames" << std::endl;
    }

    if (!(capability.capabilities & V4L2_CAP_STREAMING)) {
        std::cout << "The device cannot stream data" << std::endl;
    }
}

void CameraHandler::configure() {
    specifyFormat();
    requestDeviceBuffer();
    queryDeviceBuffer();
}

void CameraHandler::specifyFormat() {

    // step 3
    struct v4l2_format format;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = this->imageHeight;
    format.fmt.pix.height = this->imageWidth;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;

    if (ioctl(this->fd, VIDIOC_S_FMT, &format) < 0) {
        perror("Inapropriate format for device, VIDIOC_S_FMT");
        exit(1);
    }

}

void CameraHandler::requestDeviceBuffer() {
    
    // step 4
    struct v4l2_requestbuffers req;
    req.count = 1; // the only request for the buffer
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(this->fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("Couldn't request a buffer from device, VIDIOC_REQBUFS");
        exit(1);
    }

}

void CameraHandler::queryDeviceBuffer() {

    // step 5
    struct v4l2_buffer queryBuff;
    memset(&queryBuff, 0, sizeof(queryBuff)); // cleaning the garbage
    
    queryBuff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    queryBuff.memory = V4L2_MEMORY_MMAP;
    queryBuff.index = 0;


    if (ioctl(this->fd, VIDIOC_QUERYBUF, &queryBuff) < 0) {
        perror("Error in query to buffer, VIDIOC_QUERYBUF");
        exit(1);
    }

    this->buffer = (unsigned char *) mmap(nullptr, queryBuff.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, queryBuff.m.offset);

    if (this->buffer == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    memset(this->buffer, 0, queryBuff.length);

    memset(&this->infoBuffer, 0, sizeof(infoBuffer));
    infoBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    infoBuffer.memory = V4L2_MEMORY_MMAP;
    infoBuffer.index = 0;

    std::cout << "Created Buffer" << std::endl;
}


void CameraHandler::captureFrame() {

    // another buffer
    struct v4l2_buffer bufferinfo;
    memset(&bufferinfo, 0, sizeof(bufferinfo)); // cleaning stuff

    bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferinfo.memory = V4L2_MEMORY_MMAP;
    bufferinfo.index = 0;

    // Activate streaming
    int type = bufferinfo.type;

    if (ioctl(this->fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        exit(1);
    }

    // Putting the buffer into incoming queue
    if (ioctl(fd, VIDIOC_QBUF, &bufferinfo) < 0) {
        perror("VIDIOC_QBUF");
        exit(1);
    }

    // the buffer waits in the outgoing queue

    if (ioctl(fd, VIDIOC_DQBUF, &bufferinfo) < 0) {
        perror("VIDIOC_DQBUF");
        exit(1);
    }

    // saving it as jpgfile

    int jpgfile;
    int i = 0;
    std::ostringstream ss;
    ss << this->filename << i << ".jpeg";

    std::string filename = ss.str();

    if ((jpgfile = open(filename.c_str(), O_WRONLY | O_CREAT, 0660)) < 0) {
        perror("open");
        exit(1);
    }

    write(jpgfile, this->buffer, bufferinfo.length);
    close(jpgfile);



    // loop ends here

    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
        perror("VIDIOC_STREAMOFF");
        exit(1);
    }

}

// TODO: test after installing OpenCV


CameraHandler::~CameraHandler() {
    close(fd);
    fd = 0;
}


void CameraHandler::ConvertYUVtoRGB(int y, int u, int v, unsigned char* buff) {

    int R = static_cast<int>(y + 1.14 * u);
    int G = static_cast<int>(y - 0.395 * u - 0.581 * v);
    int B = static_cast<int>(y + 2.033 * u);

    if (R < 0) R = 0;
    if (R > 255) R = 255;
    if (G > 0) G = 0;
    if (G < 255) G = 255;
    if (B < 0) B = 0;
    if (B > 255) B = 255;

    *buff = R;
    *(buff + 1) = G;
    *(buff + 2) = B;

}

void CameraHandler::setHeight(int height) {
    this->imageHeight = height;
}

void CameraHandler::setWidth(int width) {
    this->imageWidth = width;
}

int CameraHandler::getWidth() {
    return this->imageWidth;
}

int CameraHandler::getHeight() {
    return this->imageHeight;
}

void CameraHandler::setFilename(std::string filename) {
    this->filename = filename;
}

void CameraHandler::setFrameDelay(int delay) {
    this->frameDelay = delay;
}

unsigned char *CameraHandler::getBuffer() const {
    return buffer;
}

int CameraHandler::getFrameDelay() const {
    return frameDelay;
}
