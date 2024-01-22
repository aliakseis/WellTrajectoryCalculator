
// WellTrajectoryCalculatorDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "WellTrajectoryCalculator.h"
#include "WellTrajectoryCalculatorDlg.h"
#include "afxdialogex.h"

#include "Trajview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



namespace {

void EditLock(CWnd* pEdit, BOOL flLock)
{
	enum { CLASS_SIZE = 100 };

	CString WindowClass;

	ASSERT (::IsWindow(pEdit->m_hWnd));
	
	if (!::IsWindow(pEdit->m_hWnd))
		return;

	GetClassName(pEdit->m_hWnd, WindowClass.GetBufferSetLength(CLASS_SIZE), CLASS_SIZE);
	WindowClass.ReleaseBuffer();

	//BOOL Result = FALSE;
	//if (WindowClass == CLASS_FPDATETIME)
	//{
	//	DeclareEditAsFPDateTime(Edt);
	//	LockControl(pFPEdit, flLock, coloration);
	//}
	//else 
	//	if (WindowClass == CLASS_FPDOUBLESINGLE)
	//{
	//	DeclareEditAsFPDoubleSingle(Edt);
	//	LockControl(pFPEdit, flLock, coloration);
	//}
	//else if (WindowClass == CLASS_FPLONGINTEGER)
	//{
	//	DeclareEditAsFPLongInteger(Edt);
	//	LockControl(pFPEdit, flLock, coloration);
	//}
	//else if (WindowClass == CLASS_FPTEXT)
	//{
	//	DeclareEditAsFPText(Edt);
	//	LockControl(pFPEdit, flLock, coloration);
	//}
	//else 
        if (WindowClass == "Edit")
	{
		static_cast<CEdit*>(pEdit)->SetReadOnly(flLock);
	}
	else 
	{
		pEdit->EnableWindow(!flLock);
	}
}

BOOL EnableEdit(CWnd* Ctrl, BOOL flEnable)
{
	if (Ctrl) 
		EditLock(Ctrl, !flEnable);
	return TRUE;
}

void ShowControl(CWnd* pWnd, BOOL Show)
{
	if (!pWnd)
		ASSERT(FALSE);
	else
	{
		int nCmdShow = Show ? SW_SHOWNA : SW_HIDE;
		pWnd->ShowWindow(nCmdShow);
	}
}

}

const auto Deg2Rad = (float)(M_PI / 180.0);
const auto Rad2Deg = (float)(180.0 / M_PI);



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CWellTrajectoryCalculatorDlg dialog



CCalcDlg::CCalcDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_2DDIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_fL1 = (float)100.0;
	m_fPhi1 = (float)10.0;
	m_fBR1 = (float)40.0;

	m_fL2 = (float)100.0;
	m_fPhi2 = (float)50.0;
	m_fBR2 = (float)-50.0;

	m_fL3 = (float)100.0;
	m_fPhi3 = (float)0.0;

	m_fTVD = (float)435.4;
	m_fDisp = (float)183.89;

	m_nMode = 0;
	m_bValidTrajectory = FALSE;
	m_bIsotropic = FALSE;
	m_nTrajectoryType = tkExtendedSTrajectory;

    m_pView = std::make_unique<CTrajView>(*this);
}

CCalcDlg::~CCalcDlg() = default;

void CCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CCalcDlg)
	DDX_Control(pDX, IDC_Sensitivity, m_Sensitivity);
	DDX_Text(pDX, IDC_TVD, m_fTVD);
	DDX_Text(pDX, IDC_Disp, m_fDisp);

	DDX_Text_Ex(pDX, IDC_EDIT2, m_fL1, F_L1);
	DDX_Text_Ex(pDX, IDC_EDIT1, m_fPhi1, F_PHI1);
	DDX_Text_Ex(pDX, IDC_EDIT3, m_fBR1, F_R1);
	DDX_Text_Ex(pDX, IDC_EDIT5, m_fL2, F_L2);
	DDX_Text_Ex(pDX, IDC_EDIT4, m_fPhi2, F_PHI2);
	DDX_Text_Ex(pDX, IDC_EDIT6, m_fBR2, F_R2);
	DDX_Text_Ex(pDX, IDC_EDIT8, m_fL3, F_L3);
	DDX_Text_Ex(pDX, IDC_EDIT7, m_fPhi3, F_PHI3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCalcDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	ON_BN_CLICKED(IDC_CHECK4, OnCheck4)
	ON_BN_CLICKED(IDC_CHECK5, OnCheck5)
	ON_BN_CLICKED(IDC_CHECK6, OnCheck6)
	ON_BN_CLICKED(IDC_CHECK7, OnCheck7)
	ON_BN_CLICKED(IDC_CHECK8, OnCheck8)
	ON_BN_CLICKED(IDC_Apply, OnApply)
	ON_BN_CLICKED(IDC_Isotropic, OnIsotropic)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON8, OnButton8)
	ON_COMMAND(IDM_J_TRAJ, OnJTraj)
	ON_COMMAND(IDM_EXT_J_TRAJ, OnExtendedJTraj)
	ON_COMMAND(IDM_S_TRAJ, OnSTraj)
	ON_COMMAND(IDM_EXT_S_TRAJ, OnExtendedSTraj)
	ON_UPDATE_COMMAND_UI(IDM_J_TRAJ, OnUpdateJTraj)
	ON_UPDATE_COMMAND_UI(IDM_EXT_J_TRAJ, OnUpdateExtendedJTraj)
	ON_UPDATE_COMMAND_UI(IDM_S_TRAJ, OnUpdateSTraj)
	ON_UPDATE_COMMAND_UI(IDM_EXT_S_TRAJ, OnUpdateExtendedSTraj)
END_MESSAGE_MAP()


// CWellTrajectoryCalculatorDlg message handlers

BOOL CCalcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Add extra initialization here
	m_Sensitivity.SetRange(0, 20, FALSE);
	m_Sensitivity.SetPos(10);
	m_Sensitivity.SetLineSize(1);
	m_Sensitivity.SetPageSize(10);
	m_Sensitivity.SetPos(10);
	if (m_bFixAngle)
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK1)->EnableWindow(FALSE);
	}
	for (int i = IDC_SPIN1; i <= IDC_SPIN8; i++)
		((CSpinButtonCtrl*)GetDlgItem(i))->SetRange(-1, 1);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCalcDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCalcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCalcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//////////////////////////////////////////////////////////////////////////////

void CCalcDlg::OnOK()
{
}


#define MaxLen  100000.f
#define MaxRate 100.f


/*
void CCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseTrajDesDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalcDlg)
	DDX_Control(pDX, IDC_Sensitivity, m_Sensitivity);
	//}}AFX_DATA_MAP
	DDX_FPValue_(pDX, IDC_TVD, m_fTVD, UV_4_TRAJ_LENGTH, IDC_TVDUnits);
	DDX_FPValue_(pDX, IDC_Disp, m_fDisp, UV_4_TRAJ_LENGTH, IDC_DispUnits);

	DDX_Text_Ex(pDX, IDC_EDIT2, m_fL1, F_L1, UV_4_TRAJ_LENGTH, IDC_EDIT2Units);
	DDX_Text_Ex(pDX, IDC_EDIT1, m_fPhi1, F_PHI1, UV_4_TRAJ_ANGLE, AngleUnitStaticId(IDC_EDIT1Units));
	DDX_Text_Ex(pDX, IDC_EDIT3, m_fBR1, F_R1, UV_4_TRAJ_BUILDRATE, IDC_EDIT3Units);
	DDX_Text_Ex(pDX, IDC_EDIT5, m_fL2, F_L2, UV_4_TRAJ_LENGTH, IDC_EDIT5Units);
	DDX_Text_Ex(pDX, IDC_EDIT4, m_fPhi2, F_PHI2, UV_4_TRAJ_ANGLE, AngleUnitStaticId(IDC_EDIT4Units));
	DDX_Text_Ex(pDX, IDC_EDIT6, m_fBR2, F_R2, UV_4_TRAJ_BUILDRATE, IDC_EDIT6Units);
	DDX_Text_Ex(pDX, IDC_EDIT8, m_fL3, F_L3, UV_4_TRAJ_LENGTH, IDC_EDIT8Units);
	DDX_Text_Ex(pDX, IDC_EDIT7, m_fPhi3, F_PHI3, UV_4_TRAJ_ANGLE, AngleUnitStaticId(IDC_EDIT7Units));
}
*/

int NBits(int nBits)
{
	int nCount = 0;
	for (int i = 1; i <= 0x80; i <<= 1) {
		if (nBits & i) nCount++;
	}
	return nCount;
}

void CCalcDlg::SetReadOnly(int nID, BOOL bReadOnly)
{
	EnableEdit(GetDlgItem(nID), !bReadOnly);
}

void CCalcDlg::EnableButton(int nButtonID, int nSpinID, BOOL bEnable, BOOL bVisible)
{
	CWnd* pButton = GetDlgItem(nButtonID);
	CWnd* pSpin = GetDlgItem(nSpinID);
	ASSERT(pButton && pSpin);
	if (m_bValidTrajectory)
	{
		pButton->ShowWindow(SW_HIDE);
		if (bVisible)
		{
			pSpin->ShowWindow(SW_SHOW);
			pSpin->EnableWindow(bEnable);
		}
		else
			pSpin->ShowWindow(SW_HIDE);
	}
	else
	{
		pSpin->ShowWindow(SW_HIDE);
		if (bVisible)
		{
			pButton->ShowWindow(SW_SHOW);
			pButton->EnableWindow(bEnable);
		}
		else
			pButton->ShowWindow(SW_HIDE);
	}
}

void CCalcDlg::EnableButtons(BOOL bEnable)
{
	EnableButton(IDC_BUTTON1, IDC_SPIN1,
		bEnable && !(m_nMode & F_PHI1) && !m_bFixAngle, TRUE);
	EnableButton(IDC_BUTTON2, IDC_SPIN2,
		bEnable && !(m_nMode & F_L1), TRUE);
	EnableButton(IDC_BUTTON3, IDC_SPIN3,
		bEnable && !(m_nMode & F_R1), TRUE);

	EnableButton(IDC_BUTTON4, IDC_SPIN4,
		bEnable && !(m_nMode & F_PHI2), TRUE);
	EnableButton(IDC_BUTTON5, IDC_SPIN5,
		bEnable && !(m_nMode & F_L2), m_nTrajectoryType > tkJTrajectory);
	EnableButton(IDC_BUTTON6, IDC_SPIN6,
		bEnable && !(m_nMode & F_R2), m_nTrajectoryType > tkExtendedJTrajectory);

	EnableButton(IDC_BUTTON7, IDC_SPIN7,
		bEnable && !(m_nMode & F_PHI3), m_nTrajectoryType > tkExtendedJTrajectory);
	EnableButton(IDC_BUTTON8, IDC_SPIN8,
		bEnable && !(m_nMode & F_L3), m_nTrajectoryType > tkSTrajectory);

	GetDlgItem(IDC_Apply)->EnableWindow(bEnable);

	SetReadOnly(IDC_EDIT1, /*bEnable &&*/ (m_nMode & F_PHI1) || m_bFixAngle);
	SetReadOnly(IDC_EDIT2, /*bEnable &&*/ (m_nMode & F_L1));
	SetReadOnly(IDC_EDIT3, /*bEnable &&*/ (m_nMode & F_R1));

	SetReadOnly(IDC_EDIT4, /*bEnable &&*/ (m_nMode & F_PHI2));
	SetReadOnly(IDC_EDIT5, /*bEnable &&*/ (m_nMode & F_L2));
	SetReadOnly(IDC_EDIT6, /*bEnable &&*/ (m_nMode & F_R2));

	SetReadOnly(IDC_EDIT7, /*bEnable &&*/ (m_nMode & F_PHI3));
	SetReadOnly(IDC_EDIT8, /*bEnable &&*/ (m_nMode & F_L3));
}

void CCalcDlg::SetMode(int nFlag, BOOL bEnable)
{
	UpdateData(TRUE);
	if (bEnable) {
		m_nMode |= nFlag;
		int nCount = NBits(m_nMode);
		if (nCount > 2) {
			m_nMode = nFlag;
			if (nFlag != F_PHI1)  CheckDlgButton(IDC_CHECK1, FALSE);
			if (nFlag != F_L1) 	CheckDlgButton(IDC_CHECK2, FALSE);
			if (nFlag != F_R1)		CheckDlgButton(IDC_CHECK3, FALSE);
			if (nFlag != F_PHI2)  CheckDlgButton(IDC_CHECK4, FALSE);
			if (nFlag != F_L2) 	CheckDlgButton(IDC_CHECK5, FALSE);
			if (nFlag != F_R2)   	CheckDlgButton(IDC_CHECK6, FALSE);
			if (nFlag != F_PHI3)	CheckDlgButton(IDC_CHECK7, FALSE);
			if (nFlag != F_L3) 	CheckDlgButton(IDC_CHECK8, FALSE);
		}
	}
	else m_nMode &= ~nFlag;
	EnableButtons(NBits(m_nMode) == 2);
	if (!m_bValidTrajectory)
		UpdateData(FALSE);
}




/////////////////////////////////////////////////////////////////////////////
// CCalcDlg message handlers

void CCalcDlg::OnCheck1()
{
	SetMode(F_PHI1, IsDlgButtonChecked(IDC_CHECK1));
}

void CCalcDlg::OnCheck2()
{
	SetMode(F_L1, IsDlgButtonChecked(IDC_CHECK2));
}

void CCalcDlg::OnCheck3()
{
	SetMode(F_R1, IsDlgButtonChecked(IDC_CHECK3));
}

void CCalcDlg::OnCheck4()
{
	SetMode(F_PHI2, IsDlgButtonChecked(IDC_CHECK4));
}

void CCalcDlg::OnCheck5()
{
	SetMode(F_L2, IsDlgButtonChecked(IDC_CHECK5));
}

void CCalcDlg::OnCheck6()
{
	SetMode(F_R2, IsDlgButtonChecked(IDC_CHECK6));
}

void CCalcDlg::OnCheck7()
{
	SetMode(F_PHI3, IsDlgButtonChecked(IDC_CHECK7));
}

void CCalcDlg::OnCheck8()
{
	SetMode(F_L3, IsDlgButtonChecked(IDC_CHECK8));
}


BOOL CheckData(PLANE_TRAJ_ARR c)
{
	if (c[0].L < 0 || c[1].L < 0 || c[2].L < 0 ||
		c[0].L > MaxLen || c[1].L > MaxLen || c[2].L > MaxLen) return FALSE;
	for (int i = 0; i < 2; i++) {
		float fAngle = c[i + 1].Phi - c[i].Phi;
		while (fAngle > M_PI) fAngle -= float(M_PI * 2);
		while (fAngle < -M_PI) fAngle += float(M_PI * 2);
		if (fAngle * c[i].R < 0) return FALSE;
	}
	return TRUE;
}


void CCalcDlg::Store(PLANE_TRAJ_ARR c)
{
	c[0].L = m_fL1;
	c[0].Phi = m_fPhi1 * Deg2Rad;

	if (m_bValidTrajectory || !(m_nMode & F_R1))
		if (m_fBR1 == 0.) c[0].R = 0.f;//return FALSE;
		else c[0].R = Rad2Deg * 100 / m_fBR1;
	else c[0].R = 0.;
	c[1].L = m_fL2;
	c[1].Phi = m_fPhi2 * Deg2Rad;
	if (m_bValidTrajectory || !(m_nMode & F_R2))
		if (m_fBR2 == 0.) c[1].R = 0.f;//return FALSE;
		else c[1].R = Rad2Deg * 100 / m_fBR2;
	else c[1].R = 0.;
	c[2].L = m_fL3;
	c[2].Phi = m_fPhi3 * Deg2Rad;
	c[2].R = 0.f;
}

BOOL CCalcDlg::Load(const PLANE_TRAJ_ARR c)
{
	float fBR1, fBR2;
	if (c[0].R != 0.) fBR1 = Rad2Deg * 100 / c[0].R;
	else fBR1 = 0.f;
	if (c[1].R != 0.) fBR2 = Rad2Deg * 100 / c[1].R;
	else fBR2 = 0.f;
	if (fabs(fBR1) > MaxRate || fabs(fBR2) > MaxRate) return FALSE;

	m_fL1 = c[0].L;
	m_fPhi1 = c[0].Phi * Rad2Deg;
	m_fBR1 = fBR1;
	m_fL2 = c[1].L;
	m_fPhi2 = c[1].Phi * Rad2Deg;
	m_fBR2 = fBR2;
	m_fL3 = c[2].L;
	m_fPhi3 = c[2].Phi * Rad2Deg;
	return TRUE;
}

bool CCalcDlg::TryToApply()
{
	PLANE_TRAJ_ARR c;
	if (m_fL1 < 0 || m_fL1 > MaxLen ||
		fabs(m_fPhi1) > 180 || fabs(m_fBR1) > MaxRate ||
		m_fL2 < 0 || m_fL2 > MaxLen ||
		fabs(m_fPhi2) > 180 || fabs(m_fBR2) > MaxRate ||
		m_fL2 < 0 || m_fL2 > MaxLen ||
		fabs(m_fPhi2) > 180) return FALSE;
	/*
	   c[0].L = m_fL1;
	   c[0].Phi = m_fPhi1 * Deg2Rad;

	   if(!(m_nMode & F_R1))
		  if(m_fBR1 == 0.) c[0].R = 0.f;//return FALSE;
		else c[0].R = Rad2Deg * 100 / m_fBR1;
	   c[1].L = m_fL2;
	   c[1].Phi = m_fPhi2 * Deg2Rad;
	   if(!(m_nMode & F_R2))
		  if(m_fBR2 == 0.) c[1].R = 0.f;//return FALSE;
		   else c[1].R = Rad2Deg * 100 / m_fBR2;
	   c[2].L = m_fL3;
	   c[2].Phi = m_fPhi3 * Deg2Rad;
	   c[2].R = 0.f;
	*/
	Store(c);
	CPlaneTrajMath Math;
	Math.SetData(c, m_fTVD, m_fDisp);

	BOOL bCorrect = FALSE;
	switch (m_nMode) {
	case F_L1 | F_R1:
		bCorrect = Math.CalcLwhereR(0); break;
	case F_L2 | F_R2:
		bCorrect = Math.CalcLwhereR(M_ROT_UP); break;
	case F_L2 | F_R1:
		bCorrect = Math.CalcLwhereR(M_TURN | M_ROT_UP); break;
	case F_L3 | F_R2:
		bCorrect = Math.CalcLwhereR(M_TURN); break;

	case F_L1 | F_R2:
		bCorrect = Math.CalcLltR(0); break;
	case F_L3 | F_R1:
		bCorrect = Math.CalcLltR(M_TURN); break;

	case F_L1 | F_L2:
		bCorrect = Math.CalcLL(0); break;
	case F_L1 | F_L3:
		bCorrect = Math.CalcLL(M_ROT_DOWN); break;
	case F_L2 | F_L3:
		bCorrect = Math.CalcLL(M_ROT_UP); break;

	case F_R1 | F_R2:
		bCorrect = Math.CalcRR(); break;

	case F_L1 | F_PHI1:
		bCorrect = Math.CalcLwherePhi(0); break;
	case F_L2 | F_PHI2:
		bCorrect = Math.CalcLwherePhi(M_ROT_UP); break;
	case F_L3 | F_PHI3:
		bCorrect = Math.CalcLwherePhi(M_ROT_DOWN); break;

	case F_L1 | F_PHI2:
		bCorrect = Math.CalcLltPhi(0); break;
	case F_L1 | F_PHI3:
		bCorrect = Math.CalcLltPhi(M_TURN | M_ROT_DOWN); break;
	case F_L2 | F_PHI1:
		bCorrect = Math.CalcLltPhi(M_TURN | M_ROT_UP); break;
	case F_L2 | F_PHI3:
		bCorrect = Math.CalcLltPhi(M_ROT_UP); break;
	case F_L3 | F_PHI1:
		bCorrect = Math.CalcLltPhi(M_ROT_DOWN); break;
	case F_L3 | F_PHI2:
		bCorrect = Math.CalcLltPhi(M_TURN); break;

	case F_R1 | F_PHI1:
		bCorrect = Math.CalcRwherePhi(0); break;
	case F_R1 | F_PHI2:
		bCorrect = Math.CalcRwherePhi(M_TURN | M_ROT_UP); break;
	case F_R2 | F_PHI2:
		bCorrect = Math.CalcRwherePhi(M_ROT_UP); break;
	case F_R2 | F_PHI3:
		bCorrect = Math.CalcRwherePhi(M_TURN); break;

	case F_R2 | F_PHI1:
		bCorrect = Math.CalcRgtPhi(0); break;
	case F_R1 | F_PHI3:
		bCorrect = Math.CalcRgtPhi(M_TURN); break;

	case F_PHI1 | F_PHI2:
		bCorrect = Math.CalcPhiPhi(0); break;
	case F_PHI1 | F_PHI3:
		bCorrect = Math.CalcPhiPhi(M_ROT_DOWN); break;
	case F_PHI2 | F_PHI3:
		bCorrect = Math.CalcPhiPhi(M_ROT_UP); break;
	}
	if (!bCorrect || !CheckData(c) || !Load(c))
		return FALSE;
	/*
		float fBR1,fBR2;
		if(c[0].R != 0.) fBR1 = Rad2Deg * 100 / c[0].R;
		else fBR1 = 0.f;
		if(c[1].R != 0.) fBR2 = Rad2Deg * 100 / c[1].R;
		else fBR2 = 0.f;
		if(fabs(fBR1) > MaxRate || fabs(fBR2) > MaxRate) return FALSE;

	   m_fL1 = c[0].L;
	   m_fPhi1 = c[0].Phi * Rad2Deg;
	   m_fBR1 = fBR1;
	   m_fL2 = c[1].L;
	   m_fPhi2 = c[1].Phi * Rad2Deg;
	   m_fBR2 = fBR2;
	   m_fL3 = c[2].L;
	   m_fPhi3 = c[2].Phi * Rad2Deg;
	*/
	memcpy(m_c, c, sizeof(PLANE_TRAJ_ARR));
	SetValidTrajectory(TRUE);
	//	UpdateData(FALSE);
	//	GetDlgItem(IDOK)->EnableWindow(TRUE);
	//	m_bValidTrajectory = TRUE;
	//	CWnd* pParent = GetParent();
	//	if(pParent) pParent->InvalidateRect(&UpdateRect);
	EnableButtons(TRUE);
	return TRUE;
}

void CCalcDlg::OnApply()
{
	// TODO: Add your control notification handler code here
	if (!UpdateData(TRUE)) return;
	/*
	   if(!(m_nMode & F_R1) && m_fBR1 == 0 ||
		  !(m_nMode & F_R2) && m_fBR2 == 0) {
		  MessageBox("Zero input."); return;
	   }
	*/
	if (!TryToApply())
	{
		SetValidTrajectory(FALSE);
		MessageBox(_T("Invalid data set."));
	}
	EnableButtons(NBits(m_nMode) == 2);
}


void CCalcDlg::OnIsotropic()
{
	m_bIsotropic = IsDlgButtonChecked(IDC_Isotropic);
	m_pView->InvalidateRect(NULL);//&UpdateRect);
}

BOOL CCalcDlg::MakeChoice(int nFlag, float* fVal, float fBound, int nCount, BOOL bExcludeZero)
{
	if (2 != NBits(m_nMode) || m_nMode & nFlag || !UpdateData(TRUE)) return FALSE;
	if (TryToApply()) return TRUE;
	float fStartVal = *fVal;
	fBound /= nCount;
	BOOL bFind = FALSE;

	SetCursor(LoadCursor(NULL, IDC_WAIT));
	for (int i = 1; i <= nCount; i++) {
		*fVal = fStartVal - fBound * i;
		if (!(bExcludeZero && 0 == (*fVal)) && TryToApply()) { bFind = TRUE; break; }
		*fVal = fStartVal + fBound * i;
		if (!(bExcludeZero && 0 == (*fVal)) && TryToApply()) { bFind = TRUE; break; }
	}
	SetCursor(LoadCursor(NULL, IDC_ARROW));

	if (!bFind)
	{
		*fVal = fStartVal;
		SetValidTrajectory(FALSE);
		MessageBox(_T("Please try another parameter."));
	}
	return bFind;
}

//inline float fmax(double a, double b) {return(float)( (a > b)? a : b ); }

void CCalcDlg::OnButton1()
{
	if (!MakeChoice(F_PHI1, &m_fPhi1, (float)360., 2000, FALSE))
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
}

void CCalcDlg::OnButton2()
{
	if (!MakeChoice(F_L1, &m_fL1, fmax(fabs(m_fTVD), fabs(m_fDisp)) * 2, 2000, FALSE))
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
}

void CCalcDlg::OnButton3()
{
	if (!MakeChoice(F_R1, &m_fBR1, (float)MaxRate * 2, 2000, TRUE))
		GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
}

void CCalcDlg::OnButton4()
{
	if (!MakeChoice(F_PHI2, &m_fPhi2, (float)360., 2000, FALSE))
		GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
}

void CCalcDlg::OnButton5()
{
	if (!MakeChoice(F_L2, &m_fL2, fmax(fabs(m_fTVD), fabs(m_fDisp)) * 2, 2000, FALSE))
		GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
}

void CCalcDlg::OnButton6()
{
	if (!MakeChoice(F_R2, &m_fBR2, MaxRate * 2, 2000, TRUE))
		GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);
}

void CCalcDlg::OnButton7()
{
	if (!MakeChoice(F_PHI3, &m_fPhi3, (float)360., 2000, FALSE))
		GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE);
}

void CCalcDlg::OnButton8()
{
	if (!MakeChoice(F_L3, &m_fL3, fmax(fabs(m_fTVD), fabs(m_fDisp)) * 2, 2000, FALSE))
		GetDlgItem(IDC_BUTTON8)->EnableWindow(FALSE);
}


void CCalcDlg::OnCancel()
{
	//Return(FALSE);
}

int  CCalcDlg::GetEditFlags()
{
	int nFlags;
	switch (m_nTrajectoryType)
	{
	case tkJTrajectory:
		nFlags = F_L1 | F_R1 | F_PHI2;
		break;
	case tkExtendedJTrajectory:
		nFlags = F_L1 | F_R1 | F_L2 | F_PHI2;
		break;
	case tkSTrajectory:
		nFlags = F_L1 | F_R1 | F_L2
			| F_PHI2 | F_R2 | F_PHI3;
		break;
	case tkExtendedSTrajectory:
		nFlags = F_L1 | F_R1 | F_L2
			| F_PHI2 | F_R2 | F_L3 | F_PHI3;
		break;
	default:
		ASSERT(0);
		return 0;
	}
	if (!m_bFixAngle)
		nFlags |= F_PHI1;
	ASSERT((nFlags & m_nMode) == m_nMode);
	return nFlags ^ m_nMode;
}

void CCalcDlg::SetValidTrajectory(BOOL bValid)
{
	GetDlgItem(IDOK)->EnableWindow(bValid);
	GetDlgItem(IDC_ADD_TO_CLIPBOOK)->EnableWindow(bValid);
	m_bValidTrajectory = bValid;
	UpdateData(FALSE);
	m_pView->InvalidateRect(NULL);//&UpdateRect);
}

//void CCalcDlg::Restore(int nIndex)
//{
//	if (nIndex < 0 || nIndex >= m_arrClipbook.GetSize())
//		return;
//	if (Load(m_arrClipbook[nIndex].m_arr))
//	{
//		m_fTVD = m_arrClipbook[nIndex].m_fTVD;
//		m_fDisp = m_arrClipbook[nIndex].m_fDisp;
//		memcpy(m_c, m_arrClipbook[nIndex].m_arr, sizeof(PLANE_TRAJ_ARR));
//		SetValidTrajectory(TRUE);
//	}
//}

void CCalcDlg::SetTrajectoryType(int nTrajectoryType)
{
	UpdateData(TRUE);

	BOOL bInvalidate = nTrajectoryType < m_nTrajectoryType;
	if ((tkJTrajectory == nTrajectoryType || tkExtendedJTrajectory == nTrajectoryType)
		&& (tkSTrajectory == m_nTrajectoryType || tkExtendedSTrajectory == m_nTrajectoryType))
		m_fPhi2 = m_fHitAngle;
	m_nTrajectoryType = nTrajectoryType;
	switch (m_nTrajectoryType)
	{
		// No breaks
	case tkJTrajectory:
		m_fL2 = 0.f;
	case tkExtendedJTrajectory:
		m_fBR2 = 0.f;
	case tkSTrajectory:
		m_fL3 = 0.f;
	case tkExtendedSTrajectory:
		break;
	default: ASSERT(0);
	}

	switch (m_nTrajectoryType)
	{
		// No breaks
	case tkJTrajectory:
		CheckDlgButton(IDC_CHECK5, FALSE);
		ShowControl(GetDlgItem(IDC_EDIT5), FALSE);
		ShowControl(GetDlgItem(IDC_CHECK5), FALSE);
		//			ShowControl(GetDlgItem(IDC_BUTTON5), FALSE);
		m_nMode &= ~F_L2;
	case tkExtendedJTrajectory:
		CheckDlgButton(IDC_CHECK6, FALSE);
		CheckDlgButton(IDC_CHECK7, FALSE);
		ShowControl(GetDlgItem(IDC_EDIT6), FALSE);
		ShowControl(GetDlgItem(IDC_CHECK6), FALSE);
		//			ShowControl(GetDlgItem(IDC_BUTTON6), FALSE);
		ShowControl(GetDlgItem(IDC_EDIT7), FALSE);
		ShowControl(GetDlgItem(IDC_CHECK7), FALSE);
		//			ShowControl(GetDlgItem(IDC_BUTTON7), FALSE);
		ShowControl(GetDlgItem(IDC_EDIT7Units), FALSE);
		m_nMode &= ~(F_R2 | F_PHI3);
	case tkSTrajectory:
		CheckDlgButton(IDC_CHECK8, FALSE);
		ShowControl(GetDlgItem(IDC_EDIT8), FALSE);
		ShowControl(GetDlgItem(IDC_CHECK8), FALSE);
		//			ShowControl(GetDlgItem(IDC_BUTTON8), FALSE);
		m_nMode &= ~F_L3;
	case tkExtendedSTrajectory:
		break;
	default: ASSERT(0);
	}

	switch (m_nTrajectoryType)
	{
		// No breaks
	case tkExtendedSTrajectory:
		ShowControl(GetDlgItem(IDC_EDIT8), TRUE);
		ShowControl(GetDlgItem(IDC_CHECK8), TRUE);
		//			ShowControl(GetDlgItem(IDC_BUTTON8), TRUE);
	case tkSTrajectory:
		ShowControl(GetDlgItem(IDC_EDIT6), TRUE);
		ShowControl(GetDlgItem(IDC_CHECK6), TRUE);
		//			ShowControl(GetDlgItem(IDC_BUTTON6), TRUE);
		ShowControl(GetDlgItem(IDC_EDIT7), TRUE);
		ShowControl(GetDlgItem(IDC_CHECK7), TRUE);
		//			ShowControl(GetDlgItem(IDC_BUTTON7), TRUE);
		ShowControl(GetDlgItem(IDC_EDIT7Units), TRUE);
	case tkExtendedJTrajectory:
		ShowControl(GetDlgItem(IDC_EDIT5), TRUE);
		ShowControl(GetDlgItem(IDC_CHECK5), TRUE);
		//			ShowControl(GetDlgItem(IDC_BUTTON5), TRUE);
	case tkJTrajectory:
		break;
	default: ASSERT(0);
	}

	if (bInvalidate)
		SetValidTrajectory(FALSE);
	else
		UpdateData(FALSE);// Show/hide labels; SetValidTrajectory() has such a line inside
	EnableButtons(NBits(m_nMode) == 2);
}

/**
void CCalcDlg::OnAddToClipbook()
{
	HINSTANCE hInstOld = AfxGetResourceHandle(); // to load resources from this DLL
	AfxSetResourceHandle(GetTrajUtilDLL()->hModule);

	ASSERT(m_bValidTrajectory);
	CDlgNewClipbookRec dlg;
	int nResult = dlg.DoModal();
	AfxSetResourceHandle(hInstOld); // restore the old resource chain
	if (IDOK != nResult)
		return;

	int nIndex = m_arrClipbook.GetSize();
	m_arrClipbook.SetSize(nIndex + 1);
	Store(m_arrClipbook[nIndex].m_arr);
	m_arrClipbook[nIndex].m_fTVD = m_fTVD;
	m_arrClipbook[nIndex].m_fDisp = m_fDisp;
	m_arrClipbook[nIndex].m_strName = dlg.m_strName;
	GetDlgItem(IDC_CLIPBOOK)->EnableWindow(TRUE);
}
*/

/*
void CCalcDlg::OnVScroll(UINT nSBCode, UINT nPos, CWnd* pScrollBar)
{
//	CBaseTrajDesDialog::OnVScroll(nSBCode, nPos, pScrollBar);
	static bool bRecurs = false;
	if(bRecurs || !pScrollBar || nSBCode != SB_THUMBPOSITION)
		return;
	bRecurs = true;
	ASSERT_VALID(pScrollBar);
	float* pVal;
	switch(pScrollBar->GetDlgCtrlID())
	{
		case IDC_SPIN1: pVal = &m_fPhi1; break;
		case IDC_SPIN2: pVal = &m_fL1;	break;
		case IDC_SPIN3: pVal = &m_fBR1;	break;
		case IDC_SPIN4: pVal = &m_fPhi2; break;
		case IDC_SPIN5: pVal = &m_fL2;	break;
		case IDC_SPIN6: pVal = &m_fBR2;	break;
		case IDC_SPIN7: pVal = &m_fPhi3; break;
		case IDC_SPIN8: pVal = &m_fL3;	break;
		default: ASSERT(0); bRecurs = false; return;
	}
	((CSpinButtonCtrl*)pScrollBar)->SetPos(0);
	float fSaveVal = *pVal;
	*pVal += (signed)nPos;
	if(!TryToApply())
		*pVal = fSaveVal;
	bRecurs = false;
}

static TCHAR szSpinClassName[] = _T("msctls_updown32");
#define SpinClassNameSize (sizeof(szSpinClassName) + sizeof(TCHAR))

BOOL CCalcDlg::PreTranslateMessage(MSG* pMsg)
{
	if(WM_KEYDOWN == pMsg->message && (VK_UP == pMsg->wParam || VK_DOWN == pMsg->wParam))
	{
		HWND hWndNext = ::GetWindow(pMsg->hwnd, GW_HWNDNEXT);
		if(IsWindow(hWndNext))
		{
			CString strClassName;
			GetClassName(hWndNext,
				strClassName.GetBuffer(SpinClassNameSize), SpinClassNameSize);
			if(strClassName == szSpinClassName)
			{
				OnVScroll(SB_THUMBPOSITION, (VK_UP == pMsg->wParam)? 1 : -1,
									CWnd::FromHandle(hWndNext));
				return TRUE;
			}
		}
	}
	return CBaseTrajDesDialog::PreTranslateMessage(pMsg);
}
*/

void CCalcDlg::OnJTraj()
{
	if (tkJTrajectory != GetTrajectoryType())
		SetTrajectoryType(tkJTrajectory);
}

void CCalcDlg::OnExtendedJTraj()
{
	if (tkExtendedJTrajectory != GetTrajectoryType())
		SetTrajectoryType(tkExtendedJTrajectory);
}

void CCalcDlg::OnSTraj()
{
	if (tkSTrajectory != GetTrajectoryType())
		SetTrajectoryType(tkSTrajectory);
}

void CCalcDlg::OnExtendedSTraj()
{
	if (tkExtendedSTrajectory != GetTrajectoryType())
		SetTrajectoryType(tkExtendedSTrajectory);
}


void CCalcDlg::OnUpdateJTraj(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(tkJTrajectory == GetTrajectoryType());
}

void CCalcDlg::OnUpdateExtendedJTraj(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(tkExtendedJTrajectory == GetTrajectoryType());
}

void CCalcDlg::OnUpdateSTraj(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(tkSTrajectory == GetTrajectoryType());
}

void CCalcDlg::OnUpdateExtendedSTraj(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(tkExtendedSTrajectory == GetTrajectoryType());
}

/*
CTrajView* CCalcDlg::GetTrajView()
{
	return STATIC_DOWNCAST(CTrajView, GetParentFrame()->GetActiveView());
}
*/
