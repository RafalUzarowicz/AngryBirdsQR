//
// Created by asia on 18.01.2021.
//

#include <opencv2/calib3d.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    Mat img;
    if(argc>1){
        img = imread(argv[1]);
        QRCodeDetector qrDecoder;

        Mat bbox;

        if(qrDecoder.detect(img, bbox)){
            if(bbox.cols ==4){
                Point topLeft(bbox.at<float>(0), bbox.at<float>(1));
                Point bottomRight(bbox.at<float>(4), bbox.at<float>(5));
                Point middle = (topLeft + bottomRight)/2;
                float width = img.size().width;
                float height = img.size().width;
                cout<<middle.x/width<<endl;
                cout<<middle.y/height<<endl;
            };
        }else{
            cout<<"Not detected"<<endl;
        };
    }
    return 0;

}
