#pragma once

#include "Base.h"

/* 애니메이션 하나의 정보를 ㄹ표현하귕한 클래스 .*/

BEGIN(Engine)
 
class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT NativeConstruct(_double TickPerSecond, _double Duration);

public:
	void Add_AnimBones(class CAnimBone* pAnimBones) {
		m_AnimBones.push_back(pAnimBones);
	}

public:/* For.Get_Funtion */
	_bool Get_IsFinished() {
		return m_isFinished;
	}
	_bool Get_NextFinish() {
		return m_bNextFinish;
	}
	_double Get_Duration() {
		return m_Duration;
	}
	_double Get_BlendSpeed() {
		return m_lfBlendSpeed;
	}
	_double Get_AnimSpeed() {
		return m_lfAnimSpeed;
	}
	_uint	Get_FrameCurrentIndex() {
		return m_iCurrentKeyFrame;
	}
public:/* For.Set_AnimSpeed */
	void Set_BlendSpeed(_double lfBlendSpeed) {
		m_lfBlendSpeed = lfBlendSpeed;
	}

	void Set_AnimSpeed(_double lfAnimSpeed) {
		m_lfAnimSpeed = lfAnimSpeed;
	}

	void Set_NextFinish(_bool _bCheck) {
		m_bNextFinish = _bCheck;
	}

	void Set_IsLoop(_bool _bCheck, _uint _iMaxTickCount) {
		m_bIsLoop = _bCheck;
		m_iMaxTickCount = _iMaxTickCount;
		m_iTickCount = 0;
	}

public:/* For.Reset_Animation */
	void Reset_Animation();
	void Reset_Animation_KeyFrame();

public:/* For.Update_AnimBone */
	void Update_AnimBoneTransformationMatrix(_double TimeDelta);
	void Update_AnimBoneTransformationMatrix_Finishied(_double TimeDelta);
	void Update_NextAnimBoneTransformationMatrix(_double TimeDelta);
	_bool Update_AnimBoneTransformationMatrix_Lefp(_double _dTimeDelta, CAnimation* _pDestAnimation);
public:/* For.Set_KeyFrameUpdate */
	void Set_KeyFrameUpdate(_double TimeDelta, _uint iSetKeyFrameIndex, _bool* bGetTheFinishied);

	

private:
	vector<class CAnimBone*>				m_AnimBones;
	typedef vector<class CAnimBone*>		ANIMBONES;


private:
	_double						m_TimeAcc = 0.0;
	_double						m_TickPerSecond = 0.0;
	_double						m_Duration = 0.0;
	_bool						m_isFinished = false;
	_bool						m_bNextFinish = false;
private:
	_uint						m_iCurrentKeyFrame = 0;
	_double						m_TimeDelta = 0.0;
private:
	_uint						m_iMaxTickCount = 0;
	_uint						m_iTickCount = 0;
	_bool						m_bIsLoop = false;
	_int						m_iSign = 1;
	_uint						m_iPreKeyFrame = 0;
private:
	_double						m_lfBlendSpeed = 0.0;
	_double						m_lfAnimSpeed = 0.0;

public:
	static CAnimation* Create(_double TickPerSecond, _double Duration);
	virtual void Free() override;
};

END