
// Project1Dlg.cpp: 구현 파일
//
#include "stdafx.h"
#include "Project1.h"
#include "Project1Dlg.h"
#include "afxdialogex.h"
#include"SnakeGame.h"
#include"Node.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{

public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CProject1Dlg 대화 상자



CProject1Dlg::CProject1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROJECT1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProject1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProject1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CProject1Dlg 메시지 처리기

BOOL CProject1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (MessageBox(L"이 게임은 뱀에 대한 잔혹한 묘사가 포함되어 있습니다.\n무서운 것에 서투른 분은 플레이를 삼가해 주십시오.\n게임 진행을 원하십니까??", L"경고문", MB_YESNO) == IDNO)
		this->DestroyWindow();
	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	MoveWindow(0, 0, 825, 825); // window make, window size is 825 825

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_right = 385; // set initialized valu. snake's start coordinates.
	m_up = 375;

	SetTimer(0, 55, nullptr); // set timer. time interval is 55ms.


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CProject1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.
#include<ctime>
#include<cstdlib> // header file set
int eat = 0; // if value is 0, food make.
int snakeeat;
int eatrow;	   // for row variable
int eatcolumn; // food column variable
void CProject1Dlg::DrawImage(CDC *pDC) // double buffering function set.
{
	CPaintDC dc(this);
	CBrush window(RGB(50, 50, 50)); // color is black
	pDC->SelectObject(window); // select brush
	pDC->Rectangle(0, 0, 825, 825);  // draw background. color is black.
	int xcoor = 0;
	int ycoor = 0;
	for (int i = 0; i < 25 * 25; i++) // draw 625 square.
	{
		xcoor = (i * 25) % 625; // xcoor is row
		ycoor = (i / 25) * 25; // ycoor is column
		CBrush brush_snake_head1(RGB(150, 150, 150)); // color is gray
		pDC->SelectObject(brush_snake_head1);  // select brush
		pDC->Rectangle(85 + xcoor, 75 + ycoor, 110 + xcoor, 100 + ycoor); // 85,75 is right,top of coordinates.
	}

}
void CProject1Dlg::OnPaint()
{
	srand((unsigned)time(NULL));
	snakeeat = ((rand() % 625) + 1); // get random number to make food.
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);

	}
	else
	{

		CPaintDC dc(this);
		CDC *pDC = GetDC();
		CRect rect;
		GetClientRect(&rect); // make window

		CDC MemDC;
		CBitmap* pOldBitmap;
		CBitmap bmp; // to use doublebuffering, set variable.

		MemDC.CreateCompatibleDC(pDC); // create pointer.
		bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
		pOldBitmap = ((CBitmap*)MemDC.SelectObject(&bmp));
		MemDC.PatBlt(0, 0, rect.Width(), rect.Height(), WHITENESS);
		DrawImage(&MemDC); // it is customed function. draw square.
		pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY); // draw square.
		MemDC.SelectObject(pOldBitmap); 
		MemDC.DeleteDC(); // delete buffer.
		


		Node *pNode,*pHead; // node variable set.
		pNode = list.GetNode(); // pnode is head of list.
		if (pNode == NULL) // pNode is NULL?? fist loop. set three node, head, body, tail.
		{
			Node *node = new Node;
			node->Setrow(385); // it is headnode.
			node->Setcolumn(375); 
			list.insertNode(node,0); // type is 0

			node = new Node;
			node->Setrow(385 - 25); // it is bodynode.
			node->Setcolumn(375);
			list.insertNode(node,0); // type is 0

			node = new Node;
			node->Setrow(385 - 50); // it is tailnode.
			node->Setcolumn(375);
			list.insertNode(node,0); // type is 0.

			pNode = list.GetNode(); // node set to head.

			CBrush brush_snake_head(RGB(189, 255, 18)); // headnode color, color is green
			dc.SelectObject(brush_snake_head);

			dc.Ellipse(pNode->Getrow() - 12.5 + 12.5, pNode->Getcolumn() - 12.5 + 12.5, pNode->Getrow() + 12.5 + 12.5, pNode->Getcolumn() + 12.5 + 12.5); // draw headnode
			pNode = pNode->GetNextNode();


			CBrush brush_snake_head1(RGB(255, 255, 72)); // bodynode, color is yellow.
			dc.SelectObject(brush_snake_head1);
			dc.Rectangle(pNode->Getrow(), pNode->Getcolumn(), pNode->Getrow() + 25, pNode->Getcolumn() + 25); // draw bodynode
			pNode = pNode->GetNextNode();

			CBrush brush_snake_head2(RGB(128, 245, 255)); // tailnode, color is blue,
			dc.SelectObject(brush_snake_head2);
			dc.Ellipse(pNode->Getrow() - 12.5 + 12.5, pNode->Getcolumn() - 12.5 + 12.5, pNode->Getrow() + 12.5 + 12.5, pNode->Getcolumn() + 12.5 + 12.5); // draw tailnode.

			CBrush brush_snake_head3(RGB(255, 0, 0)); // food, color is red.
			dc.SelectObject(brush_snake_head3);

			while (eatrow < 335 && eatrow > 385 && eatcolumn != 375) // get random number. is food's coordinates is same is snake's coordinates?? retry and get random number.
			{
				snakeeat = ((rand() % 625) + 1); // get the random number
				eatrow = (((snakeeat - 1) % 25)) * 25 + 85;
				eatcolumn = ((snakeeat - 1) / 25) * 25 + 75; // make coordinates
			}
			eatrow = (((snakeeat - 1) % 25)) * 25 + 85;
			eatcolumn = ((snakeeat - 1) / 25) * 25 + 75;
			dc.Rectangle(eatrow, eatcolumn, eatrow + 25, eatcolumn + 25); // draw food.
			eat = 1; // eat is 1. food maked.

		}
		else
		{

			pNode = list.GetNode(); // get headnode/
			int tempnoderow;
			int tempnodecolmn; // get variable.
			while (pNode->GetNextNode()) // move to tailnode.
				pNode = pNode->GetNextNode();

			CBrush brush_snake_head2(RGB(128, 245, 255)); // tailnode. color is blue.
			dc.SelectObject(brush_snake_head2); // select brush
			pNode->Setrow(pNode->GetPrevNode()->Getrow());// get the coordinate of prev node. snake move.
			pNode->Setcolumn(pNode->GetPrevNode()->Getcolumn());
			dc.Ellipse(pNode->Getrow() - 12.5+12.5, pNode->Getcolumn() - 12.5+ 12.5, pNode->Getrow() + 12.5+ 12.5, pNode->Getcolumn() + 12.5+ 12.5); // tail draw.


			CBrush brush_snake_head1(RGB(255, 255, 72)); // body. color is yellow.
			dc.SelectObject(brush_snake_head1); // select brush
			while (pNode->GetPrevNode()->GetPrevNode()) // move to head->nextnode
			{
				pNode = pNode->GetPrevNode(); // move node
				pNode->Setrow(pNode->GetPrevNode()->Getrow()); // change node's coordinates.
				pNode->Setcolumn(pNode->GetPrevNode()->Getcolumn());
				dc.Rectangle(pNode->Getrow(), pNode->Getcolumn(), pNode->Getrow() + 25, pNode->Getcolumn() + 25);  // draw body
			}


			CBrush brush_snake_head(RGB(189, 255, 18)); // head. color is green.
			dc.SelectObject(brush_snake_head); // select brush.

			pNode = pNode->GetPrevNode(); // headnode.
			pNode->Setrow(m_right); // move node, snake move.
			pNode->Setcolumn(m_up);
			int eattempcount = 0;
			dc.Ellipse(pNode->Getrow() - 12.5 + 12.5, pNode->Getcolumn() - 12.5 + 12.5, pNode->Getrow() + 12.5 + 12.5, pNode->Getcolumn() + 12.5 + 12.5);// draw headnode

			if (m_right == eatrow && m_up == eatcolumn) // if snake eat food??
			{
				eat = 0; // make food.
		
				Node *node = new Node;
				node->Setrow(pNode->Getrow());
				node->Setcolumn(pNode->Getcolumn()); // get the coordinates of headnode. 
				list.insertNode(node,1); // insert node. type is 1.


			}
			if (eat == 0) // eat 0?? make food.
			{
				int samecount = 0;
				pNode = list.GetNode(); // get headnode

				CBrush brush_snake_head3(RGB(255, 0, 0)); // color is red
				dc.SelectObject(brush_snake_head3); // select brush
				while (samecount != 1) // if food's coordinates is same as snake's coordinates
				{
					samecount = 1; // set samecount = 1
					while (pNode->GetNextNode()) // move node
					{
						if ((pNode->Getcolumn() == eatcolumn) && (pNode->Getrow() == eatrow)) // compare coordinates
						{
							samecount--;
							break;
						}
						pNode = pNode->GetNextNode(); // move node
					}
					if (samecount == 0) // if count is same??
					{
						snakeeat = ((rand() % 625) + 1);
						eatrow = (((snakeeat - 1) % 25)) * 25 + 85;
						eatcolumn = ((snakeeat - 1) / 25) * 25 + 75;
					}
				}
				dc.Rectangle(eatrow, eatcolumn, eatrow + 25, eatcolumn + 25); // draw food
				eat=1; // eat 1. do not make food again
				eattempcount++; // 
			}
			else
			{
				CBrush brush_snake_head3(RGB(255, 0, 0)); // else?? food already exist.
				dc.SelectObject(brush_snake_head3); // select brush
				dc.Rectangle(eatrow, eatcolumn, eatrow + 25, eatcolumn + 25); // draw food
			}

			pNode = list.GetNode(); // get node
			pHead = list.GetNode(); // get head
			int count = 0;
			while (pNode->GetNextNode()) // move node
			{
				pNode = pNode->GetNextNode(); // move next node
				if ((pNode->Getcolumn() == pHead->Getcolumn()) && (pNode->Getrow() == pHead->Getrow())&& eattempcount != 1) // if head coordinates is same as body or head coordinates??
				{
					KillTimer(0); // stop timer
					MessageBox(L"몸에 쿵!", L"실패", MB_ICONHAND); // print messagebox
					::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
				}

			}
			count = 0;
			eattempcount = 0;
			

		}



		
	}
}


// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CProject1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


static int setflag = 3; // setflag. 3 is right.
void CProject1Dlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nChar)
	{
		case 'w':
		case 'W' :
			setflag = 0; // 0 is up.
			break;

		case 'A':
		case 'a':
			setflag = 1; // 1 is left
			break;

		case 'S':
		case 's':
			setflag = 2; // 2 is down
			break;

		case 'D':
		case 'd':
			setflag = 3 ; // 4 is left
			break;
	}
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags); // make event
}


void CProject1Dlg::OnTimer(UINT_PTR nIDEvent) // timer event.
{
	Node *pNode;
	pNode = list.GetNode(); // set node


	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent) // event
	{
	case 0:
		if (setflag == 0 && !(pNode->Getcolumn() - 25 == pNode->GetNextNode()->Getcolumn())) // flag is w, and move direction is not down??
			m_up -= 25;
		else if(setflag == 0) // mode direction is down??? keep goint. do not change direction.
			m_up += 25;

		if (setflag == 1 && !(pNode->Getrow() - 25 == pNode->GetNextNode()->Getrow()) ) //flag is a, and move direction is not right??
			m_right -= 25;
		else if (setflag == 1)// mode direction is down??? keep goint. do not change direction.
			m_right += 25;

		if (setflag == 2 && !(pNode->Getcolumn() + 25 == pNode->GetNextNode()->Getcolumn()))// flag is s, and move direction is not up??
			m_up += 25;
		else if (setflag == 2)// mode direction is down??? keep goint. do not change direction.
			m_up -= 25;
		if (setflag == 3 && !(pNode->Getrow() + 25 == pNode->GetNextNode()->Getrow())) // flag is d, and move direction is not left??
			m_right += 25;
		else if (setflag == 3)// mode direction is down??? keep goint. do not change direction.
			m_right -= 25;



		if (m_right == 300 + 410 || m_right == -350 + 410 || m_up == 300 + 400 || m_up == -300 + 350) // if head coordinates is same as wall??
		{
			KillTimer(0); // stop timer
			MessageBox(L"벽에 쿵!", L"실패", MB_ICONHAND); // show messagebox
			::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
			//SetTimer(0, 500, nullptr);

		}

		Invalidate(FALSE); // delete and redraw table.
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}
