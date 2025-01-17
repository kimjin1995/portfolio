#include"SnakeGame.h"
#include"Node.h"

// Project1Dlg.h: 헤더 파일
//

#pragma once


// CProject1Dlg 대화 상자
class CProject1Dlg : public CDialogEx
{
// 생성입니다.
public:
	CProject1Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJECT1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void DrawImage(CDC *pDC);
private :
	int m_right,m_up;
	int snakelength = 0; // set initialized variable.
	SnakeGame list; // set snake linked list


};
