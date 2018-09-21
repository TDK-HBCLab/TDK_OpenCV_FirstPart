#include "stdafx.h"
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	VideoCapture cap(1); //capture the video from web cam

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	int iLowH = 38;
	int iHighH = 75;

	int iLowS = 127;
	int iHighS = 255;

	int iLowV = 127;
	int iHighV = 255;

	//Create trackbars in "Control" window
	cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

	cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);

	int windowsWidth  = 320;
	int	windowsHeight = 240;

	while (char(waitKey(1)) != 27) 
	{
		Mat imgOriginal;//原影像
		Mat imgHSV;//轉成hsv影像
		Mat imgProcessing;//影像處理 過程
		Mat imgThresholded;//二值化(固定閾值)
		Mat imgDst;//影像處理 完成
		int valThres = 100;

		cap.read(imgOriginal);
		cap.grab();//read=grab+retrieve grab=抓取  retrieve 接收+解碼
		cap.set(CV_CAP_PROP_FRAME_WIDTH, windowsWidth); //cap.set(3, width);
		cap.set(CV_CAP_PROP_FRAME_HEIGHT, windowsHeight); //cap.set(4, height);
		//cap.set(CAP_PROP_FPS, 30);


		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);//轉成hsv平面
		GaussianBlur(imgHSV, imgProcessing, Size(5, 5), 1.3, 1.3);//影像做高斯平滑
		//threshold(imgProcessing, imgProcessing, valThres, 255, THRESH_BINARY_INV);//固定閥值
		inRange(imgProcessing, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

		Mat getElement = getStructuringElement(MORPH_RECT, Size(5, 5));//要進行侵蝕或膨脹的模板
		morphologyEx(imgThresholded, imgDst, MORPH_OPEN, getElement);//開運算
		morphologyEx(imgThresholded, imgDst, MORPH_CLOSE, getElement);//避運算

		//輪廓
		vector< vector <Point> > contoursFind, contoursDraw;//包含所有輪廓之容器
		vector< Vec4i > imgHierarchy;//輸出向量，記錄所有輪廓
		Scalar contoursColor = Scalar(255, 0, 0);
		findContours(imgThresholded, contoursFind, noArray(), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		vector<Point2f> center( contoursFind.size() );
		vector<float> radius( contoursFind.size() );
		vector<Rect> boundRect( contoursFind.size() );
		vector<RotatedRect> minARect( contoursFind.size() );

		for (int i = 0 ; i < contoursFind.size(); i++) {
			
			/*
			//最小圓形
			int radius_int = cvRound(radius[i]);
			minEnclosingCircle(contoursFind[i], center[i], radius[i]);//擷取最小圓
			Point center_p = (Point)center[i];
			circle(imgOriginal, center_p, radius_int, Scalar(0, 0, 255), 1, CV_AA);//繪出
			circle(imgOriginal, center_p, 1, Scalar(0, 255, 0), -1, CV_AA);//繪出
			*/
			

			//最小矩形邊框(平行視窗)
			boundRect[i] = boundingRect( Mat(contoursFind[i]) );
			rectangle(imgOriginal, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 2, 8, 0);
			//.tl()左上 .br()右下

			//最小斜矩形邊框
			minARect[i] = minAreaRect( Mat(contoursFind[i]) );
			Point2f minRectPoints[4];
			minARect[i].points(minRectPoints);
			for (int j = 0; j < 4; j++)
			line(imgOriginal, minRectPoints[j], minRectPoints[(j + 1) % 4], Scalar(255, 0, 255), 1, 8);

			//計算面積
			double contourA = contourArea(contoursFind[i], false); //第二參數為方向性
			//cout << "Area : " << contourA << endl;
			if ( contourA > (windowsWidth*windowsHeight)*1/4 )
			{
				cout << "0" << endl;//true
			}
			else 
			{
				cout << "1" << endl;//false
			}

			////計算周長
			//double contourL = arcLength(contoursFind[i], false);
			//cout << "Length : " << contourL << endl;
		}
		//show
		//imshow("Thresholded Image", imgThresholded); //show the thresholded image
		imshow("Original Image", imgOriginal); //show the original imageimg
		imshow("Dst Image", imgDst); //show the dst image
		
	}
}