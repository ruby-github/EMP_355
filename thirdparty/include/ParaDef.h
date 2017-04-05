/***************************************************************
 * Name:      ParaDef.h
 * Purpose:   Code for definition of variances
 * Author:    ZCT (zct826@126.com)
 * Created:   2009-04-06
 * Copyright: ZCT
 * License:
 **************************************************************/

#include "WinDef.h"

#ifndef	PARADEF_H_INCLUDE
#define	PARADEF_H_INCLUDE

// ��һ���ֽڣ����������Ѫ����α�ʣ�
// �ڶ����ֽڣ���������ޣ�
#define COLORMAP_PDI_ASIA0		0x0000	// ������ɫ������0������
#define COLORMAP_PDI_ASIA1		0x0001	// ������ɫ������1������
#define COLORMAP_PDI_ASIA2		0x0002	// ������ɫ������2������
#define COLORMAP_PDI_ASIA3		0x0003	// ������ɫ������3������
#define COLORMAP_PDI_ASIA4		0x0004	// ������ɫ������4������
#define COLORMAP_PDI_ASIA5		0x0005	// ������ɫ������

#define COLORMAP_CFM_ASIA0		0x1000	// Ѫ����ɫ������0������
#define COLORMAP_CFM_ASIA1		0x1001	// Ѫ����ɫ������1������
#define COLORMAP_CFM_ASIA2		0x1002	// Ѫ����ɫ������2������
#define COLORMAP_CFM_ASIA3		0x1003	// Ѫ����ɫ������3������
#define COLORMAP_CFM_ASIA4		0x1004	// Ѫ����ɫ������4������
#define COLORMAP_CFM_ASIA5		0x1005	// Ѫ����ɫ������5������
#define COLORMAP_CFMVAR_ASIA0	0x11000	// Ѫ����ɫ������0�����ޣ�������

#define GRAYMAP				0xFFFF	// �Ҷȱ�
#define SIEMENS_PURPLE			0xF001	// ��������ɫ
#define SIEMENS_HYACINTH		0xF002	// �������ٺ�ɫ
#define SIEMENS_CYANINE			0xF0003	// ����������ɫ
#define SIEMENS_KHAKI			0xF0004	// ����������ɫ
#define ROTBRAUM				0xF0005	// ����ɫ
#define HYACINTH				0xF0006	// �ٺ�ɫ
#define BLOND					0xF0007	// ���ɫ
#define SIXCOLOR				0xF0008	// 6ɫ
#define EFOVCOLOR				0xF0009 // EFOV speed color


#define COLOR_MAX				255		// ��ɫ���ֵ
#define COLOR_MIN				0		// ��ɫ��Сֵ
#define COLOR_NUM				4		// ��ɫ���ࡣ4--��͸��ɫ
#define COLOR_GRADE				256		// ��ɫ����
#define VARIANCE_GRADE			7		// �����


#define PI					3.1415926	// Բ����
#define eps					0.000000001	// ����


typedef struct tagRGBQUAD
{
	BYTE rgbBlue;		// ��ɫ������(ֵ��ΧΪ0-255) 
	BYTE rgbGreen;		// ��ɫ������(ֵ��ΧΪ0-255) 
	BYTE rgbRed;		// ��ɫ������(ֵ��ΧΪ0-255) 
	BYTE rgbReserved;	// ����������Ϊ0
} RGBQUAD;


#endif 	// #ifndef PARADEF_H_INCLUDE
