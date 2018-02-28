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
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
using namespace cv;

uint8_t colorData[640 * 480 * 3] = {0};
int number = 0;
char* path = "./Pic";
void HandGrabSessionFrameCallback(ImiHandGrabFrame *data)
{

    pcl::PointCloud<pcl::PointXYZ> cloud;
	int depth_width = data->depth->width;
	int depth_height = data->depth->height;
	//cloud.width = 480;
	//cloud.height = 640;
	cloud.width = depth_width;
	cloud.height = depth_height;
	cloud.points.resize(cloud.width * cloud.height);
    
	if (data != NULL &&  data->color != NULL) 
	{
		memcpy(colorData, data->color->data, data->color->size);
		Mat rgb(480, 640, CV_8UC3, colorData);
		Mat bgr;
		cv::cvtColor(rgb,bgr,cv::COLOR_RGB2BGR);
			
		//if (access(path, NULL) == 0 && data->ROI.width > 0)
		//{
		//	char name[20] = { 0, };
		//	sprintf(name, "./Pic/%s%d.jpg", "test_", number++);
		//	Rect roi(data->ROI.x, data->ROI.y, data->ROI.width, data->ROI.height);
		//	Mat ROI = bgr(roi);
		//	imwrite(name, ROI);
		//}

		for (int j = 0; j < 4; j++)
		{

			
			Point2f p1, p2;
			p1.x = data->ROI.point[j].x;
			p1.y = data->ROI.point[j].y;
			p2.x = data->ROI.point[(j + 1) % 4].x;
			p2.y = data->ROI.point[(j + 1) % 4].y;
			
			line(bgr, p1, p2, Scalar(0, 0, 255), 1, 8);
		}
		rectangle(bgr, Point(data->ROI.x, data->ROI.y), Point(data->ROI.x + data->ROI.width, data->ROI.y + data->ROI.height), Scalar(0, 255, 0), 2, 8);
		imshow("color", bgr);

		if (access(path, NULL) == 0)
		{
			char name[20] = { 0, };
			char name_pcd[20] = {0,};
			sprintf(name, "./Pic/%s%d.jpg", "test_", number++);
			sprintf(name_pcd,"./Pic/%s%d.pcd","test_",number++);
			unsigned short int *pDepthData = (unsigned short int *) data->depth->data;
		
			for(size_t j = 0;j < depth_height;++j)
		    {
			    std::cout<<"writing "<<std::endl;

				
				for(size_t i = 0 ; i < depth_width; ++i)
					 {
						 cloud.points[j * data->depth->width + i].x = i;
						 cloud.points[j * data->depth->width + i].y = j;
						 cloud.points[j * data->depth->width + i].z = pDepthData[j * data->depth->width + i];
					 }
			}
		
            pcl::io::savePCDFileASCII (name_pcd, cloud);
			imwrite(name, bgr);
		}
		waitKey(1);
	}	
}


int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("use ./test near far \n");		
		return -1;
	}
	
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
	
	ImiStopHandGrabSession();
	return 0;
}
