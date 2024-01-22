#ifndef __PlaneTrajMath_h__
#define __PlaneTrajMath_h__

//#include "CmnUtils/Constant.h"

#define M_TURN		 1
#define M_ROT_DOWN 2
#define M_ROT_UP	 4

#define F_L1	0x0080
#define F_PHI1	0x0040
#define F_R1	0x0020
#define F_L2	0x0010
#define F_PHI2	0x0008
#define F_R2	0x0004
#define F_L3	0x0002
#define F_PHI3	0x0001

struct PLANE_TRAJ_CELL
{
	float L, Phi, R;
};

typedef PLANE_TRAJ_CELL PLANE_TRAJ_ARR[3];

class CPlaneTrajMath
{
public:
	CPlaneTrajMath() 
	{
		m_c = NULL;
		m_fTVD = 0.f;
		m_fDisp = 0.f;
	};
	void SetData(PLANE_TRAJ_ARR c, float fTVD, float fDisp );
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
   PLANE_TRAJ_CELL* m_c;
   float sin0, sin1, sin2,
   		cos0, cos1, cos2,
   		fA, fB, fC, fD, fE, fF, fG, fH,
   		m_fTVD, m_fDisp;
};

#endif// __PlaneTrajMath_h__
