
// WellTrajectoryCalculatorDlg.cpp : implementation file
//
#include "pch.h"

#include "WellTrajectoryCalculatorDlg.h"

#include "Trajview.h"
#include "WellTrajectoryCalculator.h"
#include "afxdialogex.h"
#include "framework.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{

void EditLock(CWnd* pEdit, bool flLock)
{
    enum
    {
        CLASS_SIZE = 100
    };

    CString WindowClass;

    ASSERT(::IsWindow(pEdit->m_hWnd));

    if (!::IsWindow(pEdit->m_hWnd))
        return;

    GetClassName(pEdit->m_hWnd, WindowClass.GetBufferSetLength(CLASS_SIZE), CLASS_SIZE);
    WindowClass.ReleaseBuffer();

    if (WindowClass == "Edit")
    {
        static_cast<CEdit*>(pEdit)->SetReadOnly(flLock);
    }
    else
    {
        pEdit->EnableWindow(!flLock);
    }
}

bool EnableEdit(CWnd* Ctrl, bool flEnable)
{
    if (Ctrl)
        EditLock(Ctrl, !flEnable);
    return true;
}

void ShowControl(CWnd* pWnd, bool Show)
{
    if (!pWnd)
        ASSERT(false);
    else
    {
        int nCmdShow = Show ? SW_SHOWNA : SW_HIDE;
        pWnd->ShowWindow(nCmdShow);
    }
}

}  // namespace

const auto Deg2Rad = (M_PI / 180.0);
const auto Rad2Deg = (180.0 / M_PI);

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_ABOUTBOX
    };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

    // Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) { CDialogEx::DoDataExchange(pDX); }

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CWellTrajectoryCalculatorDlg dialog

CCalcDlg::CCalcDlg(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_2DDIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_fL1 = 100.0;
    m_fPhi1 = 10.0;
    m_fBR1 = 40.0;

    m_fL2 = 100.0;
    m_fPhi2 = 50.0;
    m_fBR2 = -50.0;

    m_fL3 = 100.0;
    m_fPhi3 = 0.0;

    m_fTVD = 435.4;
    m_fDisp = 183.89;

    m_nMode = 0;
    m_bValidTrajectory = false;
    m_bIsotropic = false;
    m_nTrajectoryType = tkExtendedSTrajectory;

    m_pView = std::make_unique<CTrajView>(*this);
}

CCalcDlg::~CCalcDlg() = default;

void CCalcDlg::DDX_Text_Ex(CDataExchange* pDX, int nIDC, double& value, int nFlag)
{
    if (!(m_nMode & nFlag) || m_bValidTrajectory)
    {
        if (!pDX->m_bSaveAndValidate)
        {
            float v = static_cast<float>(value);
            DDX_Text(pDX, nIDC, v);
        }
        else
            DDX_Text(pDX, nIDC, value);
    }
    else if (!pDX->m_bSaveAndValidate)
        GetDlgItem(nIDC)->SetWindowText(_T(""));
}

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
ON_WM_VSCROLL()
ON_WM_GETMINMAXINFO()
ON_BN_CLICKED(IDC_ADD_TO_CLIPBOOK, &CCalcDlg::OnAddToClipbook)
ON_BN_CLICKED(IDC_CLIPBOOK, &CCalcDlg::OnRestoreFromClipbook)
END_MESSAGE_MAP()

// CWellTrajectoryCalculatorDlg message handlers

BOOL CCalcDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(false);
    if (pSysMenu != nullptr)
    {
        bool bNameValid;
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
    SetIcon(m_hIcon, true);   // Set big icon
    SetIcon(m_hIcon, false);  // Set small icon

    // Add extra initialization here

    m_pView->SubclassDlgItem(IDC_TRAJECTORY_VIEW, this);

    m_ToolBar.CreateEx(this);
    m_ToolBar.LoadToolBar(IDR_2D_DESIGN);
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

    m_Sensitivity.SetRange(0, 20, false);
    m_Sensitivity.SetPos(10);
    m_Sensitivity.SetLineSize(1);
    m_Sensitivity.SetPageSize(10);
    m_Sensitivity.SetPos(10);
    if (m_bFixAngle)
    {
        GetDlgItem(IDC_EDIT1)->EnableWindow(false);
        GetDlgItem(IDC_CHECK1)->EnableWindow(false);
    }
    for (int i = IDC_SPIN1; i <= IDC_SPIN8; i++) ((CSpinButtonCtrl*)GetDlgItem(i))->SetRange(-1, 1);

    CDialogTemplate dlgtemplate;
    if (dlgtemplate.Load(MAKEINTRESOURCE(IDD_2DDIALOG)))
    {
        dlgtemplate.GetSizeInPixels(&m_minSize);
        CRect rect(0, 0, m_minSize.cx, m_minSize.cy);
        AdjustWindowRectEx(&rect, GetStyle(), false, GetExStyle());
        m_minSize = rect.Size();
    }

    return true;  // return true  unless you set the focus to a control
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
        CPaintDC dc(this);  // device context for painting

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
HCURSOR CCalcDlg::OnQueryDragIcon() { return static_cast<HCURSOR>(m_hIcon); }

//////////////////////////////////////////////////////////////////////////////

void CCalcDlg::OnOK() { __super::OnOK(); }

const auto MaxLen = 100000.f;
const auto MaxRate = 100.f;

int NBits(int nBits)
{
    int nCount = 0;
    for (int i = 1; i <= 0x80; i <<= 1)
    {
        if (nBits & i)
            nCount++;
    }
    return nCount;
}

void CCalcDlg::SetReadOnly(int nID, bool bReadOnly) { EnableEdit(GetDlgItem(nID), !bReadOnly); }

void CCalcDlg::EnableButton(int nButtonID, int nSpinID, bool bEnable, bool bVisible)
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

void CCalcDlg::EnableButtons(bool bEnable)
{
    EnableButton(IDC_BUTTON1, IDC_SPIN1, bEnable && !(m_nMode & F_PHI1) && !m_bFixAngle, true);
    EnableButton(IDC_BUTTON2, IDC_SPIN2, bEnable && !(m_nMode & F_L1), true);
    EnableButton(IDC_BUTTON3, IDC_SPIN3, bEnable && !(m_nMode & F_R1), true);

    EnableButton(IDC_BUTTON4, IDC_SPIN4, bEnable && !(m_nMode & F_PHI2), true);
    EnableButton(IDC_BUTTON5, IDC_SPIN5, bEnable && !(m_nMode & F_L2), m_nTrajectoryType > tkJTrajectory);
    EnableButton(IDC_BUTTON6, IDC_SPIN6, bEnable && !(m_nMode & F_R2), m_nTrajectoryType > tkExtendedJTrajectory);

    EnableButton(IDC_BUTTON7, IDC_SPIN7, bEnable && !(m_nMode & F_PHI3), m_nTrajectoryType > tkExtendedJTrajectory);
    EnableButton(IDC_BUTTON8, IDC_SPIN8, bEnable && !(m_nMode & F_L3), m_nTrajectoryType > tkSTrajectory);

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

void CCalcDlg::SetMode(int nFlag, bool bEnable)
{
    UpdateData(true);
    if (bEnable)
    {
        m_nMode |= nFlag;
        int nCount = NBits(m_nMode);
        if (nCount > 2)
        {
            m_nMode = nFlag;
            if (nFlag != F_PHI1)
                CheckDlgButton(IDC_CHECK1, false);
            if (nFlag != F_L1)
                CheckDlgButton(IDC_CHECK2, false);
            if (nFlag != F_R1)
                CheckDlgButton(IDC_CHECK3, false);
            if (nFlag != F_PHI2)
                CheckDlgButton(IDC_CHECK4, false);
            if (nFlag != F_L2)
                CheckDlgButton(IDC_CHECK5, false);
            if (nFlag != F_R2)
                CheckDlgButton(IDC_CHECK6, false);
            if (nFlag != F_PHI3)
                CheckDlgButton(IDC_CHECK7, false);
            if (nFlag != F_L3)
                CheckDlgButton(IDC_CHECK8, false);
        }
    }
    else
        m_nMode &= ~nFlag;
    EnableButtons(NBits(m_nMode) == 2);
    if (!m_bValidTrajectory)
        UpdateData(false);
}

/////////////////////////////////////////////////////////////////////////////
// CCalcDlg message handlers

void CCalcDlg::OnCheck1() { SetMode(F_PHI1, IsDlgButtonChecked(IDC_CHECK1)); }

void CCalcDlg::OnCheck2() { SetMode(F_L1, IsDlgButtonChecked(IDC_CHECK2)); }

void CCalcDlg::OnCheck3() { SetMode(F_R1, IsDlgButtonChecked(IDC_CHECK3)); }

void CCalcDlg::OnCheck4() { SetMode(F_PHI2, IsDlgButtonChecked(IDC_CHECK4)); }

void CCalcDlg::OnCheck5() { SetMode(F_L2, IsDlgButtonChecked(IDC_CHECK5)); }

void CCalcDlg::OnCheck6() { SetMode(F_R2, IsDlgButtonChecked(IDC_CHECK6)); }

void CCalcDlg::OnCheck7() { SetMode(F_PHI3, IsDlgButtonChecked(IDC_CHECK7)); }

void CCalcDlg::OnCheck8() { SetMode(F_L3, IsDlgButtonChecked(IDC_CHECK8)); }

bool CheckData(const PLANE_TRAJ_ARR& c)
{
    if (c[0].L < 0 || c[1].L < 0 || c[2].L < 0 || c[0].L > MaxLen || c[1].L > MaxLen || c[2].L > MaxLen)
        return false;
    for (int i = 0; i < 2; i++)
    {
        double fAngle = c[i + 1].Phi - c[i].Phi;
        while (fAngle > M_PI) fAngle -= double(M_PI * 2);
        while (fAngle < -M_PI) fAngle += double(M_PI * 2);
        if (fAngle * c[i].R < 0)
            return false;
    }
    return true;
}

void CCalcDlg::Store(PLANE_TRAJ_ARR& c)
{
    c[0].L = m_fL1;
    c[0].Phi = m_fPhi1 * Deg2Rad;

    if (m_bValidTrajectory || !(m_nMode & F_R1))
        if (m_fBR1 == 0.)
            c[0].R = 0.f;  // return false;
        else
            c[0].R = Rad2Deg * 100 / m_fBR1;
    else
        c[0].R = 0.;
    c[1].L = m_fL2;
    c[1].Phi = m_fPhi2 * Deg2Rad;
    if (m_bValidTrajectory || !(m_nMode & F_R2))
        if (m_fBR2 == 0.)
            c[1].R = 0.f;  // return false;
        else
            c[1].R = Rad2Deg * 100 / m_fBR2;
    else
        c[1].R = 0.;
    c[2].L = m_fL3;
    c[2].Phi = m_fPhi3 * Deg2Rad;
    c[2].R = 0.f;
}

bool CCalcDlg::Load(const PLANE_TRAJ_ARR& c)
{
    double fBR1, fBR2;
    if (c[0].R != 0.)
        fBR1 = Rad2Deg * 100 / c[0].R;
    else
        fBR1 = 0.f;
    if (c[1].R != 0.)
        fBR2 = Rad2Deg * 100 / c[1].R;
    else
        fBR2 = 0.f;
    if (fabs(fBR1) > MaxRate || fabs(fBR2) > MaxRate)
        return false;

    m_fL1 = c[0].L;
    m_fPhi1 = c[0].Phi * Rad2Deg;
    m_fBR1 = fBR1;
    m_fL2 = c[1].L;
    m_fPhi2 = c[1].Phi * Rad2Deg;
    m_fBR2 = fBR2;
    m_fL3 = c[2].L;
    m_fPhi3 = c[2].Phi * Rad2Deg;
    return true;
}

bool CCalcDlg::TryToApply()
{
    PLANE_TRAJ_ARR c;
    if (m_fL1 < 0 || m_fL1 > MaxLen || fabs(m_fPhi1) > 180 || fabs(m_fBR1) > MaxRate || m_fL2 < 0 || m_fL2 > MaxLen ||
        fabs(m_fPhi2) > 180 || fabs(m_fBR2) > MaxRate || m_fL2 < 0 || m_fL2 > MaxLen || fabs(m_fPhi2) > 180)
        return false;
    /*
       c[0].L = m_fL1;
       c[0].Phi = m_fPhi1 * Deg2Rad;

       if(!(m_nMode & F_R1))
              if(m_fBR1 == 0.) c[0].R = 0.f;//return false;
            else c[0].R = Rad2Deg * 100 / m_fBR1;
       c[1].L = m_fL2;
       c[1].Phi = m_fPhi2 * Deg2Rad;
       if(!(m_nMode & F_R2))
              if(m_fBR2 == 0.) c[1].R = 0.f;//return false;
               else c[1].R = Rad2Deg * 100 / m_fBR2;
       c[2].L = m_fL3;
       c[2].Phi = m_fPhi3 * Deg2Rad;
       c[2].R = 0.f;
    */
    Store(c);
    CPlaneTrajMath Math(c, m_fTVD, m_fDisp);

    bool bCorrect = false;
    switch (m_nMode)
    {
    case F_L1 | F_R1:
        bCorrect = Math.CalcLwhereR(0);
        break;
    case F_L2 | F_R2:
        bCorrect = Math.CalcLwhereR(M_ROT_UP);
        break;
    case F_L2 | F_R1:
        bCorrect = Math.CalcLwhereR(M_TURN | M_ROT_UP);
        break;
    case F_L3 | F_R2:
        bCorrect = Math.CalcLwhereR(M_TURN);
        break;

    case F_L1 | F_R2:
        bCorrect = Math.CalcLltR(0);
        break;
    case F_L3 | F_R1:
        bCorrect = Math.CalcLltR(M_TURN);
        break;

    case F_L1 | F_L2:
        bCorrect = Math.CalcLL(0);
        break;
    case F_L1 | F_L3:
        bCorrect = Math.CalcLL(M_ROT_DOWN);
        break;
    case F_L2 | F_L3:
        bCorrect = Math.CalcLL(M_ROT_UP);
        break;

    case F_R1 | F_R2:
        bCorrect = Math.CalcRR();
        break;

    case F_L1 | F_PHI1:
        bCorrect = Math.CalcLwherePhi(0);
        break;
    case F_L2 | F_PHI2:
        bCorrect = Math.CalcLwherePhi(M_ROT_UP);
        break;
    case F_L3 | F_PHI3:
        bCorrect = Math.CalcLwherePhi(M_ROT_DOWN);
        break;

    case F_L1 | F_PHI2:
        bCorrect = Math.CalcLltPhi(0);
        break;
    case F_L1 | F_PHI3:
        bCorrect = Math.CalcLltPhi(M_TURN | M_ROT_DOWN);
        break;
    case F_L2 | F_PHI1:
        bCorrect = Math.CalcLltPhi(M_TURN | M_ROT_UP);
        break;
    case F_L2 | F_PHI3:
        bCorrect = Math.CalcLltPhi(M_ROT_UP);
        break;
    case F_L3 | F_PHI1:
        bCorrect = Math.CalcLltPhi(M_ROT_DOWN);
        break;
    case F_L3 | F_PHI2:
        bCorrect = Math.CalcLltPhi(M_TURN);
        break;

    case F_R1 | F_PHI1:
        bCorrect = Math.CalcRwherePhi(0);
        break;
    case F_R1 | F_PHI2:
        bCorrect = Math.CalcRwherePhi(M_TURN | M_ROT_UP);
        break;
    case F_R2 | F_PHI2:
        bCorrect = Math.CalcRwherePhi(M_ROT_UP);
        break;
    case F_R2 | F_PHI3:
        bCorrect = Math.CalcRwherePhi(M_TURN);
        break;

    case F_R2 | F_PHI1:
        bCorrect = Math.CalcRgtPhi(0);
        break;
    case F_R1 | F_PHI3:
        bCorrect = Math.CalcRgtPhi(M_TURN);
        break;

    case F_PHI1 | F_PHI2:
        bCorrect = Math.CalcPhiPhi(0);
        break;
    case F_PHI1 | F_PHI3:
        bCorrect = Math.CalcPhiPhi(M_ROT_DOWN);
        break;
    case F_PHI2 | F_PHI3:
        bCorrect = Math.CalcPhiPhi(M_ROT_UP);
        break;
    }
    if (!bCorrect || !CheckData(c) || !Load(c))
        return false;
    /*
            double fBR1,fBR2;
            if(c[0].R != 0.) fBR1 = Rad2Deg * 100 / c[0].R;
            else fBR1 = 0.f;
            if(c[1].R != 0.) fBR2 = Rad2Deg * 100 / c[1].R;
            else fBR2 = 0.f;
            if(fabs(fBR1) > MaxRate || fabs(fBR2) > MaxRate) return false;

       m_fL1 = c[0].L;
       m_fPhi1 = c[0].Phi * Rad2Deg;
       m_fBR1 = fBR1;
       m_fL2 = c[1].L;
       m_fPhi2 = c[1].Phi * Rad2Deg;
       m_fBR2 = fBR2;
       m_fL3 = c[2].L;
       m_fPhi3 = c[2].Phi * Rad2Deg;
    */
    m_c = c;
    SetValidTrajectory(true);
    //	UpdateData(false);
    //	GetDlgItem(IDOK)->EnableWindow(true);
    //	m_bValidTrajectory = true;
    //	CWnd* pParent = GetParent();
    //	if(pParent) pParent->InvalidateRect(&UpdateRect);
    EnableButtons(true);
    return true;
}

void CCalcDlg::OnApply()
{
    // TODO: Add your control notification handler code here
    if (!UpdateData(true))
        return;
    /*
       if(!(m_nMode & F_R1) && m_fBR1 == 0 ||
              !(m_nMode & F_R2) && m_fBR2 == 0) {
              MessageBox("Zero input."); return;
       }
    */
    if (!TryToApply())
    {
        SetValidTrajectory(false);
        MessageBox(_T("Invalid data set."));
    }
    EnableButtons(NBits(m_nMode) == 2);
}

void CCalcDlg::OnIsotropic()
{
    m_bIsotropic = IsDlgButtonChecked(IDC_Isotropic);
    m_pView->InvalidateRect(NULL);
}

bool CCalcDlg::MakeChoice(int nFlag, double* fVal, double fBound, int nCount, bool bExcludeZero)
{
    if (2 != NBits(m_nMode) || m_nMode & nFlag || !UpdateData(true))
        return false;
    if (TryToApply())
        return true;
    double fStartVal = *fVal;
    fBound /= nCount;
    bool bFind = false;

    SetCursor(LoadCursor(NULL, IDC_WAIT));
    for (int i = 1; i <= nCount; i++)
    {
        *fVal = fStartVal - fBound * i;
        if (!(bExcludeZero && 0 == (*fVal)) && TryToApply())
        {
            bFind = true;
            break;
        }
        *fVal = fStartVal + fBound * i;
        if (!(bExcludeZero && 0 == (*fVal)) && TryToApply())
        {
            bFind = true;
            break;
        }
    }
    SetCursor(LoadCursor(NULL, IDC_ARROW));

    if (!bFind)
    {
        *fVal = fStartVal;
        SetValidTrajectory(false);
        MessageBox(_T("Please try another parameter."));
    }
    return bFind;
}

void CCalcDlg::OnButton1()
{
    if (!MakeChoice(F_PHI1, &m_fPhi1, 360., 2000, false))
        GetDlgItem(IDC_BUTTON1)->EnableWindow(false);
}

void CCalcDlg::OnButton2()
{
    if (!MakeChoice(F_L1, &m_fL1, fmax(fabs(m_fTVD), fabs(m_fDisp)) * 2, 2000, false))
        GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
}

void CCalcDlg::OnButton3()
{
    if (!MakeChoice(F_R1, &m_fBR1, MaxRate * 2, 2000, true))
        GetDlgItem(IDC_BUTTON3)->EnableWindow(false);
}

void CCalcDlg::OnButton4()
{
    if (!MakeChoice(F_PHI2, &m_fPhi2, 360., 2000, false))
        GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
}

void CCalcDlg::OnButton5()
{
    if (!MakeChoice(F_L2, &m_fL2, fmax(fabs(m_fTVD), fabs(m_fDisp)) * 2, 2000, false))
        GetDlgItem(IDC_BUTTON5)->EnableWindow(false);
}

void CCalcDlg::OnButton6()
{
    if (!MakeChoice(F_R2, &m_fBR2, MaxRate * 2, 2000, true))
        GetDlgItem(IDC_BUTTON6)->EnableWindow(false);
}

void CCalcDlg::OnButton7()
{
    if (!MakeChoice(F_PHI3, &m_fPhi3, 360., 2000, false))
        GetDlgItem(IDC_BUTTON7)->EnableWindow(false);
}

void CCalcDlg::OnButton8()
{
    if (!MakeChoice(F_L3, &m_fL3, fmax(fabs(m_fTVD), fabs(m_fDisp)) * 2, 2000, false))
        GetDlgItem(IDC_BUTTON8)->EnableWindow(false);
}

void CCalcDlg::OnCancel() { __super::OnCancel(); }

bool CCalcDlg::IsWorkingMode() { return 2 == NBits(m_nMode); }

int CCalcDlg::GetEditFlags()
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
        nFlags = F_L1 | F_R1 | F_L2 | F_PHI2 | F_R2 | F_PHI3;
        break;
    case tkExtendedSTrajectory:
        nFlags = F_L1 | F_R1 | F_L2 | F_PHI2 | F_R2 | F_L3 | F_PHI3;
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

void CCalcDlg::SetValidTrajectory(bool bValid)
{
    GetDlgItem(IDOK)->EnableWindow(bValid);
    GetDlgItem(IDC_ADD_TO_CLIPBOOK)->EnableWindow(bValid);
    m_bValidTrajectory = bValid;
    UpdateData(false);
    m_pView->InvalidateRect(NULL);
}

void CCalcDlg::SetTrajectoryType(int nTrajectoryType)
{
    UpdateData(true);

    bool bInvalidate = nTrajectoryType < m_nTrajectoryType;
    if ((tkJTrajectory == nTrajectoryType || tkExtendedJTrajectory == nTrajectoryType) &&
        (tkSTrajectory == m_nTrajectoryType || tkExtendedSTrajectory == m_nTrajectoryType))
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
    default:
        ASSERT(0);
    }

    switch (m_nTrajectoryType)
    {
        // No breaks
    case tkJTrajectory:
        CheckDlgButton(IDC_CHECK5, false);
        ShowControl(GetDlgItem(IDC_EDIT5), false);
        ShowControl(GetDlgItem(IDC_CHECK5), false);
        //			ShowControl(GetDlgItem(IDC_BUTTON5), false);
        m_nMode &= ~F_L2;
    case tkExtendedJTrajectory:
        CheckDlgButton(IDC_CHECK6, false);
        CheckDlgButton(IDC_CHECK7, false);
        ShowControl(GetDlgItem(IDC_EDIT6), false);
        ShowControl(GetDlgItem(IDC_CHECK6), false);
        //			ShowControl(GetDlgItem(IDC_BUTTON6), false);
        ShowControl(GetDlgItem(IDC_EDIT7), false);
        ShowControl(GetDlgItem(IDC_CHECK7), false);
        //			ShowControl(GetDlgItem(IDC_BUTTON7), false);
        ShowControl(GetDlgItem(IDC_EDIT7Units), false);
        m_nMode &= ~(F_R2 | F_PHI3);
    case tkSTrajectory:
        CheckDlgButton(IDC_CHECK8, false);
        ShowControl(GetDlgItem(IDC_EDIT8), false);
        ShowControl(GetDlgItem(IDC_CHECK8), false);
        //			ShowControl(GetDlgItem(IDC_BUTTON8), false);
        m_nMode &= ~F_L3;
    case tkExtendedSTrajectory:
        break;
    default:
        ASSERT(0);
    }

    switch (m_nTrajectoryType)
    {
        // No breaks
    case tkExtendedSTrajectory:
        ShowControl(GetDlgItem(IDC_EDIT8), true);
        ShowControl(GetDlgItem(IDC_CHECK8), true);
        //			ShowControl(GetDlgItem(IDC_BUTTON8), true);
    case tkSTrajectory:
        ShowControl(GetDlgItem(IDC_EDIT6), true);
        ShowControl(GetDlgItem(IDC_CHECK6), true);
        //			ShowControl(GetDlgItem(IDC_BUTTON6), true);
        ShowControl(GetDlgItem(IDC_EDIT7), true);
        ShowControl(GetDlgItem(IDC_CHECK7), true);
        //			ShowControl(GetDlgItem(IDC_BUTTON7), true);
        ShowControl(GetDlgItem(IDC_EDIT7Units), true);
    case tkExtendedJTrajectory:
        ShowControl(GetDlgItem(IDC_EDIT5), true);
        ShowControl(GetDlgItem(IDC_CHECK5), true);
        //			ShowControl(GetDlgItem(IDC_BUTTON5), true);
    case tkJTrajectory:
        break;
    default:
        ASSERT(0);
    }

    if (bInvalidate)
        SetValidTrajectory(false);
    else
        UpdateData(false);  // Show/hide labels; SetValidTrajectory() has such a line inside
    EnableButtons(NBits(m_nMode) == 2);
}

static TCHAR szSpinClassName[] = _T("msctls_updown32");
#define SpinClassNameSize (sizeof(szSpinClassName) + sizeof(TCHAR))

BOOL CCalcDlg::PreTranslateMessage(MSG* pMsg)
{
    if (WM_KEYDOWN == pMsg->message && (VK_UP == pMsg->wParam || VK_DOWN == pMsg->wParam))
    {
        HWND hWndNext = ::GetWindow(pMsg->hwnd, GW_HWNDNEXT);
        if (IsWindow(hWndNext))
        {
            CString strClassName;
            GetClassName(hWndNext, strClassName.GetBuffer(SpinClassNameSize), SpinClassNameSize);
            if (strClassName == szSpinClassName)
            {
                OnVScroll(SB_THUMBPOSITION, (VK_UP == pMsg->wParam) ? 1 : -1,
                          static_cast<CScrollBar*>(CWnd::FromHandle(hWndNext)));
                return true;
            }
        }
    }
    return __super::PreTranslateMessage(pMsg);
}

void CCalcDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    static bool bRecurs = false;
    if (bRecurs || !pScrollBar || nSBCode != SB_THUMBPOSITION)
        return;
    bRecurs = true;
    ASSERT_VALID(pScrollBar);
    double* pVal;
    switch (pScrollBar->GetDlgCtrlID())
    {
    case IDC_SPIN1:
        pVal = &m_fPhi1;
        break;
    case IDC_SPIN2:
        pVal = &m_fL1;
        break;
    case IDC_SPIN3:
        pVal = &m_fBR1;
        break;
    case IDC_SPIN4:
        pVal = &m_fPhi2;
        break;
    case IDC_SPIN5:
        pVal = &m_fL2;
        break;
    case IDC_SPIN6:
        pVal = &m_fBR2;
        break;
    case IDC_SPIN7:
        pVal = &m_fPhi3;
        break;
    case IDC_SPIN8:
        pVal = &m_fL3;
        break;
    default:
        ASSERT(0);
        bRecurs = false;
        return;
    }
    ((CSpinButtonCtrl*)pScrollBar)->SetPos(0);
    double fSaveVal = *pVal;
    *pVal += (signed)nPos;
    if (!TrySpinValue())
        *pVal = fSaveVal;
    bRecurs = false;
}

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

void CCalcDlg::OnUpdateJTraj(CCmdUI* pCmdUI) { pCmdUI->SetRadio(tkJTrajectory == GetTrajectoryType()); }

void CCalcDlg::OnUpdateExtendedJTraj(CCmdUI* pCmdUI) { pCmdUI->SetRadio(tkExtendedJTrajectory == GetTrajectoryType()); }

void CCalcDlg::OnUpdateSTraj(CCmdUI* pCmdUI) { pCmdUI->SetRadio(tkSTrajectory == GetTrajectoryType()); }

void CCalcDlg::OnUpdateExtendedSTraj(CCmdUI* pCmdUI) { pCmdUI->SetRadio(tkExtendedSTrajectory == GetTrajectoryType()); }

void CCalcDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    CDialogEx::OnGetMinMaxInfo(lpMMI);

    if (m_minSize.cx && m_minSize.cy)
    {
        lpMMI->ptMinTrackSize.x = m_minSize.cx;
        lpMMI->ptMinTrackSize.y = m_minSize.cy;
    }
}

void CCalcDlg::OnAddToClipbook()
{
    Store(m_clip);
    m_fClipTVD = m_fTVD;
    m_fClipDisp = m_fDisp;
    GetDlgItem(IDC_CLIPBOOK)->EnableWindow(true);
}

void CCalcDlg::OnRestoreFromClipbook()
{
    if (Load(m_clip))
    {
        m_fTVD = m_fClipTVD;
        m_fDisp = m_fClipDisp;
        m_c = m_clip;
        SetValidTrajectory(true);
    }
}
