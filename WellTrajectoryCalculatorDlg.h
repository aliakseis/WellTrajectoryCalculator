
// WellTrajectoryCalculatorDlg.h : header file
//

#pragma once

#include "PlaneTrajMath.h"

#include <memory>

enum
{
	tkJTrajectory = 0,
	tkExtendedJTrajectory,
	tkSTrajectory,
	tkExtendedSTrajectory
};

class CTrajView;

// CWellTrajectoryCalculatorDlg dialog
class CCalcDlg : public CDialogEx
{
// Construction
public:
	CCalcDlg(CWnd* pParent = nullptr);	// standard constructor
	~CCalcDlg();
    CCalcDlg(const CCalcDlg&) = delete;
	CCalcDlg& operator =(const CCalcDlg&) = delete;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_2DDIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Construction
public:
//	CCalcDlg(CWnd* pParent = NULL);	// standard constructor
//	BOOL Create(CWnd* pParent = NULL);
	virtual void OnOK();
	virtual void OnCancel();
	bool TryToApply();

	float	m_fPhi1;
	float	m_fL1;
	float	m_fBR1;
	float	m_fPhi2;
	float	m_fL2;
	float	m_fBR2;
	float	m_fPhi3;
	float	m_fL3;


	// Dialog Data
		//{{AFX_DATA(CCalcDlg)
	//enum { IDD = IDD_2DDIALOG };
	CSliderCtrl	m_Sensitivity;
	float	m_fTVD;
	float	m_fDisp;
	//}}AFX_DATA
	PLANE_TRAJ_ARR m_c;
	//	CRect UpdateRect;
	BOOL m_bValidTrajectory = FALSE;
	BOOL m_bIsotropic = FALSE;
	BOOL m_bFixAngle = FALSE;

	int  GetMode() { return m_nMode; }
	BOOL IsWorkingMode();// { return 2 == NBits(m_nMode); }
	int  GetEditFlags();
	void Store(PLANE_TRAJ_ARR c);
	BOOL Load(const PLANE_TRAJ_ARR c);
	//void Restore(int nIndex);
	void SetTrajectoryType(int nTrajectoryType);
	int GetTrajectoryType() { return m_nTrajectoryType; }
	void SetHitAngle(float fHitAngle) { m_fHitAngle = fHitAngle; }

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CCalcDlg)
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableButtons(BOOL bEnable);
	void SetMode(int nFlag, BOOL bEnable);
	BOOL MakeChoice(int nFlag, float* fVal, float fBound, int nCount, BOOL bExcludeZero);
	void SetReadOnly(int nID, BOOL bReadOnly);
	//void Return(BOOL bSave);
	void DDX_Text_Ex(CDataExchange* pDX, int nIDC, float& value, int nFlag);
	//	EUCVariables eUnitType, int nUnitIDC);
	void SetValidTrajectory(BOOL bValid);
	void EnableButton(int nButtonID, int nSpinID, BOOL bEnable, BOOL bVisible);
	//CTrajView* GetTrajView();

	// Generated message map functions
	//{{AFX_MSG(CCalcDlg)
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	afx_msg void OnCheck5();
	afx_msg void OnCheck6();
	afx_msg void OnCheck7();
	afx_msg void OnCheck8();
	afx_msg void OnApply();
	afx_msg void OnIsotropic();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnButton7();
	afx_msg void OnButton8();
	afx_msg void OnJTraj();
	afx_msg void OnExtendedJTraj();
	afx_msg void OnSTraj();
	afx_msg void OnExtendedSTraj();
	afx_msg void OnUpdateJTraj(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExtendedJTraj(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSTraj(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExtendedSTraj(CCmdUI* pCmdUI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool TrySpinValue() { return TryToApply(); }
	//virtual DWORD GetWindowID() { return WPID_DTCalculate; }

private:
	int   m_nMode;
	int   m_nTrajectoryType;
	float m_fHitAngle;

    std::unique_ptr<CTrajView> m_pView;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
};
