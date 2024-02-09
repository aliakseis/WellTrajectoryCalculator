// trajview.h : header file
//

// #include "destrdoc.h"
#include "WellTrajectoryCalculatorDlg.h"

struct MARKER
{
    CPoint Point;
    double fSin, fCos;
};

enum TrajViewState
{
    TVS_NONE,
    TVS_ONMARKER,
    TVS_DRAGGING
};


/////////////////////////////////////////////////////////////////////////////
// CTrajView view

class CTrajView : public CWnd
{
public:
    CTrajView(CCalcDlg& calcDlg) : m_CalcDlg(calcDlg) { m_State = TVS_NONE; }
    virtual ~CTrajView();

    // Attributes
public:
    TrajViewState m_State;
    CCalcDlg& m_CalcDlg;
    MARKER m_Markers[5];
    int m_nMarker;
    CPoint m_InitialOffset;
    double m_fSin, m_fCos, m_fSavePhi, m_fSaveL, m_fSaveBR, m_fCoeffX, m_fCoeffY;
    // Operations
public:
    // Implementation
protected:
    void DrawGridAndScale(CDC* pDC, const CRect& rect, const double fXmin, const double fXmax, const double fYmin,
                          const double fYmax, const double fX, const double fCoeffX, const double fY, const double fCoeffY);
    void UpdateMinMax(int i, double& fX, double& fY, double& fR, double& sin0, double& sin1, double& cos0, double& cos1,
                        double& fXmin, double& fXmax, double& fYmin, double& fYmax);
    void DrawAxes(CDC* pDC);
    int GetSegNumber();
    int GetMarkerNumber();

    // Generated message map functions
protected:
    //{{AFX_MSG(CTrajView)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnPaint();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

/////////////////////////////////////////////////////////////////////////////
