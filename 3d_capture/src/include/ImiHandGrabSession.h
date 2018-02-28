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
	int x;//��ǿ����Ͻ�x
	int y;//��ǿ����Ͻ�y
	int width;//��ǿ���
	int height;//��ǿ��
	int nearDis;//��ǿ�������ͷ������뵥λ����
}ImiImageROI;

typedef struct tag_ImiImage 
{
	uint64_t timeStamp;
	void *data;
	int size;
	int width;
	int height;
} ImiImage;

//�ص����ݸ�ʽ
typedef struct tag_ImiHandGrabFrame
{
	//�ص���ͼ����
	ImiImage *color;
	//�ص��������
	ImiImage *depth;
	//��ǿ�
	ImiImageROI ROI;
} ImiHandGrabFrame;

//ʶ�����ݻص��ӿ�

typedef void(*ImiFrameCallBack)(ImiHandGrabFrame *data);

#ifdef __cplusplus
extern "C" {
#endif
/**
* @brief  ���豸����ʶ�𣬲�����ʶ����� near��far Ϊ�����Զ���룬��λΪ����
*         �ֱ���Ĭ�ϴ򿪵���640*480���ݲ�֧���޸� 
* @param  int near: С��near���벻����
* @param  int far�� ����far���벻����
* @return 0: success, < 0 : error
**/
int ImiCreatHandGrabSession(int near, int far);

/**
* @brief  �ر��豸���ر�ʶ��
*
* @return 0: success, < 0 : error
**/
int ImiStopHandGrabSession();

/**
* @brief  ���ûص����ݽӿ�

* @param  ImiFrameCallBack callback �ص�����
* 
* @return 0: success, < 0 : error
**/
int ImiSetFrameCallback(ImiFrameCallBack callback);

#ifdef __cplusplus
}
#endif


#endif // !__IMI_HANDGRABSESSION_H__












