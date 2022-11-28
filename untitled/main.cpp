#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <process.h>
#include <E:/alclion/untitled/stdafx.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include "opencv2/imgcodecs/legacy/constants_c.h"
#pragma comment(lib,"opencv_core2410d.lib")
#pragma comment(lib,"opencv_highgui2410d.lib")
#pragma comment(lib,"opencv_imgproc2410d.lib")


using namespace cv;
using namespace std;

struct con {
    double x, y;                    //轮廓位置
    int order;                      //轮廓向量contours中的第几个

    bool operator<(con &m) {
        if (y > m.y) return false;
        else if (y == m.y) {
            if (x < m.x) return true;
            else return false;
        } else return true;
    }

} con[15];

struct result {
    double bi;
    int num;

    bool operator<(result &m) {
        if (bi < m.bi)return true;
        else return false;
    }
} result[15];

Mat num[15];
Mat sample;

void deal(Mat &src, int order);

void Threshold(Mat &src, Mat &sample, int m);

int main(int argc, char **argv) {

    VideoCapture cap(1);//括号内数字为1是调用usb摄像头，为0则是调用电脑自带摄像头
    Mat frame, a;
    Mat srcImage;
    Mat dstImage, grayImage, Image;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    Point2f pp[5][4];
    vector<vector<Point>>::iterator It;
    Rect rect[1];
    Point2f vertex[4];
    while (1) {

        cap >> srcImage;//将摄像头抓取的图像放入frame中
        imshow("视频", srcImage);
        srcImage.copyTo(dstImage);
        cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);
        threshold(grayImage, Image, 48, 255, 1);

        //轮廓包含点到点之间的连续轮廓

        findContours(Image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        int i = 0;


        It = contours.begin();

//    for(It = contours.begin();It < contours.end();It++){                        //画出可包围数字的最小矩形

        rect[0] = boundingRect(*It);
        vertex[0] = rect[i].tl();                                                           //矩阵左上角的点
        vertex[1].x = (float) rect[i].tl().x, vertex[1].y = (float) rect[i].br().y;           //矩阵左下方的点
        vertex[2] = rect[i].br();                                                           //矩阵右下角的点
        vertex[3].x = (float) rect[i].br().x, vertex[3].y = (float) rect[i].tl().y;           //矩阵右上方的点
//
        for (int j = 0; j < 4; j++)
            line(dstImage, vertex[j], vertex[(j + 1) % 4], Scalar(0, 0, 255), 1);

        con[i].x = (vertex[0].x + vertex[1].x + vertex[2].x + vertex[3].x) / 4.0;                  //根据中心点判断图像的位置
        con[i].y = (vertex[0].y + vertex[1].y + vertex[2].y + vertex[3].y) / 4.0;
        con[i].order = i;
        i++;

        int k = con[0].order;
        srcImage(rect[k]).copyTo(num[0]);
//        cvtColor(num[0], num[0], COLOR_BGR2GRAY);
//        threshold(num[0], num[0], 48, 255, 1);

//        imshow("测试", num[0]);
//        waitKey(0);

        deal(num[0], 8);


        waitKey(1);

    }
    return 0;
}

void Threshold(Mat &src, Mat &sample, int m) {

    Mat roi_img (src.rows, src.cols, CV_8UC3, Scalar::all(0));//创建与image像素大小相等的三通道图像
    src.copyTo(roi_img);
    sample.copyTo(sample);

    CV_Assert(roi_img.channels() == 3);
    CV_Assert(sample.channels() == 3);
    cv::resize(roi_img, roi_img, cv::Size(357, 419), 0, 0, cv::INTER_NEAREST);
    //cv::flip(matSrc1, matSrc1, 1);
    cv::resize(sample, sample, cv::Size(2177, 3233), 0, 0, cv::INTER_LANCZOS4);
    imshow("shit2", roi_img);
    cv::resize(roi_img, roi_img, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
    cv::resize(sample, sample, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);

    imshow("shit", roi_img);


    cv::Mat temp1 = roi_img;
    cv::Mat temp2 = sample;
    cv::cvtColor(temp1 , roi_img, COLOR_BGR2GRAY);
    cv::cvtColor(temp2 , sample, COLOR_BGR2GRAY);

    int iAvg1 = 0, iAvg2 = 0;
    int arr1[64], arr2[64];

    for (int i = 0; i < 8; i++)
    {
        uchar* data1 = roi_img.ptr<uchar>(i);
        uchar* data2 = sample.ptr<uchar>(i);

        int tmp = i * 8;

        for (int j = 0; j < 8; j++)
        {
            int tmp1 = tmp + j;

            arr1[tmp1] = data1[j] / 4 * 4;
            arr2[tmp1] = data2[j] / 4 * 4;

            iAvg1 += arr1[tmp1];
            iAvg2 += arr2[tmp1];
        }
    }

    iAvg1 /= 64;
    iAvg2 /= 64;

    for (int i = 0; i < 64; i++)
    {
        arr1[i] = (arr1[i] >= iAvg1) ? 1 : 0;
        arr2[i] = (arr2[i] >= iAvg2) ? 1 : 0;
    }

    int iDiffNum = 0;
    //等同于计算明汉距离
    for (int i = 0; i < 64; i++)
        if (arr1[i] != arr2[i])
            ++iDiffNum;

    result[m].bi = iDiffNum;
    result[m].num = m;
}

void deal(Mat &src, int order){

    sample = imread("E://pic//number//1.jpg",IMREAD_COLOR);
    Threshold(src, sample, 1);

    sample = imread("E://pic//number//2.jpg",IMREAD_COLOR);
    Threshold(src, sample, 2);

    sample = imread("E://pic//number//3.jpg",IMREAD_COLOR);
    Threshold(src, sample, 3);

    sample = imread("E://pic//number//4.jpg",IMREAD_COLOR);
    Threshold(src, sample, 4);

    sample = imread("E://pic//number//5.jpg",IMREAD_COLOR);
    Threshold(src, sample, 5);

    sample = imread("E://pic//number//6.jpg",IMREAD_COLOR);
    Threshold(src, sample, 6);

    sample = imread("E://pic//number//7.jpg",IMREAD_COLOR);
    Threshold(src, sample, 7);

    sample = imread("E://pic//number//8.jpg",IMREAD_COLOR);
    Threshold(src, sample, 8);



    sort(result+1, result + order);

    if (1) {
//        cout << "识别的" << "数字为 " << result[1].num<< result[2].num << result[3].num << result[4].num << result[5].num  << endl;
        cout << "数字为 " << result[1].num << endl;
    } else cout << "无法识别到相关数字" << endl;
}