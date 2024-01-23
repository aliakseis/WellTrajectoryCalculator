#ifndef __PlaneTrajMath_h__
#define __PlaneTrajMath_h__

#include <array>

enum {
	M_TURN = 1,
	M_ROT_DOWN = 2,
	M_ROT_UP = 4
};

enum {
	F_L1 = 0x0080,
	F_PHI1 = 0x0040,
	F_R1 = 0x0020,
	F_L2 = 0x0010,
	F_PHI2 = 0x0008,
	F_R2 = 0x0004,
	F_L3 = 0x0002,
	F_PHI3 = 0x0001
};

struct PLANE_TRAJ_CELL
{
	float L, Phi, R;
};

typedef std::array<PLANE_TRAJ_CELL, 3> PLANE_TRAJ_ARR;

class CPlaneTrajMath
{
public:
	CPlaneTrajMath(PLANE_TRAJ_ARR& c, float fTVD, float fDisp)
        : m_c(c), m_fTVD(fTVD), m_fDisp(fDisp)
	{
	}
	void Direct(int nFlags);
	void Inverse(int nFlags);
	float GetTVDError();
	float GetDispError();

	BOOL FindSolution2x2();
	void FindAngle();
	BOOL SimpleTrigonometric();
	BOOL PuzzleTrigonometric();

	BOOL CalcLwhereR(int nFlags);
	BOOL CalcLltR(int nFlags);
	BOOL CalcLL(int nFlags);
	BOOL CalcRR();
	BOOL CalcLwherePhi(int nFlags);
	BOOL CalcLltPhi(int nFlags);
	BOOL CalcRwherePhi(int nFlags);
	BOOL CalcRgtPhi(int nFlags);
	BOOL CalcPhiPhi(int nFlags);

private:
	PLANE_TRAJ_ARR& m_c;
   float sin0, sin1, sin2,
   		cos0, cos1, cos2,
   		fA, fB, fC, fD, fE, fF, fG, fH,
   		m_fTVD, m_fDisp;
};

#endif// __PlaneTrajMath_h__
