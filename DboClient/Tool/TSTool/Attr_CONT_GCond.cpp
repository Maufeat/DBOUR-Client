// Attr_CONT_GCond.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TSTool.h"
#include "Attr_CONT_GCond.h"


// CAttr_CONT_GCond 대화 상자입니다.

IMPLEMENT_SERIAL(CAttr_CONT_GCond, CAttr_Page, 1)

CAttr_CONT_GCond::CAttr_CONT_GCond()
	: CAttr_Page(CAttr_CONT_GCond::IDD)
	, m_tcID(NTL_TS_TC_ID_INVALID)
{

}

CAttr_CONT_GCond::~CAttr_CONT_GCond()
{
}

CString CAttr_CONT_GCond::GetPageData( void )
{
	UpdateData( true );

	CString strData;

	strData += PakingPageData( _T("cid"), m_tcID );
	strData += PakingPageData( _T("rm"), (m_ctrRewardMark.GetCheck() == BST_CHECKED) ? 1 : 0 );

	return strData;
}

void CAttr_CONT_GCond::UnPakingPageData( CString& strKey, CString& strValue )
{
	if ( _T("cid") == strKey )
	{
		m_tcID = atoi( strValue.GetBuffer() );
	}

	if ( _T("rm") == strKey )
	{
		if ( 1 == atoi( strValue.GetBuffer() ) )
		{
			m_ctrRewardMark.SetCheck( BST_CHECKED );
		}
		else
		{
			m_ctrRewardMark.SetCheck( BST_UNCHECKED );
		}
	}
}

void CAttr_CONT_GCond::DoDataExchange(CDataExchange* pDX)
{
	CAttr_Page::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TS_CONT_ATTR_GCOND_ID_EDITOR, m_tcID);
	DDV_MinMaxUInt(pDX, m_tcID, 0, NTL_TS_TC_ID_INVALID);
	DDX_Control(pDX, IDC_TS_CONT_ATTR_GCOND_REWARD_MARK_CHECK, m_ctrRewardMark );
}


BEGIN_MESSAGE_MAP(CAttr_CONT_GCond, CAttr_Page)
END_MESSAGE_MAP()


// CAttr_CONT_GCond 메시지 처리기입니다.
