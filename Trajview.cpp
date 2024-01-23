// trajview.cpp : implementation file
//

#include "pch.h"
#include "resource.h"
#include "Trajview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

namespace {

float GetStep(float fRange)
{
   float fStep=(fRange>0)? 1e10f : -1e10f;
   while(fabs(fStep) > fabs(fRange)) fStep /= 10;
   if(fabs(fStep) * 5 <= fabs(fRange)) fStep *= 5;
   else if(fabs(fStep) * 2 <= fabs(fRange)) fStep *= 2;
   return fStep;
}


}

/////////////////////////////////////////////////////////////////////////////
// CTrajView

//IMPLEMENT_DYNCREATE(CTrajView, CBaseView)


CTrajView::~CTrajView()
{
}


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


void DrawMarker(CDC* pDC, CPoint Point)
{
	CRect rect;
	rect.left = Point.x - 4;
	rect.top = Point.y - 4;
	rect.right = Point.x + 4;
	rect.bottom = Point.y + 4;
   pDC->Rectangle(&rect);
}

BOOL IsMarkerHere(CPoint Marker, CPoint Mouse)
{
	return ( Mouse.x >= Marker.x - 4 &&
				Mouse.x <= Marker.x + 4 &&
				Mouse.y >= Marker.y - 4 &&
				Mouse.y <= Marker.y + 4);
}

/////////////////////////////////////////////////////////////////////////////
// CTrajView drawing


inline float fsqr(float x) {return x * x; }

void CTrajView::DrawAxes(CDC* pDC)
{
   CPoint Line[2];
	int nFlag;
   if(m_State != TVS_DRAGGING)
	   return;
	float fNorm = 160.f / (float)sqrt(fsqr(m_fCoeffY * m_fCos) + fsqr(m_fCoeffX * m_fSin));

	switch(m_nMarker) {
		case 0: nFlag = F_PHI1; break;
		case 2: nFlag = F_PHI2; break;
		case 4: nFlag = F_PHI3; break;
		default:nFlag = 0x00FF;
	}
	if(m_CalcDlg.GetEditFlags() & nFlag) 
	{
		Line[0].x = (int)(m_InitialOffset.x - fNorm * m_fCoeffY * m_fCos);
		Line[0].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffX * m_fSin);
		Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffY * m_fCos);
		Line[1].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffX * m_fSin);
		pDC->Polyline(Line, 2);
		Line[0].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffY * m_fCos * 0.55);
		Line[0].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffX * m_fSin * 0.55);
		Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffY * m_fCos * 0.45
						+ fNorm * m_fCoeffX * m_fSin * 0.03);
		Line[1].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffX * m_fSin * 0.45
						+ fNorm * m_fCoeffY * m_fCos * 0.03);
		pDC->Polyline(Line, 2);
		Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffY * m_fCos * 0.45
						- fNorm * m_fCoeffX * m_fSin * 0.03);
		Line[1].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffX * m_fSin * 0.45
						- fNorm * m_fCoeffY * m_fCos * 0.03);
   }
   else {
		Line[0].x = (int)(m_InitialOffset.x - fNorm * m_fCoeffY * m_fCos * 0.03);
		Line[0].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffX * m_fSin * 0.03);
		Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffY * m_fCos * 0.03);
		Line[1].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffX * m_fSin * 0.03);
   }
	pDC->Polyline(Line, 2);
	switch(m_nMarker) {
		case 0: nFlag = F_L1; break;
		case 2: nFlag = F_L2; break;
		case 4: nFlag = F_L3; break;
		default:nFlag = 0;
	}
   if(/*!(m_nMarker & 1) && (*/m_CalcDlg.GetEditFlags() & nFlag)
	{
		Line[0].x = (int)(m_InitialOffset.x - fNorm * m_fCoeffX * m_fSin);
		Line[0].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffY * m_fCos);
		Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffX * m_fSin);
		Line[1].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffY * m_fCos);
		pDC->Polyline(Line, 2);
		Line[0].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffX * m_fSin * 0.55);
		Line[0].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffY * m_fCos * 0.55);
		Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffX * m_fSin * 0.45
						+ fNorm * m_fCoeffY * m_fCos * 0.03);
		Line[1].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffY * m_fCos * 0.45
		            - fNorm * m_fCoeffX * m_fSin * 0.03);
		pDC->Polyline(Line, 2);
		Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffX * m_fSin * 0.45
						- fNorm * m_fCoeffY * m_fCos * 0.03);
		Line[1].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffY * m_fCos * 0.45
		            + fNorm * m_fCoeffX * m_fSin * 0.03);
	}
	else {
		Line[0].x = (int)(m_InitialOffset.x - fNorm * m_fCoeffX * m_fSin * 0.03);
		Line[0].y = (int)(m_InitialOffset.y - fNorm * m_fCoeffY * m_fCos * 0.03);
		Line[1].x = (int)(m_InitialOffset.x + fNorm * m_fCoeffX * m_fSin * 0.03);
		Line[1].y = (int)(m_InitialOffset.y + fNorm * m_fCoeffY * m_fCos * 0.03);
	}
	pDC->Polyline(Line, 2);
}

BOOL FitEllipseRect(CRect& rect)
{
	rect.NormalizeRect();
	int nWidth  = rect.Width();
	int nHeight = rect.Height();
	if(nWidth < 3 || nHeight < 3)
		return FALSE;
	if(nWidth > 32766)
		if(nHeight > 32766)
			return FALSE;
		else
		{
			if(abs(rect.right) > abs(rect.left))
				rect.right = rect.left + 32766;
			else
				rect.left = rect.right - 32766;
			int nDelta = int(double(nHeight) * (1. - sqrt(32766. / double(nWidth))) / 2.);
			rect.top += nDelta;
			rect.bottom -= nDelta;
		}
	else
		if(nHeight > 32766)
		{
			if(abs(rect.bottom) > abs(rect.top))
				rect.bottom = rect.top + 32766;
			else
				rect.bottom -= 32766;
			int nDelta = int(double(nWidth) * (1. - sqrt(32766. / double(nHeight))) / 2.);
			rect.left += nDelta;
			rect.right -= nDelta;
		}
	return TRUE;
}



const auto Epsilon = 1.e-6f;

void CTrajView::OnPaint()
{
	if (!m_CalcDlg.m_bValidTrajectory)
	{
        ValidateRect(NULL);
		return;
	}

	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(&rect);

    dc.IntersectClipRect(rect);

	rect.left += /*nDlgWidth +*/ 20;
	rect.top += 20;
	rect.right -= 12;
	rect.bottom -= 12;

	float fXmin,fXmax,fYmin,fYmax;
   if(m_CalcDlg.m_fTVD > 0) 
	{ 
		fYmin = 0.f; 
		fYmax = m_CalcDlg.m_fTVD; 
	}
	else 
	{ 
		fYmin = m_CalcDlg.m_fTVD; 
		fYmax = 0.f;
	}

	if(m_CalcDlg.m_fDisp > 0) 
	{ 
		fXmin = 0.f; 
		fXmax = m_CalcDlg.m_fDisp; 
	}
   else 
	{ 
		fXmin = m_CalcDlg.m_fDisp; 
		fXmax = 0.f;
	}


   float fX = 0.f;
   float fY = 0.f;

   float fCoeffX = 0.f;
   float fCoeffY = 0.f;
	
   CDC* pDC = &dc;

   CPoint Line[2];
	CPen Pen;
	Pen.CreatePen(PS_SOLID, 3, RGB(0,0,255));
	CPen* pOldPen = NULL;

   for(int bDraw = 0; bDraw < 2; bDraw++) 
	{
		if(bDraw)
		{
		//	Grid drawing
			//float fEvenStepX = GetStep(ToUnits_(fXmax - fXmin, UV_4_TRAJ_LENGTH) / 4);
			//float fEvenStepY = GetStep(ToUnits_(fYmax - fYmin, UV_4_TRAJ_LENGTH) / 4);
			//float fStepX = FromUnits_(fEvenStepX, UV_4_TRAJ_LENGTH);
			//float fStepY = FromUnits_(fEvenStepY, UV_4_TRAJ_LENGTH);
			float fEvenStepX = GetStep((fXmax - fXmin) / 4);
			float fEvenStepY = GetStep((fYmax - fYmin) / 4);
			float fStepX = fEvenStepX;
			float fStepY = fEvenStepY;
			if(m_CalcDlg.m_bIsotropic)
				fStepX = fStepY = max(fStepX, fStepY);

			float fOffsetX = fX * fCoeffX;
			while(fOffsetX + Epsilon >= 0)
				fOffsetX -= fStepX * fCoeffX;
			fOffsetX += rect.left;

			float fOffsetY = fY * fCoeffY;
			while(fOffsetY + Epsilon >= 0)
				fOffsetY -= fStepY * fCoeffY;
			fOffsetY += rect.top;
			
			CPen penGrid(PS_SOLID, 1, RGB(196, 196, 196));
			pOldPen = pDC->SelectObject(&penGrid);

			int nScaleLeft, nScaleRight,
				 nScaleTop, nScaleBottom;
			for(int i = 1;; i++)
			{
				int x = int(fOffsetX + fStepX * fCoeffX * i);
				if(x > rect.right)
					break;
				pDC->MoveTo(x, rect.bottom);
				pDC->LineTo(x, rect.top);
				switch(i)
				{
					case 1: nScaleLeft = x; break;
					case 2: nScaleRight = x; break;
				}
			}

			for(int i = 1;; i++)
			{
				int y = int(fOffsetY + fStepY * fCoeffY * i);
				if(y > rect.bottom)
					break;
				pDC->MoveTo(rect.left, y);
				pDC->LineTo(rect.right, y);
				switch(i)
				{
					case 1: nScaleTop = y; break;
					case 2: nScaleBottom = y; break;
				}
			}

			pDC->MoveTo(nScaleLeft,  rect.top - 8);
			pDC->LineTo(nScaleRight, rect.top - 8);
			pDC->MoveTo(nScaleLeft,  rect.top - 6);
			pDC->LineTo(nScaleLeft,  rect.top - 11);
			pDC->MoveTo(nScaleRight, rect.top - 6);
			pDC->LineTo(nScaleRight, rect.top - 11);

			pDC->MoveTo(rect.left - 8,  nScaleTop);
			pDC->LineTo(rect.left - 8,  nScaleBottom);
			pDC->MoveTo(rect.left - 6,  nScaleTop);
			pDC->LineTo(rect.left - 11, nScaleTop);
			pDC->MoveTo(rect.left - 6,  nScaleBottom);
			pDC->LineTo(rect.left - 11, nScaleBottom);

			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfHeight = 14;
			lf.lfWeight = 400;
			_tcscpy_s(lf.lfFaceName, _T("Arial"));

			CString strBuf;
			CString strUnit;// = GetLabel_(UV_4_TRAJ_LENGTH);

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
			CPen penAxes;
			penAxes.CreatePen(PS_SOLID, 0, RGB(0,0,0));
			pDC->SelectObject(&penAxes);

			CBrush* pOldBrush = 
   			pDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
			pDC->Rectangle(&rect);
			pDC->SelectObject(pOldBrush);
   
			DrawAxes(pDC);

			pDC->SelectObject(&Pen);
		}

		Line[0].x = (int)(rect.left +fX * fCoeffX);
	   Line[0].y = (int)(rect.top + fY * fCoeffY);
	   for(int i = 0; i < 3; i++) 
		{
	   	float sin0 = (float)sin(m_CalcDlg.m_c[i].Phi);
	   	float cos0 = (float)cos(m_CalcDlg.m_c[i].Phi);
			fX += m_CalcDlg.m_c[i].L * sin0;
			fY += m_CalcDlg.m_c[i].L * cos0;
		   Line[1].x = (int)(rect.left +fX * fCoeffX);
		   Line[1].y = (int)(rect.top + fY * fCoeffY);
		   m_Markers[i*2].Point.x = (Line[0].x + Line[1].x) / 2;
		   m_Markers[i*2].Point.y = (Line[0].y + Line[1].y) / 2;
		   m_Markers[i*2].fSin = sin0;
		   m_Markers[i*2].fCos = cos0;
	      if(bDraw) pDC->Polyline(Line, 2);
	      if(i < 2) {                
	      	float fR = m_CalcDlg.m_c[i].R;
	      	fX += fR * cos0;
	      	fY -= fR * sin0;
	      	float sin1 = (float)sin(m_CalcDlg.m_c[i + 1].Phi);
	      	float cos1 = (float)cos(m_CalcDlg.m_c[i + 1].Phi);
	      	if(!bDraw) {              
	      	   float fRabs = (float)fabs(fR);
	      	   float fAngle = m_CalcDlg.m_c[i + 1].Phi - m_CalcDlg.m_c[i].Phi;
	      	   while(fAngle > M_PI) fAngle -= (float)(2 * M_PI);
	      	   while(fAngle < -M_PI) fAngle += (float)(2 * M_PI);
	      	   BOOL bIsHalfTurn = (fAngle * fR < 0);
               BOOL bExtendX = FALSE;
               BOOL bExtendY = FALSE;
	      		int nMayBeLoop = 0;
					if(sin0 <= 0 && sin1 >= 0) {
						fXmin = fmin(fXmin, fX - fRabs);
						if(bIsHalfTurn) bExtendY = TRUE;
					}	
					else if(sin0 >= 0 && sin1 <= 0) {
						fXmax = fmax(fXmax, fX + fRabs);
						if(bIsHalfTurn) bExtendY = TRUE;
					}	
               else nMayBeLoop++;

					if(cos0 <= 0 && cos1 >= 0) {
						fYmin = fmin(fYmin, fY - fRabs);
						if(bIsHalfTurn) bExtendX = TRUE;
					}	
					else if(cos0 >= 0 && cos1 <= 0) {
						fYmax = fmax(fYmax, fY + fRabs);
						if(bIsHalfTurn) bExtendX = TRUE;
					}	
					else nMayBeLoop++;
					if(!bIsHalfTurn) nMayBeLoop = 0;
					
					if(nMayBeLoop == 2 || bExtendX) {
							fXmin = fmin(fXmin, fX - fRabs);                          
							fXmax = fmax(fXmax, fX + fRabs);
					}		
					if(nMayBeLoop == 2 || bExtendY) {
							fYmin = fmin(fYmin, fY - fRabs);                          
							fYmax = fmax(fYmax, fY + fRabs);
					}
	      	}
	         CRect bound;
	      	bound.left = (int)(rect.left + (fX - fR) * fCoeffX);
	      	bound.top = (int)(rect.top + (fY - fR) * fCoeffY);
	      	bound.right = (int)(rect.left + (fX + fR) * fCoeffX);
	      	bound.bottom = (int)(rect.top + (fY + fR) * fCoeffY);
	      	
	      	fX -= fR * cos1;
	      	fY += fR * sin1;
				
			   Line[0].x = (int)(rect.left + fX * fCoeffX);
			   Line[0].y = (int)(rect.top + fY * fCoeffY);
			   m_Markers[i*2 + 1].Point.x = (Line[0].x + Line[1].x) / 2;
			   m_Markers[i*2 + 1].Point.y = (Line[0].y + Line[1].y) / 2;
			   m_Markers[i*2 + 1].fSin = 
			   	(float)sin((m_CalcDlg.m_c[i].Phi + m_CalcDlg.m_c[i + 1].Phi) / 2);
			   m_Markers[i*2 + 1].fCos =
			   	(float)cos((m_CalcDlg.m_c[i].Phi + m_CalcDlg.m_c[i + 1].Phi) / 2);

			   if(bDraw && m_CalcDlg.m_c[i+1].Phi != m_CalcDlg.m_c[i].Phi &&
			   	Line[0] != Line[1] && fR != 0) {
			   	BOOL bArcOK = FitEllipseRect(bound);
					if(bArcOK)
			   		if(fR<0)  
							bArcOK = pDC->Arc(bound, Line[0], Line[1]);
						else  
							bArcOK = pDC->Arc(bound, Line[1], Line[0]);
					if(!bArcOK) pDC->Polyline(Line,2);
				}								             
	      }
	   }
	   fX = -fXmin;
	   fY = -fYmin;
		fCoeffX = (rect.Width()) / (fXmax - fXmin);
		fCoeffY = (rect.Height()) / (fYmax - fYmin);
      if(m_CalcDlg.m_bIsotropic)
      	if(fCoeffX < fCoeffY) fCoeffY = fCoeffX;
      	else fCoeffX = fCoeffY;
   }
   pDC->SelectObject(pOldPen);
   if(m_State != TVS_DRAGGING) {
		m_fCoeffX = fCoeffX;
		m_fCoeffY = fCoeffY;
   	for(int i = 0; i < GetMarkerNumber(); i++) 
			DrawMarker(pDC, m_Markers[i].Point);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CTrajView message handlers
 
void CTrajView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_State == TVS_ONMARKER) {
		SetCapture();
		m_State = TVS_DRAGGING;
		m_InitialOffset.x = point.x;// - m_Markers[m_nMarker].Point.x;
		m_InitialOffset.y = point.y;// - m_Markers[m_nMarker].Point.y;
		m_fSin = m_Markers[m_nMarker].fSin;
		m_fCos = m_Markers[m_nMarker].fCos;
		switch(m_nMarker) {
			case 0:  m_fSavePhi = m_CalcDlg.m_fPhi1;
						m_fSaveL = m_CalcDlg.m_fL1;
						break;
			case 1:  m_fSaveBR = m_CalcDlg.m_fBR1;
			         break;
			case 2:  m_fSavePhi = m_CalcDlg.m_fPhi2;
						m_fSaveL = m_CalcDlg.m_fL2;
						break;
			case 3:  m_fSaveBR = m_CalcDlg.m_fBR2;
						break;
			case 4:  m_fSavePhi = m_CalcDlg.m_fPhi3;
						m_fSaveL = m_CalcDlg.m_fL3;
						m_fSin = -m_fSin; m_fCos = -m_fCos; // Rotate axes on 180 degr.
						break;
		}
	}
}

void CTrajView::OnMouseMove(UINT nFlags, CPoint point)
{
	int nMarker;
   float fSaveL, fSaveBR, fSavePhi;
   if(m_CalcDlg.IsWorkingMode() && m_CalcDlg.m_bValidTrajectory) {
		BOOL bOnMarker = FALSE;
	   for(nMarker = 0; nMarker < GetMarkerNumber(); nMarker++) 
		{
	      switch(nMarker) {
				case 0:if(!(m_CalcDlg.GetEditFlags() & (F_L1 | F_PHI1)))
							continue; 
						break;
				case 1:if(!(m_CalcDlg.GetEditFlags() & F_R1)) 
							 continue; 
						break;
				case 2:if(!(m_CalcDlg.GetEditFlags() & (F_L2 | F_PHI2))) 
							 continue; 
						break;
				case 3:if(!(m_CalcDlg.GetEditFlags() & F_R2))
							 continue; 
						break;
				case 4:if(!(m_CalcDlg.GetEditFlags() & (F_L3 | F_PHI3)))
							 continue; 
						break;
	      }
			if(IsMarkerHere(m_Markers[nMarker].Point, point)) 
			{
				bOnMarker = TRUE;
				break;
			}
	   } 
		switch(m_State) 
		{
			case TVS_NONE: 
					if(bOnMarker) {
						SetCursor(LoadCursor(NULL, IDC_CROSS));
						m_State = TVS_ONMARKER;
						m_nMarker = nMarker;
					} break;
			case TVS_ONMARKER: 
					if(!bOnMarker) {
						SetCursor(LoadCursor(NULL, IDC_ARROW));
						m_State = TVS_NONE;
					} break;
			case TVS_DRAGGING:
               float fSensitivity = (float)exp(0.3 * (float)(m_CalcDlg.m_Sensitivity.GetPos() -10));
			      float x = fSensitivity * (float)(point.x - m_InitialOffset.x);// * sqrt(m_fCoeffY / m_fCoeffX);// - m_Markers[m_nMarker].Point.x;
			      float y = fSensitivity * (float)(point.y - m_InitialOffset.y);// * sqrt(m_fCoeffX / m_fCoeffY);// - m_Markers[m_nMarker].Point.y;
               float fConv = (float)sqrt(m_fCoeffX / m_fCoeffY);

			      float fLong = y * m_fCos / fConv + 
			                    x * m_fSin * fConv;  
			      float fTrans =x * m_fCos / fConv - 
			      				  y * m_fSin * fConv;
			      				  
//			      float fLong = y * m_fCos / m_fCoeffX + 
//			                    x * m_fSin / m_fCoeffY;  
//			      float fTrans =x * m_fCos / m_fCoeffX - 
//			      				  y * m_fSin / m_fCoeffY;

				  const auto fCoeffPhi = (float)0.5;
				  const auto fCoeffL = (float)1.;
				  const auto fCoeffBR = (float)1.;

					BOOL bAllowed = TRUE;
		         switch(m_nMarker) 
					{
						case 0:
								fSavePhi = m_CalcDlg.m_fPhi1;
								fSaveL = m_CalcDlg.m_fL1;
								if(m_CalcDlg.GetEditFlags() & F_PHI1)
									m_CalcDlg.m_fPhi1 = m_fSavePhi + fTrans * fCoeffPhi;
								if(m_CalcDlg.GetEditFlags() & F_L1)
									m_CalcDlg.m_fL1 = m_fSaveL + fLong * fCoeffL;
								if(!m_CalcDlg.TryToApply()) {
									m_CalcDlg.m_fPhi1 = fSavePhi;
									m_CalcDlg.m_fL1 = fSaveL;
									bAllowed = FALSE;
								} break;
						case 1:
								fSaveBR = m_CalcDlg.m_fBR1;
								m_CalcDlg.m_fBR1 = m_fSaveBR + fTrans * fCoeffBR;
								if(!m_CalcDlg.TryToApply()) {
									m_CalcDlg.m_fBR1 = fSaveBR;
									bAllowed = FALSE;
								}
								break;
						case 2:
								fSavePhi = m_CalcDlg.m_fPhi2;
								fSaveL = m_CalcDlg.m_fL2;
								if(m_CalcDlg.GetEditFlags() & F_PHI2)
									m_CalcDlg.m_fPhi2 = m_fSavePhi + fTrans * fCoeffPhi;
								if(m_CalcDlg.GetEditFlags() & F_L2)
									m_CalcDlg.m_fL2 = m_fSaveL + fLong * fCoeffL;
								if(!m_CalcDlg.TryToApply()) {
									m_CalcDlg.m_fPhi2 = fSavePhi;
									m_CalcDlg.m_fL2 = fSaveL;
									bAllowed = FALSE;
								} break;
						case 3:
								fSaveBR = m_CalcDlg.m_fBR2;
								m_CalcDlg.m_fBR2 = m_fSaveBR + fTrans * fCoeffBR;
								if(!m_CalcDlg.TryToApply()) {
									m_CalcDlg.m_fBR2 = fSaveBR;
									bAllowed = FALSE;
								}
								break;
						case 4:
								fSavePhi = m_CalcDlg.m_fPhi3;
								fSaveL = m_CalcDlg.m_fL3;
								if(m_CalcDlg.GetEditFlags() & F_PHI3)
									m_CalcDlg.m_fPhi3 = m_fSavePhi + fTrans * fCoeffPhi;
								if(m_CalcDlg.GetEditFlags() & F_L3)
									m_CalcDlg.m_fL3 = m_fSaveL + fLong * fCoeffL;
								if(!m_CalcDlg.TryToApply()) {
									m_CalcDlg.m_fPhi3 = fSavePhi;
									m_CalcDlg.m_fL3 = fSaveL;
									bAllowed = FALSE;
								} break;
		         }
               if(bAllowed) SetCursor(LoadCursor(NULL, IDC_CROSS));
					else 
						SetCursor(AfxGetApp()->LoadCursor((LPCTSTR)IDC_NOACTION));
					break;
		}
   }
	__super::OnMouseMove(nFlags, point);
}

BOOL CTrajView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(m_State == TVS_ONMARKER) {
		SetCursor(LoadCursor(NULL, IDC_CROSS));
		return TRUE;
	}
	return __super::OnSetCursor(pWnd, nHitTest, message);
}

void CTrajView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_State == TVS_DRAGGING) {
		ReleaseCapture();
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		m_State = TVS_NONE;
		DestroyCursor(SetCursor(AfxGetApp()->LoadCursor((LPCTSTR)IDC_NOACTION)));
		InvalidateRect(NULL);//&m_CalcDlg.UpdateRect);
	}
}

int CTrajView::GetSegNumber() 
{ 
	return m_CalcDlg.GetTrajectoryType() + 2; 
}

int CTrajView::GetMarkerNumber()
{
	switch(m_CalcDlg.GetTrajectoryType())
	{
		case tkJTrajectory:
		case tkExtendedJTrajectory:
			return 3;
		case tkSTrajectory:
		case tkExtendedSTrajectory:
			return 5;
		default: 
			ASSERT(0); return 0;
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
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),
		PATCOPY);
	pDC->SelectObject(pPrevBrush);
	return TRUE;
}
