#pragma once

#include "PolyLineTunnelDraw.h"

// �����һ���Ч������
class PolyLineStorageDrawBase : public PolyLineTunnelDraw
{
public:
	ACRX_DECLARE_MEMBERS(PolyLineStorageDrawBase);

public:
	virtual void setAllExtraParamsToDefault();
	virtual void configExtraParams();
	virtual void updateExtraParams();	

	virtual void readExtraParam(DrawParamReader& reader);
	virtual void writeExtraParam(DrawParamWriter& writer);

protected:
	virtual Adesk::Boolean subWorldDraw (AcGiWorldDraw *mode);	

public:
	void drawBox(AcGiWorldDraw *mode);
	void updateBoxWidth();
	void drawText(AcGiWorldDraw *mode);

	// extra param
	double m_boxHeight; // ���ֿ�߶�,Ĭ��Ϊ60
	double m_boxWidth;  // ���ֿ����,Ĭ��ʼĩ�㳤�ȵ�1/3
	double m_minLength; // ��С����(������ʾ���ֿ�)��Ĭ��150

protected:
	PolyLineStorageDrawBase(const CString& text);
	CString m_text; // Ҫ��ʾ������
};

#ifdef DEFGEPLUGIN_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(PolyLineStorageDrawBase)
#endif