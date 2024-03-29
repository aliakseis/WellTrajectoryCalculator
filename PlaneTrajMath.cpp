#include "pch.h"

#include "PlaneTrajMath.h"

////////////////////////////////////////////////
//	Math methods

namespace
{

void Turn(PLANE_TRAJ_ARR& m_c)
{
    double fBuf = m_c[0].L;
    m_c[0].L = m_c[2].L;
    m_c[2].L = fBuf;
    fBuf = m_c[0].Phi;
    m_c[0].Phi = m_c[2].Phi;
    m_c[2].Phi = fBuf;
    fBuf = m_c[0].R;
    m_c[0].R = -m_c[1].R;
    m_c[1].R = -fBuf;
}

void RotateDown(PLANE_TRAJ_ARR& m_c)
{
    PLANE_TRAJ_CELL
    Buf = m_c[2];
    m_c[2] = m_c[1];
    m_c[1] = m_c[0];
    m_c[0] = Buf;
}

void RotateUp(PLANE_TRAJ_ARR& m_c)
{
    PLANE_TRAJ_CELL
    Buf = m_c[0];
    m_c[0] = m_c[1];
    m_c[1] = m_c[2];
    m_c[2] = Buf;
}

double GetRoot(double fAsin, double fAtan, int i)
{
    return ((0 != i) ^ (fabs(M_PI - fAsin - fAtan) > fabs(fAsin - fAtan))) ? fAsin - fAtan : M_PI - fAsin - fAtan;
}

}  // namespace

void CPlaneTrajMath::Direct(int nFlags)
{
    ASSERT(m_c[2].R == 0);
    if (nFlags & M_TURN)
        Turn(m_c);
    if (nFlags & M_ROT_DOWN)
        RotateDown(m_c);
    else if (nFlags & M_ROT_UP)
        RotateUp(m_c);

    sin0 = sin(m_c[0].Phi);
    sin1 = sin(m_c[1].Phi);
    sin2 = sin(m_c[2].Phi);

    cos0 = cos(m_c[0].Phi);
    cos1 = cos(m_c[1].Phi);
    cos2 = cos(m_c[2].Phi);
}

void CPlaneTrajMath::Inverse(int nFlags)
{
    if (nFlags & M_ROT_DOWN)
        RotateUp(m_c);
    else if (nFlags & M_ROT_UP)
        RotateDown(m_c);
    if (nFlags & M_TURN)
        Turn(m_c);
    ASSERT(m_c[2].R == 0);
}

double CPlaneTrajMath::GetTVDError()
{
    return fabs(m_c[0].L * cos0 + m_c[0].R * (sin1 - sin0) + m_c[1].L * cos1 + m_c[1].R * (sin2 - sin1) +
                m_c[2].L * cos2 + m_c[2].R * (sin0 - sin2) - m_fTVD);
}

double CPlaneTrajMath::GetDispError()
{
    return fabs(m_c[0].L * sin0 + m_c[0].R * (cos0 - cos1) + m_c[1].L * sin1 + m_c[1].R * (cos1 - cos2) +
                m_c[2].L * sin2 + m_c[2].R * (cos2 - cos0) - m_fDisp);
}

//  A * x + B * y = C
//  D * x + E * y = F
//  x => A    y => B

bool CPlaneTrajMath::FindSolution2x2()
{
    double fMod = fB * fD - fE * fA;
    if (fabs(fMod) < 1e-10)
        return false;
    double fBuf = (fB * fF - fC * fE) / fMod;
    fB = (fC * fD - fF * fA) / fMod;
    fA = fBuf;
    return true;
}

// B = C * sin(Phi) + D * cos(Phi)
// A <= Phi = arcsin(B / sqrt(C * C + D * D)) - arctg(D / C)

void CPlaneTrajMath::FindAngle()
{
    double fArg = fB / sqrt(fC * fC + fD * fD);
    double fBuf = atan2(fD, fC);
    if (fBuf > -M_PI / 2)
        if (fBuf < M_PI / 2)
            fA = asin(fArg) - fBuf;
        else
            fA = double(M_PI - asin(fArg) - fBuf);
    else
        fA = double(-M_PI - asin(fArg) - fBuf);
}

//	A * x + B * sin(Phi) + C * cos(Phi) = D
//	E * x + F * sin(Phi) + G * cos(Phi) = H
// Phi => A		x => B

bool CPlaneTrajMath::SimpleTrigonometric()
{
    double fLeft = fB * fE - fF * fA;
    if (fabs(fLeft) < 1e-10)
        return false;
    double fRight = fC * fE - fG * fA;
    double fArg = (fD * fE - fH * fA) / sqrt(fLeft * fLeft + fRight * fRight);
    if (fArg < -1 || fArg > 1)
        return false;
    double fBuf = atan2(fRight, fLeft);
    if (fBuf > -M_PI / 2)
        if (fBuf < M_PI / 2)
            fA = asin(fArg) - fBuf;
        else
            fA = double(M_PI - asin(fArg) - fBuf);
    else
        fA = double(-M_PI - asin(fArg) - fBuf);
    fB = ((fC * fF - fG * fB) * cos(fA) + fH * fB - fD * fF) / fLeft;
    return true;
}

//	A * cos(Phi) - (x - B) * sin(Phi) + C * x = D
// A * sin(Phi) + (x - B) * cos(Phi) - E * x = F
// Phi => A 	x => B

bool CPlaneTrajMath::PuzzleTrigonometric()
{
    double fLeft = fA * fC + fB * fE + fF;
    if (fabs(fLeft) < 1e-10)
        return false;
    double fRight = fD - fB * fC + fA * fE;
    double fArg = (fA + fD * fE + fF * fC) / sqrt(fLeft * fLeft + fRight * fRight);
    if (fArg < -1 || fArg > 1)
        return false;
    double fBuf = atan2(fRight, fLeft);
    double fAngle = asin(fArg) - fBuf;
    double fSin = sin(fAngle);
    double fCos = cos(fAngle);
    if (fabs(fE - fCos) < 1e-4 && fabs(fC - fSin) < 1e-4)
    {
        fAngle = double(M_PI - asin(fArg) - fBuf);
        if (fAngle > M_PI)
            fAngle -= double(2 * M_PI);
        fSin = sin(fAngle);
        fCos = cos(fAngle);
    }
    if (fabs(fE - fCos) > fabs(fC - fSin))
        fB = (fA * fSin - fB * fCos - fF) / (fE - fCos);
    else
        fB = (fD - fA * fCos - fB * fSin) / (fC - fSin);
    fA = fAngle;
    return true;
}

// Main Equations :
// TVD = L0 * cos0 + R0 * (sin1 - sin0) +
//       L1 * cos1 + R1 * (sin2 - sin1) +
//			L2 * cos2 + R2 * (sin0 - sin2)

// Disp= L0 * sin0 + R0 * (cos0 - cos1) +
//       L1 * sin1 + R1 * (cos1 - cos2) +
//			L2 * sin2 + R2 * (cos2 - cos0)

//	Find: L0, R0
bool CPlaneTrajMath::CalcLwhereR(int nFlags)
{
    Direct(nFlags);

    fA = cos0;
    fB = sin1 - sin0;
    fC = m_fTVD - m_c[1].L * cos1 - m_c[1].R * (sin2 - sin1) - m_c[2].L * cos2 - m_c[2].R * (sin0 - sin2);
    fD = sin0;
    fE = cos0 - cos1;
    fF = m_fDisp - m_c[1].L * sin1 - m_c[1].R * (cos1 - cos2) - m_c[2].L * sin2 - m_c[2].R * (cos2 - cos0);
    bool bResult = FindSolution2x2();
    if (bResult)
    {
        m_c[0].L = fA;
        m_c[0].R = fB;
    }
    Inverse(nFlags);
    return bResult;
}

// Find: L0, R1
bool CPlaneTrajMath::CalcLltR(int nFlags)
{
    Direct(nFlags);

    fA = cos0;
    fB = sin2 - sin1;
    fC = m_fTVD - m_c[1].L * cos1 - m_c[0].R * (sin1 - sin0) - m_c[2].L * cos2 - m_c[2].R * (sin0 - sin2);
    fD = sin0;
    fE = cos1 - cos2;
    fF = m_fDisp - m_c[1].L * sin1 - m_c[0].R * (cos0 - cos1) - m_c[2].L * sin2 - m_c[2].R * (cos2 - cos0);
    bool bResult = FindSolution2x2();
    if (bResult)
    {
        m_c[0].L = fA;
        m_c[1].R = fB;
    }
    Inverse(nFlags);
    return bResult;
}

bool CPlaneTrajMath::CalcLL(int nFlags)
{
    Direct(nFlags);

    fA = cos0;
    fB = cos1;
    fC = m_fTVD - m_c[0].R * (sin1 - sin0) - m_c[1].R * (sin2 - sin1) - m_c[2].R * (sin0 - sin2) - m_c[2].L * cos2;
    fD = sin0;
    fE = sin1;
    fF = m_fDisp - m_c[0].R * (cos0 - cos1) - m_c[1].R * (cos1 - cos2) - m_c[2].R * (cos2 - cos0) - m_c[2].L * sin2;
    bool bResult = FindSolution2x2();
    if (bResult)
    {
        m_c[0].L = fA;
        m_c[1].L = fB;
    }
    Inverse(nFlags);
    return bResult;
}

bool CPlaneTrajMath::CalcRR()
{
    Direct(0);
    fA = sin1 - sin0;
    fB = sin2 - sin1;
    fC = m_fTVD - m_c[0].L * cos0 - m_c[1].L * cos1 - m_c[2].L * cos2;
    fD = cos0 - cos1;
    fE = cos1 - cos2;
    fF = m_fDisp - m_c[0].L * sin0 - m_c[1].L * sin1 - m_c[2].L * sin2;

    bool bResult = FindSolution2x2();
    if (bResult)
    {
        m_c[0].R = fA;
        m_c[1].R = fB;
    }
    return bResult;
}

//	Find: L0, Phi0
bool CPlaneTrajMath::CalcLwherePhi(int nFlags)
{
    Direct(nFlags);

    fB = m_c[2].R - m_c[0].R;
    fC = m_fTVD - m_c[0].R * sin1 - m_c[1].L * cos1 - m_c[1].R * (sin2 - sin1) - m_c[2].L * cos2 + m_c[2].R * sin2;
    fD = -(m_fDisp + m_c[0].R * cos1 - m_c[1].L * sin1 - m_c[1].R * (cos1 - cos2) - m_c[2].L * sin2 - m_c[2].R * cos2);

    double fArg = fB / sqrt(fC * fC + fD * fD);
    if (fabs(fArg) > 1.)
        return false;
    double fBuf = atan2(fD, fC);

    double fPhi[2], fL[2], fTVDErr[2], fDispErr[2];
    fPhi[0] = asin(fArg) - fBuf;
    fPhi[1] = (M_PI - asin(fArg) - fBuf);
    for (int i = 0; i < 2; i++)
    {
        while (fPhi[i] > M_PI) fPhi[i] -= double(2 * M_PI);
        while (fPhi[i] < -M_PI) fPhi[i] += double(2 * M_PI);
        if (fPhi[i] > -M_PI / 4 && fPhi[i] < M_PI / 4)
            fL[i] = ((fC - fB * sin(fPhi[i])) / cos(fPhi[i]));
        else
            fL[i] = ((-fD + fB * cos(fPhi[i])) / sin(fPhi[i]));
    }
    m_c[0].Phi = fPhi[0];
    sin0 = sin(fPhi[0]);
    cos0 = cos(fPhi[0]);
    m_c[0].L = fL[0];
    fTVDErr[0] = GetTVDError();
    fDispErr[0] = GetDispError();

    m_c[0].Phi = fPhi[1];
    sin0 = sin(fPhi[1]);
    cos0 = cos(fPhi[1]);
    m_c[0].L = fL[1];
    fTVDErr[1] = GetTVDError();
    fDispErr[1] = GetDispError();

    //   if((fTVDErr[1]>fTVDErr[0])^(fDispErr[1]>fDispErr[0])) return false;
    if (fL[1] < 0 || ((fTVDErr[1] > fTVDErr[0]) && (fDispErr[1] > fDispErr[0]) && fL[0] >= 0))
    {
        if (fTVDErr[0] > 0.1 || fDispErr[0] > 0.1)
            return false;
        m_c[0].Phi = fPhi[0];
        m_c[0].L = fL[0];
    }
    else if (fTVDErr[1] > 0.1 || fDispErr[1] > 0.1)
        return false;

    /*
            if(fBuf > -M_PI/2)
               if(fBuf < M_PI/2) fA = asin(fArg) - fBuf;
               else fA = M_PI - asin(fArg) -fBuf;
            else fA = -M_PI - asin(fArg) -fBuf;

       m_c[0].Phi = fA;
       if(fA > -M_PI/4 && fA < M_PI/4)
                    m_c[0].L = (fC - fB * sin(fA)) / cos(fA);
       else
                    m_c[0].L = (-fD + fB * cos(fA)) / sin(fA);
    */
    Inverse(nFlags);
    return true;
}

//	Find: L0, Phi1
bool CPlaneTrajMath::CalcLltPhi(int nFlags)
{
    Direct(nFlags);

    fA = cos0;
    fB = m_c[0].R - m_c[1].R;
    fC = m_c[1].L;
    fD = m_fTVD + m_c[0].R * sin0 - m_c[1].R * sin2 - m_c[2].L * cos2 - m_c[2].R * (sin0 - sin2);
    fE = sin0;
    fF = m_c[1].L;
    fG = -m_c[0].R + m_c[1].R;
    fH = m_fDisp - m_c[0].R * cos0 + m_c[1].R * cos2 - m_c[2].L * sin2 - m_c[2].R * (cos2 - cos0);
    bool bResult = SimpleTrigonometric();
    if (bResult)
    {
        m_c[1].Phi = fA;
        m_c[0].L = fB;
    }
    Inverse(nFlags);
    return bResult;
}

// Main Equations :
// TVD = L0 * cos0 + R0 * (sin1 - sin0) +
//       L1 * cos1 + R1 * (sin2 - sin1) +
//			L2 * cos2 + R2 * (sin0 - sin2)

// Disp= L0 * sin0 + R0 * (cos0 - cos1) +
//       L1 * sin1 + R1 * (cos1 - cos2) +
//			L2 * sin2 + R2 * (cos2 - cos0)

//	Find: R0, Phi0
bool CPlaneTrajMath::CalcRwherePhi(int nFlags)
{
    Direct(nFlags);

    fA = m_c[0].L;
    fB = m_c[2].R;
    fC = sin1;
    fD = m_fTVD - m_c[1].L * cos1 - m_c[1].R * (sin2 - sin1) - m_c[2].L * cos2 + m_c[2].R * sin2;
    fE = cos1;
    fF = m_fDisp - m_c[1].L * sin1 - m_c[1].R * (cos1 - cos2) - m_c[2].L * sin2 - m_c[2].R * cos2;

    bool bResult = PuzzleTrigonometric();
    if (bResult)
    {
        m_c[0].Phi = fA;
        m_c[0].R = fB;
    }
    Inverse(nFlags);
    return bResult;
}

//	Find: R1, Phi0
bool CPlaneTrajMath::CalcRgtPhi(int nFlags)
{
    Direct(nFlags);

    fA = sin2 - sin1;
    fB = m_c[2].R - m_c[0].R;
    fC = m_c[0].L;
    fD = m_fTVD - m_c[1].L * cos1 - m_c[0].R * sin1 - m_c[2].L * cos2 + m_c[2].R * sin2;
    fE = cos1 - cos2;
    fF = m_c[0].L;
    fG = m_c[0].R - m_c[2].R;
    fH = m_fDisp - m_c[1].L * sin1 + m_c[0].R * cos1 - m_c[2].L * sin2 - m_c[2].R * cos2;

    bool bResult = SimpleTrigonometric();
    if (bResult)
    {
        m_c[0].Phi = fA;
        m_c[1].R = fB;
    }
    Inverse(nFlags);
    return bResult;
}

// Main Equations :
// TVD = L0 * cos0 + R0 * (sin1 - sin0) +
//       L1 * cos1 + R1 * (sin2 - sin1) +
//			L2 * cos2 + R2 * (sin0 - sin2)

// Disp= L0 * sin0 + R0 * (cos0 - cos1) +
//       L1 * sin1 + R1 * (cos1 - cos2) +
//			L2 * sin2 + R2 * (cos2 - cos0)

//	A * cos(Phi0) - B * sin(Phi0) + C * cos(Phi1) - D * sin(Phi1) = E
//	A * cos(Phi0) + B * sin(Phi0) + C * cos(Phi1) + D * sin(Phi1) = F

//	Find: Phi0, Phi1
bool CPlaneTrajMath::CalcPhiPhi(int nFlags)
{
    int i;
    Direct(nFlags);

    fA = m_c[0].L;
    fB = m_c[0].R - m_c[2].R;
    fC = m_c[1].L;
    fD = m_c[1].R - m_c[0].R;
    fE = m_fTVD - m_c[1].R * sin2 - m_c[2].L * cos2 + m_c[2].R * sin2;
    fF = m_fDisp + m_c[1].R * cos2 - m_c[2].L * sin2 - m_c[2].R * cos2;
    double fLeft = fA * fD - fB * fC;
    double fRight = fB * fD + fA * fC;
    double fArg =
        (fE * fE + fF * fF - fA * fA - fB * fB - fC * fC - fD * fD) / 2. / sqrt(fLeft * fLeft + fRight * fRight);
    if (fabs(fArg) > 1)
        return false;
    fArg = asin(fArg);
    double fAtan = atan2(fRight, fLeft);
    bool bResult = false;
    for (i = 0; i < 2; i++)
    {
        double fDelta = -GetRoot(fArg, fAtan, i);
        while (fDelta < -M_PI) fDelta += double(2 * M_PI);
        while (fDelta > M_PI) fDelta -= double(2 * M_PI);
        if (fDelta * m_c[0].R < 0)
            continue;
        double fSin = sin(fDelta);
        double fCos = cos(fDelta);
        double fLeft1 = -fB - fC * fSin - fD * fCos;
        double fRight1 = fA + fC * fCos - fD * fSin;
        double fArg1 = fE / sqrt(fLeft1 * fLeft1 + fRight1 * fRight1);
        if (fabs(fArg1) > 1)
            continue;
        fArg1 = asin(fArg1);
        double fAtan1 = atan2(fRight1, fLeft1);
        for (int j = 0; j < 2; j++)
        {
            m_c[0].Phi = GetRoot(fArg1, fAtan1, j);
            while (m_c[0].Phi < -M_PI) m_c[0].Phi += double(2 * M_PI);
            while (m_c[0].Phi > M_PI) m_c[0].Phi -= double(2 * M_PI);
            sin0 = sin(m_c[0].Phi);
            cos0 = cos(m_c[0].Phi);
            m_c[1].Phi = m_c[0].Phi + fDelta;
            while (m_c[1].Phi < -M_PI) m_c[1].Phi += double(2 * M_PI);
            while (m_c[1].Phi > M_PI) m_c[1].Phi -= double(2 * M_PI);
            sin1 = sin(m_c[1].Phi);
            cos1 = cos(m_c[1].Phi);
            if (GetTVDError() < 0.1 && GetDispError() < 0.1)
            {
                bResult = true;
                goto L1;
            }
        }
    }
L1:
    Inverse(nFlags);
    return bResult;
}
