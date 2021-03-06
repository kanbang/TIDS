#include "StdAfx.h"
#include "WindageCacul.h"
#include "NetWorkData.h"

#include "../MineGE/HelperClass.h"
#include "../DrawVentCmd/DrawCmd.h"

#include "../DefGE/Fan.h"
#include "../DefGE/Chimney.h"

extern Chimney_DataLink* ChimneyDatas(AcDbObjectId objId );
WindageCacul::WindageCacul(AcDbObjectId objId):m_ksaiJO(0.15),m_ksaiIN(0.6),m_ksaiON(1),m_ro(1.2)
{
	initDatas(objId);
}

WindageCacul::~WindageCacul()
{

}

void WindageCacul::initDatas(AcDbObjectId objId)
{
	Chimney_DataLink *cdl = ChimneyDatas(objId);
	m_lenth = cdl->lenth;
	m_bends = cdl->bends;
	m_diam = cdl->diameter;
	m_meter = cdl->tm;
	m_joint = cdl->joints;
	m_alpha = cdl->friction;
	m_hmWindage = cdl->hmWindage;

	setMethodByFan(objId);
	setKsaiBEByBend();
}


void WindageCacul::setMethodByFan(AcDbObjectId objId)
{
	AcDbObjectId fanId;
	DrawHelper::GetHostGE(objId,fanId);

	DataHelper::GetPropertyData( fanId, _T( "工作方式" ), m_method );

}

void WindageCacul::setKsaiBEByBend()
{
	m_ksaiBE = 0 ;
	//分割得到的字符
	TCHAR *token;
	//分割的符号,这里用法的是逗号
	TCHAR *seps;
	//为了方便分割字符串，把其他的特殊符号都转换成逗号
	m_bends.Replace(_T(" "),_T(","));
	m_bends.Replace(_T("\\"),_T(","));
	m_bends.Replace(_T("/"),_T(","));
	m_bends.Replace(_T(";"),_T(","));
	m_bends.Replace(_T("'"),_T(","));
	m_bends.Replace(_T("|"),_T(","));

	seps  = _T(",");
	//分割得到第一个字符串
	token = _tcstok( (LPTSTR)(LPCTSTR)m_bends, seps );
	//获取其他被分割的字符串
	while( token != NULL )
	{
		//acutPrintf(_T("\n弯角:%s"),token);
		CString bend;
		bend.Format(_T("%s"),token);
		m_ksaiBE += 7e-7 * _tstof(bend) * _tstof(bend) * _tstof(bend) + 1e-6 * _tstof(bend) * _tstof(bend) + 0.007 * _tstof(bend) - 0.009;
		//acutPrintf(_T("\n弯角阻力:%lf"),7e-7 * _tstof(bend) * _tstof(bend) * _tstof(bend) + 1e-6 * _tstof(bend) * _tstof(bend) + 0.007 * _tstof(bend) - 0.009);
		token = _tcstok( NULL, seps );
	}
}

double WindageCacul::windageCaculRet()
{
	double Rp,Rfr,Rbe,Rjo,RonOrin;
	if(m_diam == 0) return 0;
	m_diam = m_diam * 0.001;	//把单位从mm化成m
	double area = PI * pow(m_diam * 0.5,2);

	CString msg;
	if (_T("压入式") == m_method)
	{
		RonOrin = (m_ksaiON * m_ro  * 0.5) / pow(area,2);
		msg.Append(_T("\n出口风阻:%fN·s^2/m^8"));
		//acutPrintf(_T("\n压入式"));
	}
	else if (_T("抽出式") == m_method)
	{
		RonOrin = (m_ksaiIN * m_ro  * 0.5) / pow(area,2);
		msg.Append(_T("\n入口风阻:%fN·s^2/m^8"));
		//acutPrintf(_T("\n抽出式"));
	}
	else
	{
		AfxMessageBox(_T("风筒没有连接到风机\n或者所连风机没有设置工作方式!"),MB_OK | MB_ICONSTOP);
		return 0;
	}

	if (_T("金属风筒") == m_meter)
	{
		Rjo = 0;
		//acutPrintf(_T("\n金属风筒"));
	}
	else if (_T("胶布风筒") == m_meter)
	{
		Rjo = ( m_joint * m_ksaiJO *  m_ro  * 0.5 ) / pow(area,2);
		//acutPrintf(_T("\n胶布风筒"));
	}
	else
	{
		AfxMessageBox(_T("风筒的属性没有设置!"),MB_OK | MB_ICONSTOP);
		return 0;

	}

	Rbe = ( m_ksaiBE *  m_ro  * 0.5 ) / pow(area,2);
	Rfr = ( 6.5 * m_alpha * m_lenth )/ pow(m_diam,5);

	Rp = Rfr + Rjo + Rbe + RonOrin;

	CString printMsg = _T("\n沿程风阻：%fN·s^2/m^8\n局部风阻:%fN·s^2/m^8\n弯头风阻:%fN·s^2/m^8");
	printMsg.Append(msg);
	printMsg.Append(_T("\n总风阻:%fN·s^2/m^8"));
	acutPrintf(printMsg,Rfr,Rjo,Rbe,RonOrin,Rp);
	return Rp;
}

double WindageCacul::windageCaculByHM()
{
	return m_hmWindage * m_lenth / 100;
}
