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
		Mat imgOriginal;//��v��
		Mat imgHSV;//�নhsv�v��
		Mat imgProcessing;//�v���B�z �L�{
		Mat imgThresholded;//�G�Ȥ�(�T�w�H��)
		Mat imgDst;//�v���B�z ����
		int valThres = 100;

		cap.read(imgOriginal);
		cap.grab();//read=grab+retrieve grab=���  retrieve ����+�ѽX
		cap.set(CV_CAP_PROP_FRAME_WIDTH, windowsWidth); //cap.set(3, width);
		cap.set(CV_CAP_PROP_FRAME_HEIGHT, windowsHeight); //cap.set(4, height);
		//cap.set(CAP_PROP_FPS, 30);


		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);//�নhsv����
		GaussianBlur(imgHSV, imgProcessing, Size(5, 5), 1.3, 1.3);//�v������������
		//threshold(imgProcessing, imgProcessing, valThres, 255, THRESH_BINARY_INV);//�T�w�֭�
		inRange(imgProcessing, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

		Mat getElement = getStructuringElement(MORPH_RECT, Size(5, 5));//�n�i��I�k�ο��Ȫ��ҪO
		morphologyEx(imgThresholded, imgDst, MORPH_OPEN, getElement);//�}�B��
		morphologyEx(imgThresholded, imgDst, MORPH_CLOSE, getElement);//�׹B��

		//����
		vector< vector <Point> > contoursFind, contoursDraw;//�]�t�Ҧ��������e��
		vector< Vec4i > imgHierarchy;//��X�V�q�A�O���Ҧ�����
		Scalar contoursColor = Scalar(255, 0, 0);
		findContours(imgThresholded, contoursFind, noArray(), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		vector<Point2f> center( contoursFind.size() );
		vector<float> radius( contoursFind.size() );
		vector<Rect> boundRect( contoursFind.size() );
		vector<RotatedRect> minARect( contoursFind.size() );

		for (int i = 0 ; i < contoursFind.size(); i++) {
			
			/*
			//�̤p���
			int radius_int = cvRound(radius[i]);
			minEnclosingCircle(contoursFind[i], center[i], radius[i]);//�^���̤p��
			Point center_p = (Point)center[i];
			circle(imgOriginal, center_p, radius_int, Scalar(0, 0, 255), 1, CV_AA);//ø�X
			circle(imgOriginal, center_p, 1, Scalar(0, 255, 0), -1, CV_AA);//ø�X
			*/
			

			//�̤p�x�����(�������)
			boundRect[i] = boundingRect( Mat(contoursFind[i]) );
			rectangle(imgOriginal, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 2, 8, 0);
			//.tl()���W .br()�k�U

			//�̤p�ׯx�����
			minARect[i] = minAreaRect( Mat(contoursFind[i]) );
			Point2f minRectPoints[4];
			minARect[i].points(minRectPoints);
			for (int j = 0; j < 4; j++)
			line(imgOriginal, minRectPoints[j], minRectPoints[(j + 1) % 4], Scalar(255, 0, 255), 1, 8);

			//�p�⭱�n
			double contourA = contourArea(contoursFind[i], false); //�ĤG�ѼƬ���V��
			//cout << "Area : " << contourA << endl;
			if ( contourA > (windowsWidth*windowsHeight)*1/4 )
			{
				cout << "0" << endl;//true
			}
			else 
			{
				cout << "1" << endl;//false
			}

			////�p��P��
			//double contourL = arcLength(contoursFind[i], false);
			//cout << "Length : " << contourL << endl;
		}
		//show
		//imshow("Thresholded Image", imgThresholded); //show the thresholded image
		imshow("Original Image", imgOriginal); //show the original imageimg
		imshow("Dst Image", imgDst); //show the dst image
		
	}
}