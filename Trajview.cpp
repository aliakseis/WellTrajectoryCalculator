// trajview.cpp : implementation file
//
#include "pch.h"

#include "Trajview.h"

#include "resource.h"

#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

namespace
{

double GetStep(double fRange)
{
    double fStep = (fRange > 0) ? 1e10f : -1e10f;
    while (fabs(fStep) > fabs(fRange)) fStep /= 10;
    if (fabs(fStep) * 5 <= fabs(fRange))
        fStep *= 5;
    else if (fabs(fStep) * 2 <= fabs(fRange))
        fStep *= 2;
    return fStep;
}

void DrawMarker(CDC* pDC, const CPoint& Point)
{
    CRect rect;
    rect.left = Point.x - 4;
    rect.top = Point.y - 4;
    rect.right = Point.x + 4;
    rect.bottom = Point.y + 4;
    pDC->Rectangle(&rect);
}

bool IsMarkerHere(const CPoint& Marker, const CPoint& Mouse)
{
    return (Mouse.x >= Marker.x - 4 && Mouse.x <= Marker.x + 4 && Mouse.y >= Marker.y - 4 && Mouse.y <= Marker.y + 4);
}

bool FitEllipseRect(CRect& rect)
{
    rect.NormalizeRect();
    int nWidth = rect.Width();
    int nHeight = rect.Height();
    if (nWidth < 3 || nHeight < 3)
        return false;
    if (nWidth > 32766)
        if (nHeight > 32766)
            return false;
        else
        {
            if (abs(rect.right) > abs(rect.left))
                rect.right = rect.left + 32766;
            else
                rect.left = rect.right - 32766;
            int nDelta = int(double(nHeight) * (1. - sqrt(32766. / double(nWidth))) / 2.);
            rect.top += nDelta;
            rect.bottom -= nDelta;
        }
    else if (nHeight > 32766)
    {
        if (abs(rect.bottom) > abs(rect.top))
            rect.bottom = rect.top + 32766;
        else
            rect.bottom -= 32766;
        int nDelta = int(double(nWidth) * (1. - sqrt(32766. / double(nHeight))) / 2.);
        rect.left += nDelta;
        rect.right -= nDelta;
    }
    return true;
}

double fsqr(double x) { return x * x; }

}  // namespace

/////////////////////////////////////////////////////////////////////////////
// CTrajView

// IMPLEMENT_DYNCREATE(CTrajView, CBaseView)

CTrajView::~CTrajView() {}

BEGIN_MESSAGE_MAP(CTrajView, CWnd)
//{{AFX_MSG_MAP(CTrajView)
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_SETCURSOR()
ON_WM_LBUTTONUP()
ON_WM_PAINT()
//}}AFX_MSG_MAP
ON_WM_NCHITTEST()
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrajView drawing


void CTrajView::DrawAxes(CDC* pDC)
{
    if (m_State != TVS_DRAGGING)
        return;

    CPen penAxes;
    penAxes.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
    auto pOldPen = pDC->SelectObject(&penAxes);

    CPoint Line[2];
    int nFlag;
    double fNorm = 160.f / sqrt(fsqr(m_fCoeffY * m_fCos) + fsqr(m_fCoeffX * m_fSin));

    switch (m_nMarker)
    {
    case 0:
        nFlag = F_PHI1;
        break;
    case 2:
        nFlag = F_PHI2;
        break;
    case 4:
        nFlag = F_PHI3;
        break;
    default:
        nFlag = 0x00FF;
    }
    if (m_CalcDlg.GetEditFlags() & nFlag)
    {
        Line[0].x = (int)(m_InitialOffset.x - fNorm * m_fCoeffY * m_fCos);
        Line[0].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffX * m_fSin);
        Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffY * m_fCos);
        Line[1].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffX * m_fSin);
        pDC->Polyline(Line, 2);
        Line[0].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffY * m_fCos * 0.55);
        Line[0].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffX * m_fSin * 0.55);
        Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffY * m_fCos * 0.45 + fNorm * m_fCoeffX * m_fSin * 0.03);
        Line[1].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffX * m_fSin * 0.45 + fNorm * m_fCoeffY * m_fCos * 0.03);
        pDC->Polyline(Line, 2);
        Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffY * m_fCos * 0.45 - fNorm * m_fCoeffX * m_fSin * 0.03);
        Line[1].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffX * m_fSin * 0.45 - fNorm * m_fCoeffY * m_fCos * 0.03);
    }
    else
    {
        Line[0].x = (int)(m_InitialOffset.x - fNorm * m_fCoeffY * m_fCos * 0.03);
        Line[0].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffX * m_fSin * 0.03);
        Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffY * m_fCos * 0.03);
        Line[1].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffX * m_fSin * 0.03);
    }
    pDC->Polyline(Line, 2);
    switch (m_nMarker)
    {
    case 0:
        nFlag = F_L1;
        break;
    case 2:
        nFlag = F_L2;
        break;
    case 4:
        nFlag = F_L3;
        break;
    default:
        nFlag = 0;
    }
    if (/*!(m_nMarker & 1) && (*/ m_CalcDlg.GetEditFlags() & nFlag)
    {
        Line[0].x = (int)(m_InitialOffset.x - fNorm * m_fCoeffX * m_fSin);
        Line[0].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffY * m_fCos);
        Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffX * m_fSin);
        Line[1].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffY * m_fCos);
        pDC->Polyline(Line, 2);
        Line[0].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffX * m_fSin * 0.55);
        Line[0].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffY * m_fCos * 0.55);
        Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffX * m_fSin * 0.45 + fNorm * m_fCoeffY * m_fCos * 0.03);
        Line[1].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffY * m_fCos * 0.45 - fNorm * m_fCoeffX * m_fSin * 0.03);
        pDC->Polyline(Line, 2);
        Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffX * m_fSin * 0.45 - fNorm * m_fCoeffY * m_fCos * 0.03);
        Line[1].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffY * m_fCos * 0.45 + fNorm * m_fCoeffX * m_fSin * 0.03);
    }
    else
    {
        Line[0].x = (int)(m_InitialOffset.x - fNorm * m_fCoeffX * m_fSin * 0.03);
        Line[0].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffY * m_fCos * 0.03);
        Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffX * m_fSin * 0.03);
        Line[1].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffY * m_fCos * 0.03);
    }
    pDC->Polyline(Line, 2);

    pDC->SelectObject(pOldPen);
}


const auto Epsilon = 1.e-6f;


void CTrajView::DrawGridAndScale(CDC* pDC, const CRect& rect, const double fXmin, const double fXmax,
                                 const double fYmin, const double fYmax, const double fX, const double fCoeffX,
                                 const double fY, const double fCoeffY)
{
    double fEvenStepX = GetStep((fXmax - fXmin) / 4);
    double fEvenStepY = GetStep((fYmax - fYmin) / 4);
    double fStepX = fEvenStepX;
    double fStepY = fEvenStepY;
    if (m_CalcDlg.m_bIsotropic)
        fStepX = fStepY = std::max(fStepX, fStepY);

    double fOffsetX = fX * fCoeffX;
    while (fOffsetX + Epsilon >= 0) fOffsetX -= fStepX * fCoeffX;
    fOffsetX += rect.left;

    double fOffsetY = fY * fCoeffY;
    while (fOffsetY + Epsilon >= 0) fOffsetY -= fStepY * fCoeffY;
    fOffsetY += rect.top;

    CPen penGrid(PS_SOLID, 1, RGB(196, 196, 196));
    auto pOldPen = pDC->SelectObject(&penGrid);

    int nScaleLeft, nScaleRight, nScaleTop, nScaleBottom;
    for (int i = 1;; i++)
    {
        int x = int(fOffsetX + fStepX * fCoeffX * i);
        if (x > rect.right)
            break;
        pDC->MoveTo(x, rect.bottom);
        pDC->LineTo(x, rect.top);
        switch (i)
        {
        case 1:
            nScaleLeft = x;
            break;
        case 2:
            nScaleRight = x;
            break;
        }
    }

    for (int i = 1;; i++)
    {
        int y = int(fOffsetY + fStepY * fCoeffY * i);
        if (y > rect.bottom)
            break;
        pDC->MoveTo(rect.left, y);
        pDC->LineTo(rect.right, y);
        switch (i)
        {
        case 1:
            nScaleTop = y;
            break;
        case 2:
            nScaleBottom = y;
            break;
        }
    }

    pDC->MoveTo(nScaleLeft, rect.top - 8);
    pDC->LineTo(nScaleRight, rect.top - 8);
    pDC->MoveTo(nScaleLeft, rect.top - 6);
    pDC->LineTo(nScaleLeft, rect.top - 11);
    pDC->MoveTo(nScaleRight, rect.top - 6);
    pDC->LineTo(nScaleRight, rect.top - 11);

    pDC->MoveTo(rect.left - 8, nScaleTop);
    pDC->LineTo(rect.left - 8, nScaleBottom);
    pDC->MoveTo(rect.left - 6, nScaleTop);
    pDC->LineTo(rect.left - 11, nScaleTop);
    pDC->MoveTo(rect.left - 6, nScaleBottom);
    pDC->LineTo(rect.left - 11, nScaleBottom);

    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));
    lf.lfHeight = 14;
    lf.lfWeight = 400;
    _tcscpy_s(lf.lfFaceName, _T("Arial"));

    CString strBuf;
    CString strUnit;  // = GetLabel_(UV_4_TRAJ_LENGTH);

    CFont fontScaleHorz;
    fontScaleHorz.CreateFontIndirect(&lf);
    CFont* pOldFont = pDC->SelectObject(&fontScaleHorz);
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextAlign(TA_LEFT | TA_BASELINE);
    strBuf.Format(_T("%.0f %s"), fEvenStepX, (LPCTSTR)strUnit);
    pDC->TextOut(nScaleRight + 6, rect.top - 6, strBuf);

    lf.lfEscapement = 900;
    lf.lfHeight = 12;
    CFont fontScaleVert;
    fontScaleVert.CreateFontIndirect(&lf);
    pDC->SelectObject(&fontScaleVert);
    pDC->SetTextAlign(TA_RIGHT | TA_BASELINE);
    strBuf.Format(_T("%.0f %s"), fEvenStepY, (LPCTSTR)strUnit);
    pDC->TextOut(rect.left - 6, nScaleBottom + 6, strBuf);
    pDC->SelectObject(pOldFont);

    pDC->SelectObject(pOldPen);

    CBrush* pOldBrush = pDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
    pDC->Rectangle(&rect);
    pDC->SelectObject(pOldBrush);
}

void CTrajView::UpdateMinMax(const int i, const double fX, const double fY, const double fR, const double sin0,
                             const double sin1, const double cos0, const double cos1, double& fXmin, double& fXmax,
                             double& fYmin, double& fYmax)
{
    double fRabs = fabs(fR);
    double fAngle = m_CalcDlg.m_c[i + 1].Phi - m_CalcDlg.m_c[i].Phi;
    while (fAngle > M_PI) fAngle -= (2 * M_PI);
    while (fAngle < -M_PI) fAngle += (2 * M_PI);
    bool bIsHalfTurn = (fAngle * fR < 0);
    bool bExtendX = false;
    bool bExtendY = false;
    int nMayBeLoop = 0;
    if (sin0 <= 0 && sin1 >= 0)
    {
        fXmin = fmin(fXmin, fX - fRabs);
        if (bIsHalfTurn)
            bExtendY = true;
    }
    else if (sin0 >= 0 && sin1 <= 0)
    {
        fXmax = fmax(fXmax, fX + fRabs);
        if (bIsHalfTurn)
            bExtendY = true;
    }
    else
        nMayBeLoop++;

    if (cos0 <= 0 && cos1 >= 0)
    {
        fYmin = fmin(fYmin, fY - fRabs);
        if (bIsHalfTurn)
            bExtendX = true;
    }
    else if (cos0 >= 0 && cos1 <= 0)
    {
        fYmax = fmax(fYmax, fY + fRabs);
        if (bIsHalfTurn)
            bExtendX = true;
    }
    else
        nMayBeLoop++;
    if (!bIsHalfTurn)
        nMayBeLoop = 0;

    if (nMayBeLoop == 2 || bExtendX)
    {
        fXmin = fmin(fXmin, fX - fRabs);
        fXmax = fmax(fXmax, fX + fRabs);
    }
    if (nMayBeLoop == 2 || bExtendY)
    {
        fYmin = fmin(fYmin, fY - fRabs);
        fYmax = fmax(fYmax, fY + fRabs);
    }
}


// A method that performs custom painting on the view
void CTrajView::OnPaint()
{
    // If the trajectory is not valid, do nothing
    if (!m_CalcDlg.m_bValidTrajectory)
    {
        ValidateRect(NULL);
        return;
    }

    // Create a device context for painting
    CPaintDC dc(this);

    // Get the client rectangle of the view
    CRect rect;
    GetClientRect(&rect);

    // Clip the device context to the client rectangle
    dc.IntersectClipRect(rect);

    // Adjust the rectangle to leave some margins
    rect.left += /*nDlgWidth +*/ 20;
    rect.top += 20;
    rect.right -= 12;
    rect.bottom -= 12;

    // Initialize the minimum and maximum values of x and y coordinates
    auto [fYmin, fYmax] = std::minmax({0.0, m_CalcDlg.m_fTVD});
    auto [fXmin, fXmax] = std::minmax({0.0, m_CalcDlg.m_fDisp});

    // Initialize the current values of x and y coordinates
    double fX = 0.0;
    double fY = 0.0;

    // Initialize the coefficients for scaling the coordinates
    double fCoeffX = 0.0;
    double fCoeffY = 0.0;

    // Create a pen for drawing the trajectory
    CPen Pen;
    Pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
    CPen* pOldPen = NULL;

    // Loop twice: first to calculate the coefficients, second to draw the trajectory
    for (int bDraw = 0; bDraw < 2; bDraw++)
    {
        if (bDraw)
        {
            // Draw the grid and the scale on the device context
            DrawGridAndScale(&dc, rect, fXmin, fXmax, fYmin, fYmax, fX, fCoeffX, fY, fCoeffY);

            // Draw the axes on the device context
            DrawAxes(&dc);

            // Select the pen for drawing the trajectory
            pOldPen = dc.SelectObject(&Pen);
        }

        // Create an array of points for drawing the lines
        CPoint Line[2];
        Line[0].x = (int)(rect.left + fX * fCoeffX);
        Line[0].y = (int)(rect.top + fY * fCoeffY);

        // Loop through the segments of the trajectory
        for (int i = 0; i < 3; i++)
        {
            // Calculate the sine and cosine of the angle of the segment
            double sin0 = sin(m_CalcDlg.m_c[i].Phi);
            double cos0 = cos(m_CalcDlg.m_c[i].Phi);

            // Update the x and y coordinates by adding the length of the segment
            fX += m_CalcDlg.m_c[i].L * sin0;
            fY += m_CalcDlg.m_c[i].L * cos0;

            // Update the second point of the line
            Line[1].x = (int)(rect.left + fX * fCoeffX);
            Line[1].y = (int)(rect.top + fY * fCoeffY);

            // Update the marker position and orientation
            m_Markers[i * 2].Point.x = (Line[0].x + Line[1].x) / 2;
            m_Markers[i * 2].Point.y = (Line[0].y + Line[1].y) / 2;
            m_Markers[i * 2].fSin = sin0;
            m_Markers[i * 2].fCos = cos0;

            // Draw the line if in the second loop
            if (bDraw)
            {
                dc.Polyline(Line, 2);
            }

            // If not the last segment, draw the circle
            if (i < 2)
            {
                // Get the radius of the circle
                double fR = m_CalcDlg.m_c[i].R;

                // Update the x and y coordinates by adding the offset of the circle
                fX += fR * cos0;
                fY -= fR * sin0;

                // Calculate the sine and cosine of the angle of the next segment
                double sin1 = sin(m_CalcDlg.m_c[i + 1].Phi);
                double cos1 = cos(m_CalcDlg.m_c[i + 1].Phi);

                // If in the first loop, update the minimum and maximum values of x and y coordinates
                if (!bDraw)
                {
                    UpdateMinMax(i, fX, fY, fR, sin0, sin1, cos0, cos1, fXmin, fXmax, fYmin, fYmax);
                }

                // Create a rectangle that bounds the circle
                CRect bound;
                bound.left = (int)(rect.left + (fX - fR) * fCoeffX);
                bound.top = (int)(rect.top + (fY - fR) * fCoeffY);
                bound.right = (int)(rect.left + (fX + fR) * fCoeffX);
                bound.bottom = (int)(rect.top + (fY + fR) * fCoeffY);

                // Update the x and y coordinates by subtracting the offset of the circle
                fX -= fR * cos1;
                fY += fR * sin1;

                // Update the first point of the line
                Line[0].x = (int)(rect.left + fX * fCoeffX);
                Line[0].y = (int)(rect.top + fY * fCoeffY);

                // Update the marker position and orientation
                m_Markers[i * 2 + 1].Point.x = (Line[0].x + Line[1].x) / 2;
                m_Markers[i * 2 + 1].Point.y = (Line[0].y + Line[1].y) / 2;
                m_Markers[i * 2 + 1].fSin = sin((m_CalcDlg.m_c[i].Phi + m_CalcDlg.m_c[i + 1].Phi) / 2);
                m_Markers[i * 2 + 1].fCos = cos((m_CalcDlg.m_c[i].Phi + m_CalcDlg.m_c[i + 1].Phi) / 2);

                // Draw the arc if in the second loop and the circle is not degenerate
                if (bDraw && m_CalcDlg.m_c[i + 1].Phi != m_CalcDlg.m_c[i].Phi && Line[0] != Line[1] && fR != 0)
                {
                    // Fit the ellipse to the rectangle
                    bool bArcOK = FitEllipseRect(bound);

                    // Draw the arc with the correct orientation
                    if (bArcOK)
                    {
                        if (fR < 0)
                        {
                            bArcOK = dc.Arc(bound, Line[0], Line[1]);
                        }
                        else
                        {
                            bArcOK = dc.Arc(bound, Line[1], Line[0]);
                        }
                    }

                    // If the arc drawing failed, draw a line instead
                    if (!bArcOK)
                    {
                        dc.Polyline(Line, 2);
                    }
                }
            }
        }

        // Calculate the coefficients for scaling the coordinates
        fX = -fXmin;
        fY = -fYmin;
        fCoeffX = (rect.Width()) / (fXmax - fXmin);
        fCoeffY = (rect.Height()) / (fYmax - fYmin);

        // If the trajectory is isotropic, use the same coefficient for both axes
        if (m_CalcDlg.m_bIsotropic)
        {
            fCoeffX = fCoeffY = std::min(fCoeffX, fCoeffY);
        }
    }

    // Restore the old pen
    dc.SelectObject(pOldPen);
    if (m_State != TVS_DRAGGING)
    {
        m_fCoeffX = fCoeffX;
        m_fCoeffY = fCoeffY;
        for (int i = 0; i < GetMarkerNumber(); i++) 
            DrawMarker(&dc, m_Markers[i].Point);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CTrajView message handlers

void CTrajView::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_State == TVS_ONMARKER)
    {
        SetCapture();
        m_State = TVS_DRAGGING;
        m_InitialOffset.x = point.x;  // - m_Markers[m_nMarker].Point.x;
        m_InitialOffset.y = point.y;  // - m_Markers[m_nMarker].Point.y;
        m_fSin = m_Markers[m_nMarker].fSin;
        m_fCos = m_Markers[m_nMarker].fCos;
        switch (m_nMarker)
        {
        case 0:
            m_fSavePhi = m_CalcDlg.m_fPhi1;
            m_fSaveL = m_CalcDlg.m_fL1;
            break;
        case 1:
            m_fSaveBR = m_CalcDlg.m_fBR1;
            break;
        case 2:
            m_fSavePhi = m_CalcDlg.m_fPhi2;
            m_fSaveL = m_CalcDlg.m_fL2;
            break;
        case 3:
            m_fSaveBR = m_CalcDlg.m_fBR2;
            break;
        case 4:
            m_fSavePhi = m_CalcDlg.m_fPhi3;
            m_fSaveL = m_CalcDlg.m_fL3;
            m_fSin = -m_fSin;
            m_fCos = -m_fCos;  // Rotate axes on 180 degr.
            break;
        }
    }
}

void CTrajView::OnMouseMove(UINT nFlags, CPoint point)
{
    int nMarker;
    double fSaveL, fSaveBR, fSavePhi;
    if (m_CalcDlg.IsWorkingMode() && m_CalcDlg.m_bValidTrajectory)
    {
        bool bOnMarker = false;
        for (nMarker = 0; nMarker < GetMarkerNumber(); nMarker++)
        {
            switch (nMarker)
            {
            case 0:
                if (!(m_CalcDlg.GetEditFlags() & (F_L1 | F_PHI1)))
                    continue;
                break;
            case 1:
                if (!(m_CalcDlg.GetEditFlags() & F_R1))
                    continue;
                break;
            case 2:
                if (!(m_CalcDlg.GetEditFlags() & (F_L2 | F_PHI2)))
                    continue;
                break;
            case 3:
                if (!(m_CalcDlg.GetEditFlags() & F_R2))
                    continue;
                break;
            case 4:
                if (!(m_CalcDlg.GetEditFlags() & (F_L3 | F_PHI3)))
                    continue;
                break;
            }
            if (IsMarkerHere(m_Markers[nMarker].Point, point))
            {
                bOnMarker = true;
                break;
            }
        }
        switch (m_State)
        {
        case TVS_NONE:
            if (bOnMarker)
            {
                SetCursor(LoadCursor(NULL, IDC_CROSS));
                m_State = TVS_ONMARKER;
                m_nMarker = nMarker;
            }
            break;
        case TVS_ONMARKER:
            if (!bOnMarker)
            {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                m_State = TVS_NONE;
            }
            break;
        case TVS_DRAGGING:
            double fSensitivity = exp(0.3 * (m_CalcDlg.m_Sensitivity.GetPos() - 10));
            double x =
                fSensitivity *
                (point.x - m_InitialOffset.x);  // * sqrt(m_fCoeffY / m_fCoeffX);// - m_Markers[m_nMarker].Point.x;
            double y =
                fSensitivity *
                (point.y - m_InitialOffset.y);  // * sqrt(m_fCoeffX / m_fCoeffY);// - m_Markers[m_nMarker].Point.y;
            double fConv = sqrt(m_fCoeffX / m_fCoeffY);

            double fLong = y * m_fCos / fConv + x * m_fSin * fConv;
            double fTrans = x * m_fCos / fConv - y * m_fSin * fConv;

            //			      double fLong = y * m_fCos / m_fCoeffX +
            //			                    x * m_fSin / m_fCoeffY;
            //			      double fTrans =x * m_fCos / m_fCoeffX -
            //			      				  y * m_fSin / m_fCoeffY;

            const auto fCoeffPhi = 0.5;
            const auto fCoeffL = 1.;
            const auto fCoeffBR = 1.;

            bool bAllowed = true;
            switch (m_nMarker)
            {
            case 0:
                fSavePhi = m_CalcDlg.m_fPhi1;
                fSaveL = m_CalcDlg.m_fL1;
                if (m_CalcDlg.GetEditFlags() & F_PHI1)
                    m_CalcDlg.m_fPhi1 = m_fSavePhi + fTrans * fCoeffPhi;
                if (m_CalcDlg.GetEditFlags() & F_L1)
                    m_CalcDlg.m_fL1 = m_fSaveL + fLong * fCoeffL;
                if (!m_CalcDlg.TryToApply())
                {
                    m_CalcDlg.m_fPhi1 = fSavePhi;
                    m_CalcDlg.m_fL1 = fSaveL;
                    bAllowed = false;
                }
                break;
            case 1:
                fSaveBR = m_CalcDlg.m_fBR1;
                m_CalcDlg.m_fBR1 = m_fSaveBR + fTrans * fCoeffBR;
                if (!m_CalcDlg.TryToApply())
                {
                    m_CalcDlg.m_fBR1 = fSaveBR;
                    bAllowed = false;
                }
                break;
            case 2:
                fSavePhi = m_CalcDlg.m_fPhi2;
                fSaveL = m_CalcDlg.m_fL2;
                if (m_CalcDlg.GetEditFlags() & F_PHI2)
                    m_CalcDlg.m_fPhi2 = m_fSavePhi + fTrans * fCoeffPhi;
                if (m_CalcDlg.GetEditFlags() & F_L2)
                    m_CalcDlg.m_fL2 = m_fSaveL + fLong * fCoeffL;
                if (!m_CalcDlg.TryToApply())
                {
                    m_CalcDlg.m_fPhi2 = fSavePhi;
                    m_CalcDlg.m_fL2 = fSaveL;
                    bAllowed = false;
                }
                break;
            case 3:
                fSaveBR = m_CalcDlg.m_fBR2;
                m_CalcDlg.m_fBR2 = m_fSaveBR + fTrans * fCoeffBR;
                if (!m_CalcDlg.TryToApply())
                {
                    m_CalcDlg.m_fBR2 = fSaveBR;
                    bAllowed = false;
                }
                break;
            case 4:
                fSavePhi = m_CalcDlg.m_fPhi3;
                fSaveL = m_CalcDlg.m_fL3;
                if (m_CalcDlg.GetEditFlags() & F_PHI3)
                    m_CalcDlg.m_fPhi3 = m_fSavePhi + fTrans * fCoeffPhi;
                if (m_CalcDlg.GetEditFlags() & F_L3)
                    m_CalcDlg.m_fL3 = m_fSaveL + fLong * fCoeffL;
                if (!m_CalcDlg.TryToApply())
                {
                    m_CalcDlg.m_fPhi3 = fSavePhi;
                    m_CalcDlg.m_fL3 = fSaveL;
                    bAllowed = false;
                }
                break;
            }
            if (bAllowed)
                SetCursor(LoadCursor(NULL, IDC_CROSS));
            else
                SetCursor(AfxGetApp()->LoadCursor((LPCTSTR)IDC_NOACTION));
            break;
        }
    }
    __super::OnMouseMove(nFlags, point);
}

BOOL CTrajView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (m_State == TVS_ONMARKER)
    {
        SetCursor(LoadCursor(NULL, IDC_CROSS));
        return true;
    }
    return __super::OnSetCursor(pWnd, nHitTest, message);
}

void CTrajView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_State == TVS_DRAGGING)
    {
        ReleaseCapture();
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        m_State = TVS_NONE;
        DestroyCursor(SetCursor(AfxGetApp()->LoadCursor((LPCTSTR)IDC_NOACTION)));
        InvalidateRect(NULL);  //&m_CalcDlg.UpdateRect);
    }
}

int CTrajView::GetSegNumber() { return m_CalcDlg.GetTrajectoryType() + 2; }

int CTrajView::GetMarkerNumber()
{
    switch (m_CalcDlg.GetTrajectoryType())
    {
    case tkJTrajectory:
    case tkExtendedJTrajectory:
        return 3;
    case tkSTrajectory:
    case tkExtendedSTrajectory:
        return 5;
    default:
        ASSERT(0);
        return 0;
    }
}

LRESULT CTrajView::OnNcHitTest(CPoint point)
{
    // https://groups.google.com/g/microsoft.public.vc.mfc/c/fHJiQmwZpfk
    return HTCLIENT;
}

BOOL CTrajView::OnEraseBkgnd(CDC* pDC)
{
    auto pPrevBrush = pDC->SelectObject(GetStockObject(WHITE_BRUSH));
    CRect rect;
    GetClientRect(&rect);
    pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
    pDC->SelectObject(pPrevBrush);
    return true;
}
