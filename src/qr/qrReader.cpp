//
// Created by asia on 18.01.2021.
//

#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

int
main(int argc, char *argv[])
{
    mqd_t mqd;
    struct mq_attr attr{};

    if (argc != 2) {
        fprintf(stderr, "Usage: %s mq-name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    mqd = mq_open(argv[1], O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, NULL);
    if (mqd == (mqd_t) -1)
        errExit("mq_open");

    if (mq_getattr(mqd, &attr) == -1)
        errExit("mq_getattr");

    printf("Maximum # of messages on queue:   %ld\n", attr.mq_maxmsg);
    printf("Maximum message size:             %ld\n", attr.mq_msgsize);

    if (mq_unlink(argv[1]) == -1)
        errExit("mq_unlink");

    exit(EXIT_SUCCESS);
}
/*
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
}*/

