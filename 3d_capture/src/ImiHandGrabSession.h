#ifndef __IMI_HANDGRABSESSION_H__
#define __IMI_HANDGRABSESSION_H__

struct PointPos
{
	int x;
	int y;
};
typedef struct tag_ImiImageROI
{
	PointPos point[4];
	int x;//标记框左上角x
	int y;//标记框左上角y
	int width;//标记框宽度
	int height;//标记框高
	int nearDis;//标记框离摄像头最近距离单位毫米
}ImiImageROI;

typedef struct tag_ImiImage 
{
	uint64_t timeStamp;
	void *data;
	int size;
	int width;
	int height;
} ImiImage;

//回调数据格式
typedef struct tag_ImiHandGrabFrame
{
	//回调抠图数据
	ImiImage *color;
	//回掉深度数据
	ImiImage *depth;
	//标记框
	ImiImageROI ROI;
} ImiHandGrabFrame;

//识别数据回调接口

typedef void(*ImiFrameCallBack)(ImiHandGrabFrame *data);

#ifdef __cplusplus
extern "C" {
#endif
/**
* @brief  打开设备启动识别，并设置识别距离 near，far 为最近最远距离，单位为毫米
*         分辨率默认打开的是640*480，暂不支持修改 
* @param  int near: 小于near距离不处理
* @param  int far： 大于far距离不处理
* @return 0: success, < 0 : error
**/
int ImiCreatHandGrabSession(int near, int far);

/**
* @brief  关闭设备并关闭识别
*
* @return 0: success, < 0 : error
**/
int ImiStopHandGrabSession();

/**
* @brief  设置回调数据接口

* @param  ImiFrameCallBack callback 回调函数
* 
* @return 0: success, < 0 : error
**/
int ImiSetFrameCallback(ImiFrameCallBack callback);

#ifdef __cplusplus
}
#endif


#endif // !__IMI_HANDGRABSESSION_H__












