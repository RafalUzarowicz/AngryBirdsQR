//
// Created by asia on 18.01.2021.
//

#include <opencv2/calib3d.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../memory/IGame.h"
#include "../memory/IVideo.h"
#include "../memory/QueueGame.h"
#include "../memory/QueueVideo.h"
#include "../memory/SharedGame.h"
#include "../memory/SharedVideo.h"

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
                Point points[4];
                int max_y=0, min_y=img.size().height;
                for(int i=0; i<4;i++){
                    points[i] = Point(bbox.at<float>(2*i), bbox.at<float>(2*i +1));
                    max_y = points[i].y>max_y?points[i].y:max_y;
                    min_y = points[i].y<min_y?points[i].y:min_y;
                }
                int lower_bound = (max_y - min_y)/2;
                int upper_bound = img.size().height- (max_y -min_y)/2;
                int middle = (max_y + min_y)/2;
                double percent = (middle-lower_bound)/(double)(upper_bound-lower_bound);
            };
        }else{
            cout<<"Not detected"<<endl;
        };
    }
    return 0;
}

