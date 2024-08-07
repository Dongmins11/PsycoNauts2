#pragma once

#include "Base.h"

#define MAX_NAME 50
/* 애니메이션을 구성하는 뼈대 하나의 정보. */

BEGIN(Engine)

class CAnimBone final : public CBase
{
private:
	CAnimBone();
	virtual ~CAnimBone() = default;
public:
	vector<KEYFRAME*> Get_KeyFrames() {
		return m_KeyFrames;
	}
	_uint Get_CurrentKeyFrameIndex() const {
		return m_iCurrentKeyFrame;
	}

	void Set_CurrentKeyFrameIndex(_uint iKeyFrameIndex) {
		m_iCurrentKeyFrame = iKeyFrameIndex;
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	_matrix Get_TransformationMatrix() {
		return XMLoadFloat4x4(&m_TransformationMatrix);
	}

	_matrix Get_NonTransformmationMatrix(){
		return XMLoadFloat4x4(&m_NonRotationTransformationMatrix);
	}

public:/*For.BoneName */
	void Set_BoneName(const char* pName) {
		strcpy_s(m_szName, pName);
	}
	char* Get_szName() {
		return m_szName;
	}

public:/*For.Reset_BoneState*/
	void Reset_State();
	void Reset_State_TransformationMatrix();

public:/*For.Update_Bone*/
	void Update_TransformationMatrix(_double _dTimeAcc, _bool _bIsFinished);
	_bool Update_TransformationMatrix_Lefp(_double _dTimeAcc, CAnimBone* _pDestAnimBone);

public:/*For.Set_KeyFrame Matrix*/
	void Set_KeyFrameTransformationMatrix(_double _dTimeAcc, _bool bBackKeyFrameCheck);

public:
	HRESULT NativeConstruct();
	void Add_KeyFrame(KEYFRAME* pKeyFrame) {
		m_KeyFrames.push_back(pKeyFrame);
	}

private:
	vector<KEYFRAME*>			m_KeyFrames;
	_uint						m_iCurrentKeyFrame = 0;
private:
	_uint						m_iNumKeyFrames = 0;
	_float4x4					m_TransformationMatrix;
	_float4x4					m_NonRotationTransformationMatrix;

private:/* For.BoneLerp */
	_float3						m_vScale_BackUp;
	_float4						m_vRotation_BackUp;
	_float3						m_vPos_BackUp;
	_float						m_fRatio = 0.f;

	_bool						m_bIsLoop = false;
private:/* For.BoneName */
	char						m_szName[MAX_NAME];

public:
	static CAnimBone* Create();
	virtual void Free() override;
};

END