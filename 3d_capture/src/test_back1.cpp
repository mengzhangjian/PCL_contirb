/********************************************************************************
Copyright (c) 2015 Huajie IMI Technology Co., Ltd.
All rights reserved.

@File Name     : test.cpp
@Author        : Sun Jie
@Date          : 2018-01-12
@Description   : hand grab api example
@Version       : 0.1.0
@History       :
********************************************************************************/

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "ImiHandGrabSession.h"
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/features2d/features2d.hpp>
#include <math.h>
using namespace cv;

uint8_t colorData[640 * 480 * 3] = {0};
int number = 0;
char* path = "./Pic";
FILE *fdata = NULL;
bool setBackground = false;
RNG rng(12345);
Mat RegionGrow(Mat src, cv::Point2i pt, int th)  
{  
    Point2i ptGrowing;                      //待生长点位置  
    int nGrowLable = 0;                             //标记是否生长过  
    int nSrcValue = 0;                              //生长起点灰度值  
    int nCurValue = 0;                              //当前生长点灰度值  
    Mat matDst = Mat::zeros(src.size(), CV_8UC1);   //创建一个空白区域，填充为黑色  
    //生长方向顺序数据  
    int DIR[8][2] = {{-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}};    
    Vector<Point2i> vcGrowPt;                     //生长点栈  
    vcGrowPt.push_back(pt);                         //将生长点压入栈中  
    matDst.at<uchar>(pt.y, pt.x) = 255;               //标记生长点  
    nSrcValue = src.at<uchar>(pt.y, pt.x);            //记录生长点的灰度值  
      
    while (!vcGrowPt.empty())                       //生长栈不为空则生长  
    {  
        pt = vcGrowPt.back();                       //取出一个生长点  
        vcGrowPt.pop_back();                          
  
        //分别对八个方向上的点进行生长  
        for (int i = 0; i<9; ++i)  
        {  
            ptGrowing.x = pt.x + DIR[i][0];       
            ptGrowing.y = pt.y + DIR[i][1];   
            //检查是否是边缘点  
            if (ptGrowing.x < 0 || ptGrowing.y < 0 || ptGrowing.x > (src.cols-1) || (ptGrowing.y > src.rows -1))  
                continue;  
  
            nGrowLable = matDst.at<uchar>(ptGrowing.y, ptGrowing.x);      //当前待生长点的灰度值  
  
            if (nGrowLable == 0)                    //如果标记点还没有被生长  
            {  
                nCurValue = src.at<uchar>(ptGrowing.y, ptGrowing.x);            
                if (abs(nSrcValue - nCurValue) < th)                 //在阈值范围内则生长  
                {  
                    matDst.at<uchar>(ptGrowing.y, ptGrowing.x) = 255;     //标记为白色  
                    vcGrowPt.push_back(ptGrowing);                  //将下一个生长点压入栈中  
                }  
            }  
        }  
    }  
    return matDst.clone();  
}  
Mat clipBackground(Mat &fMat,float dThr,float hThr)
{
    for(int i = 0 ; i < fMat.rows;i++)
        for(int j =0;j < fMat.cols;j++)
        {

            float val = (fMat.at<float>(i,j) > dThr && fMat.at<float>(i,j) < hThr) ? 255.0 : 0.0;
            fMat.at<float>(i,j) = val;
			
        }
        return fMat;
}
void HandGrabSessionFrameCallback(ImiHandGrabFrame *data)
{
    if (data != NULL &&  data->color != NULL && data->depth != NULL)
    {
        // memcpy(colorData, data->color->data, data->color->size);
        // Mat rgb(480, 640, CV_8UC3, colorData);
        // Mat bgr;
        // cv::cvtColor(rgb,bgr,cv::COLOR_RGB2BGR);

        fwrite((char *)data->depth->data, 1, data->depth->size, fdata);
        unsigned short int * pDepthData = (unsigned short int *)data->depth->data;
        int depth_width = data->depth->width;
        int depth_height = data->depth->height;
        for (int j = 0; j < depth_height; ++j)
        {
            for (int i = 0; i < depth_width; ++i)
            {
                int depth = pDepthData[j * data->depth->width + i];
				//std::cout<<"depth is "<<depth<<std::endl;
				
            }
        }
      vector< vector<cv::Point> > contours;
      vector<Vec4i> hierarchy;
      vector<float> zmap;
      for (int j = 0; j < depth_height; ++j)
      {
          for (int i = 0; i < depth_width; ++i)
          {
               zmap.push_back(pDepthData[j * data->depth->width + i]);
          }
      }
      
        Mat dMat = Mat(480,640,CV_32FC1,zmap.data());
        Mat drawing = Mat::zeros(480,640,CV_8UC3);
        memcpy(colorData, data->color->data, data->color->size);
        Mat rgb(480,640, CV_8UC3, colorData);
        cvtColor(rgb,drawing,CV_RGB2BGR);
       Mat resultMat = clipBackground(dMat,350,800);
       Mat bMat;
         resultMat.convertTo(bMat,CV_8U,255.0);
       Mat morphMat = bMat.clone();
       Mat element  = getStructuringElement(0,Size(3,3),Point(1,1));
       morphologyEx(bMat,morphMat,2,element);
       findContours(morphMat,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_NONE,Point(0,0));

        vector<vector<Point>> hull(contours.size());
        vector<vector<Point> > contours_poly( contours.size() );
        vector<vector<Point> > contours_hull( contours.size() );
        vector<Rect> boundRect( contours.size() );
        vector<Point2f>center( contours.size() );
        vector<float>radius( contours.size() );
        vector<RotatedRect> minRect(1);
        cv::Point2i point;
        vector<vector<int> >  hullsI(contours.size());
        vector<vector<Vec4i>> defects( contours.size()) ;
     // find the largest contour area

     int largest_area = 0;
     int largest_index = 0;
     if(contours.size() !=0)
     {
        for( int i = 0; i < contours.size(); i++ )
        { 
            //approxPolyDP( Mat(contours[i]), contours_poly[i], 1, true );
    //         if(contourArea(contours_poly[i])>100)
    //         {     
    //         boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    //    //minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
    //    //point.push_back(center[i]);
    //         }
            if(largest_area < contourArea(contours[i]))
            {
                largest_area = contourArea(contours[i]);
                largest_index = i;
            }
        }
     cv::drawContours(drawing, contours, largest_index, cv::Scalar(0, 0, 255), 1);
      // Convex hull
      if (!contours.empty())
      {
          std::vector<std::vector<cv::Point> > hull(1);
          cv::convexHull(cv::Mat(contours[largest_index]), hull[0], false);
          cv::drawContours(drawing, hull, 0, cv::Scalar(0, 255, 0), 3);
          if(hull[0].size()>2)
          {
             std::vector<int> hullIndexes;
              cv::convexHull(cv::Mat(contours[largest_index]), hullIndexes, true);
              std::vector<cv::Vec4i> convexityDefects;
              cv::convexityDefects(cv::Mat(contours[largest_index]), hullIndexes, convexityDefects);
           for (size_t i = 0; i < convexityDefects.size(); i++)
              {
                  cv::Point p1 = contours[largest_index][convexityDefects[i][0]];
                  cv::Point p2 = contours[largest_index][convexityDefects[i][1]];
                  cv::Point p3 = contours[largest_index][convexityDefects[i][2]];
                  cv::line(drawing, p1, p3, cv::Scalar(255, 0, 0), 2);
                  cv::line(drawing, p3, p2, cv::Scalar(255, 0, 0), 2);
              }
          }
          minRect[0] = minAreaRect(Mat(contours[largest_index]));
          boundRect[0] = boundingRect( Mat(contours[largest_index]) );
          rectangle( drawing, boundRect[0].tl(), boundRect[0].br(), CV_RGB(0,0,0), 2, 8, 0 );
           Point2f rect_points[4];
           minRect[0].points( rect_points );

                  for( int j = 0; j < 4; j++ )
                      {
                        //draw rotated rectangle for the biggest contour
                        line(drawing, rect_points[j], rect_points[(j+1)%4], CV_RGB(255,255,0), 2, 8 );
                      }
      }
      cv::imshow("windowName", drawing);
      waitKey(1);
  
  }
  else
  {
      cv::imshow("windowName", drawing);
      waitKey(1);

  }

    //     for(int i = 0;i <contours.size();i++)
    //     {
    //         convexHull(Mat(contours[i]),hull[i],false);
    //         convexityDefects(Mat(contours[i]),hullsI[i], defects[i]);
    //        if(i == largest_index)
    //        {
    //            minRect[i] = minAreaRect(Mat(contours[i]));
    //            drawContours( drawing, contours,largest_index, CV_RGB(255,255,255), 2, 8, hierarchy,0, Point() );
    //            drawContours( drawing, hull, largest_index, CV_RGB(255,0,0), 2, 8, hierarchy, 0, Point() );
    //            boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    //            rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), CV_RGB(0,0,0), 2, 8, 0 );
    //            Point2f rect_points[4];
    //               minRect[i].points( rect_points );

    //               for( int j = 0; j < 4; j++ )
    //                   {
    //                     //draw rotated rectangle for the biggest contour
    //                     line(drawing, rect_points[j], rect_points[(j+1)%4], CV_RGB(255,255,0), 2, 8 );
    //                   }
    //        }
    //     }

    //    imshow("img",morphMat);
	//     waitKey(1);

    //  }

    //   else
    //   {
    //     imshow("img",morphMat);
	//     waitKey(1); 
    //   }

    //  minEnclosingCircle( (Mat)contours_poly[largest_index], center[largest_index], radius[largest_index] );
    //  //Mat finalMat = RegionGrow(morphMat,point,5);
     


    //    circle( drawing, center[largest_index], (int)radius[largest_index], Scalar(0,255,0), 2, 8, 0 );
    //    point = center[largest_index];
    //    Mat finalResult = RegionGrow(morphMat,point,50);
    //    imshow("img",resultMat);
	//    waitKey(1);


        std::cout<< contours.size()<<std::endl;


   
//           /// Draw contours + hull results
//    for( int i = 0; i< contours.size(); i++ )
//       {
//         Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
//         //drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
//         drawContours( drawing, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
//       }
    //      for( int i = 0; i< contours.size(); i++ )
    //  {
    //    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    //    drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
    //    //rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
    //     //circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
    //  }
    //      /// Approximate contours to polygons + get bounding rects and circles

    //  int largest_area = 0;
    //  int largest_index = 0;
    //  if(contours.size() !=0)
    //  {
    //  for( int i = 0; i < contours.size(); i++ )
    //  { approxPolyDP( Mat(contours[i]), contours_poly[i], 1, true );
    //    if(contourArea(contours_poly[i])>100)
    //    {     
    //    boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    //    //minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
    //    //point.push_back(center[i]);
    //    }
    //    if(largest_area < contourArea(contours_poly[i]))
    //    {
    //        largest_area = contourArea(contours_poly[i]);
    //         largest_index = i;
    //    }
    //  }
    //  }
    //  minEnclosingCircle( (Mat)contours_poly[largest_index], center[largest_index], radius[largest_index] );
    //  //Mat finalMat = RegionGrow(morphMat,point,5);
     


    //    circle( drawing, center[largest_index], (int)radius[largest_index], Scalar(0,255,0), 2, 8, 0 );
    //    point = center[largest_index];
    //    Mat finalResult = RegionGrow(morphMat,point,50);
    //    imshow("img",resultMat);
	//    waitKey(1);
    //  }
    //   else
    //  {
    //     imshow("img",resultMat);
	//     waitKey(1);
    //   }
    // vector<Rect> minRect( contours.size() );
    //  for( int i = 0; i < contours_hull.size(); i++ )
    //  { 
    //      convexHull( Mat(contours_hull[i]), hull[i], false );
    //      convexHull( Mat(contours_hull[i]), hullsI[i], false );
    //      convexityDefects(Mat(contours_hull[i]),hullsI[i], defects[i]);

    //         if(largest_index == i)
    //            {
    //               minRect[i] = minAreaRect( Mat(contours_hull[i]) );
                    
    //              //draw contour of biggest object
    //               drawContours( frame2, contours_hull,IndexOfBiggestContour, CV_RGB(255,255,255), 2, 8, hierarchy,0, Point() );
    //             //draw hull of biggesr object
    //               drawContours( frame2, hull, IndexOfBiggestContour, CV_RGB(255,0,0), 2, 8, hierarchy, 0, Point() );

                  
    //               approxPolyDP( Mat(contours_hull[i]), contours_poly[i], 3, true );
    //               boundRect[i] = boundingRect( Mat(contours_poly[i]) );
                 
    //               //draw rectangle for the biggest contour
    //               rectangle( frame2, boundRect[i].tl(), boundRect[i].br(), CV_RGB(0,0,0), 2, 8, 0 );

    //               Point2f rect_points[4];
    //               minRect[i].points( rect_points );

    //               for( int j = 0; j < 4; j++ )
    //                   {
    //                     //draw rotated rectangle for the biggest contour
    //                     line( frame2, rect_points[j], rect_points[(j+1)%4], CV_RGB(255,255,0), 2, 8 );
    //                   }

    //            }
    //  }







  

    }
}


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("use ./test near far \n");
        return -1;
    }
    fdata = fopen("depthfile", "wb");

    //if (access(path, NULL) != 0)
    //{
    //	if (mkdir(path, 0755) == -1)
    //	{
    //		perror("mkdir   error");
    //		return -1;
    //	}
    //}

    for (int i = 0; i <= argc; i++)
    {
        printf("argv[%d]:%s\n", i, argv[i]);
    }

    std::cout << "Thread Started" << std::endl;

    ImiCreatHandGrabSession(atoi(argv[1]), atoi(argv[2]));

    ImiSetFrameCallback(HandGrabSessionFrameCallback);

    getchar();
    fclose(fdata);
    ImiStopHandGrabSession();
    return 0;
}
