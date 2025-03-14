
// WellTrajectoryCalculatorDlg.h : header file
//

#pragma once

#include <memory>

#include "PlaneTrajMath.h"

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
    CCalcDlg(CWnd* pParent = nullptr);  // standard constructor
    ~CCalcDlg();
    CCalcDlg(const CCalcDlg&) = delete;
    CCalcDlg& operator=(const CCalcDlg&) = delete;

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_2DDIALOG
    };
#endif

protected:
    void DoDataExchange(CDataExchange* pDX) override;  // DDX/DDV support

public:
    void OnOK() override;
    void OnCancel() override;
    bool TryToApply();

    double m_fPhi1;
    double m_fL1;
    double m_fBR1;
    double m_fPhi2;
    double m_fL2;
    double m_fBR2;
    double m_fPhi3;
    double m_fL3;

    CSliderCtrl m_Sensitivity;

    double m_fTVD;
    double m_fDisp;
    PLANE_TRAJ_ARR m_c;

    double m_fClipTVD;
    double m_fClipDisp;
    PLANE_TRAJ_ARR m_clip;

    bool m_bValidTrajectory = false;
    bool m_bIsotropic = false;
    bool m_bFixAngle = false;

    int GetMode() { return m_nMode; }
    bool IsWorkingMode();
    int GetEditFlags();
    void Store(PLANE_TRAJ_ARR& c);
    bool Load(const PLANE_TRAJ_ARR& c);
    void SetTrajectoryType(int nTrajectoryType);
    int GetTrajectoryType() { return m_nTrajectoryType; }
    void SetHitAngle(double fHitAngle) { m_fHitAngle = fHitAngle; }

    // Implementation
protected:
    void EnableButtons(bool bEnable);
    void SetMode(int nFlag, bool bEnable);
    bool MakeChoice(int nFlag, double* fVal, double fBound, int nCount, bool bExcludeZero);
    void SetReadOnly(int nID, bool bReadOnly);
    void DDX_Text_Ex(CDataExchange* pDX, int nIDC, double& value, int nFlag);
    void SetValidTrajectory(bool bValid);
    void EnableButton(int nButtonID, int nSpinID, bool bEnable, bool bVisible);

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
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    bool TrySpinValue() { return TryToApply(); }
    BOOL PreTranslateMessage(MSG* pMsg) override;

private:
    int m_nMode;
    int m_nTrajectoryType;
    double m_fHitAngle;

    std::unique_ptr<CTrajView> m_pView;
    CToolBar m_ToolBar;

    CSize m_minSize{};

    // Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
     BOOL OnInitDialog() override;

public:
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnAddToClipbook();
    afx_msg void OnRestoreFromClipbook();
};
