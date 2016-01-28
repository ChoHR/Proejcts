/*
 * histogram�� ����Ͽ� ���� �ӿ��� ����� ã�� ���α׷�.
 * ���� �� ���� ������ histogram�� ���ϰ�, ����� histogram�� ����.
 * ���� ����� histogram�� ���� ������ ����� ���̶� �����Ͽ� üũ �� ������ش�.
 * ����� histogram�� ���ϱ� ���� ������ ����� ã����� ������ �ʿ��ϴ�.
 */

/*
 * title  : Recognition by Histogram
 * school : Hanyang univ 
 * major  : CSE
 * date   : 2014-06-25
 * writed by CHR
 */

#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include<string.h>

// Used for library linking
#ifdef _DEBUG
#pragma comment(lib,"opencv_core2410d.lib")
#pragma comment(lib,"opencv_highgui2410d.lib")
#pragma comment(lib,"opencv_imgproc2410d.lib")
#pragma comment(lib,"opencv_video2410d.lib")
#else
#pragma comment(lib,"opencv_core2410.lib")
#pragma comment(lib,"opencv_highgui2410.lib")
#pragma comment(lib,"opencv_imgproc2410.lib")
#pragma comment(lib,"opencv_video2410.lib")
#endif

int main()
{
   double dintersect = 0;
   double MAX_dintersect = 0;
   int hist_size[] = {256};   
   int ROI_x,ROI_y,ROI_size;
   int MAX_x,MAX_y,MAX_size;
   IplImage *Image = cvLoadImage("Ball_Color1.jpg"); // �󱸰��� ������׷��� ���ϱ� ���� �󱸰� �׸�����
   IplImage *finalImage = cvLoadImage("test8.jpg"); // �󱸰��� ã���� �ϴ� �׸�����
   IplImage *dstImage = cvLoadImage("test8.jpg") ; // �󱸰��� ã���� �ϴ� �׸�����
   cvNamedWindow("image_before",0);
   cvResizeWindow("image_before",500,500);
   cvShowImage("image_before",dstImage);
   IplImage *hsv1 = cvCreateImage(cvGetSize(Image), 8, 3);
   IplImage *h1 = cvCreateImage(cvGetSize(Image), 8, 1);
   IplImage *s1 = cvCreateImage(cvGetSize(Image), 8, 1);
   IplImage *v1 = cvCreateImage(cvGetSize(Image), 8, 1);
   
   cvCvtColor(Image, hsv1, CV_BGR2HSV);
   cvSplit(hsv1, h1, s1, v1, NULL);


   // �׸��� �κ����� �� ���� �󱸰��� ������׷��� ��ġ�ϴ� ������ ã�´�.
   for(ROI_size = 50; ROI_size < Image->height; ROI_size+=10)
      for(ROI_y = 0; ROI_y < dstImage->height-ROI_size; ROI_y+=ROI_size/3)
         for(ROI_x = 0; ROI_x < dstImage->width-ROI_size; ROI_x+=ROI_size/3){
            
            cvSetImageROI(dstImage, cvRect(ROI_x,ROI_y,ROI_size,ROI_size));
            IplImage *hsv = cvCreateImage(cvGetSize(dstImage), 8, 3);
            IplImage *h = cvCreateImage(cvGetSize(dstImage), 8, 1);
            IplImage *s = cvCreateImage(cvGetSize(dstImage), 8, 1);
            IplImage *v = cvCreateImage(cvGetSize(dstImage), 8, 1);

            cvCvtColor(dstImage, hsv, CV_BGR2HSV);
            cvSplit(hsv, h, s, v, NULL);

             CvHistogram *hist1 = cvCreateHist(1, hist_size, CV_HIST_ARRAY);
             CvHistogram *hist3 = cvCreateHist(1, hist_size, CV_HIST_ARRAY);
             cvCalcHist( &h1, hist1 ); 
             cvCalcHist( &s1, hist3 ); 
             CvHistogram *hist2 = cvCreateHist(1, hist_size, CV_HIST_ARRAY);
             CvHistogram *hist4 = cvCreateHist(1, hist_size, CV_HIST_ARRAY);
             cvCalcHist( &h, hist2 ); 
             cvCalcHist( &s, hist4 ); 
             dintersect = cvCompareHist(hist1,hist2,CV_COMP_CORREL);
             dintersect += cvCompareHist(hist3,hist4,CV_COMP_CORREL);
             if(dintersect > MAX_dintersect){
                MAX_x = ROI_x;
                MAX_y = ROI_y;
                MAX_size = ROI_size;
                MAX_dintersect = dintersect;
             }
            cvResetImageROI(dstImage);
         }
   
   cvCircle(dstImage,cvPoint(MAX_x+MAX_size/2,MAX_y+MAX_size/2),MAX_size/2,CV_RGB(0,0,250),3);
   
   cvNamedWindow("image_after",0);
   cvResizeWindow("image_after",500,500);
   cvShowImage("image_after",dstImage);
   cvWaitKey(0);

   return 0;
}