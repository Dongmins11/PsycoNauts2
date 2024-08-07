#include "..\Public\Animation.h"
#include "AnimBone.h"


CAnimation::CAnimation()
{
}

HRESULT CAnimation::NativeConstruct(_double TickPerSecond, _double Duration)
{
	m_TickPerSecond = TickPerSecond;
	m_Duration = Duration;

	m_lfAnimSpeed = 1.0;
	m_lfBlendSpeed = 10.0;

	return S_OK;
}

void CAnimation::Reset_Animation()
{
	m_TimeAcc = 0.0;
	m_iCurrentKeyFrame = 0;
	m_isFinished = false;
	m_bNextFinish = false;

	for (auto& pAnimBone : m_AnimBones)
	{
		if (nullptr != pAnimBone)
			pAnimBone->Reset_State();
	}

}

void CAnimation::Reset_Animation_KeyFrame()
{
	m_TimeAcc = 0.0;
	m_iCurrentKeyFrame = 0;
	m_isFinished = false;
	m_bNextFinish = false;

	for (auto& pAnimBone : m_AnimBones)
	{
		if (nullptr != pAnimBone)
			pAnimBone->Reset_State_TransformationMatrix();
	}
}

void CAnimation::Update_AnimBoneTransformationMatrix_Finishied(_double TimeDelta)
{
	m_TimeAcc += m_TickPerSecond * TimeDelta;

	if (m_TimeAcc >= m_Duration)
	{
		m_iCurrentKeyFrame = 0;
		m_TimeAcc = 0.0;
		m_isFinished = true;

		for (auto& pAnimBone : m_AnimBones)
		{
			_vector		vScale, vRotation, vPosition;

			vector<KEYFRAME*>	KeyFrames = pAnimBone->Get_KeyFrames();

			KEYFRAME*	pKeyFrame = KeyFrames.back();

			vScale = XMLoadFloat3(&pKeyFrame->vScale);
			vRotation = XMLoadFloat4(&pKeyFrame->vRotation);
			vPosition = XMLoadFloat3(&pKeyFrame->vPosition);

			_matrix			TransformationMatrix;

			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

			pAnimBone->Set_TransformationMatrix(TransformationMatrix);

			pAnimBone->Set_CurrentKeyFrameIndex(0);
		}
		return;
	}
	else
		m_isFinished = false;

	for (auto& pAnimBone : m_AnimBones)
	{
		_uint iCurKeyFrame = pAnimBone->Get_CurrentKeyFrameIndex();

		if (iCurKeyFrame > m_iCurrentKeyFrame)
			m_iCurrentKeyFrame = iCurKeyFrame;

		if (true == m_isFinished)
			pAnimBone->Set_CurrentKeyFrameIndex(0);


		vector<KEYFRAME*>	KeyFrames = pAnimBone->Get_KeyFrames();

		_vector		vSourScale, vDestScale, vScale;
		_vector		vSourRotation, vDestRotation, vRotation;
		_vector		vSourPosition, vDestPosition, vPosition;

		_uint iCurrentKeyFrame = pAnimBone->Get_CurrentKeyFrameIndex();

		if (m_TimeAcc >= KeyFrames.back()->Time)
		{
			KEYFRAME*	pKeyFrame = KeyFrames.back();

			vScale = XMLoadFloat3(&pKeyFrame->vScale);
			vRotation = XMLoadFloat4(&pKeyFrame->vRotation);
			vPosition = XMLoadFloat3(&pKeyFrame->vPosition);

			iCurrentKeyFrame = KeyFrames.size() - 1;
			pAnimBone->Set_CurrentKeyFrameIndex(iCurrentKeyFrame);
		}

		else
		{
			while (m_TimeAcc >= KeyFrames[iCurrentKeyFrame + 1]->Time)
			{
				iCurrentKeyFrame = iCurrentKeyFrame + 1;
				pAnimBone->Set_CurrentKeyFrameIndex(iCurrentKeyFrame);
			}

			_float		fRatio = (m_TimeAcc - KeyFrames[iCurrentKeyFrame]->Time)
				/ (KeyFrames[iCurrentKeyFrame + 1]->Time - KeyFrames[iCurrentKeyFrame]->Time);

			vSourScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame]->vScale);
			vSourRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrame]->vRotation);
			vSourPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame]->vPosition);

			vDestScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame + 1]->vScale);
			vDestRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrame + 1]->vRotation);
			vDestPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame + 1]->vPosition);

			vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
		}

		_matrix			TransformationMatrix;

		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		pAnimBone->Set_TransformationMatrix(TransformationMatrix);
	}
}

void CAnimation::Update_AnimBoneTransformationMatrix(_double TimeDelta)
{
	m_TimeAcc += m_TickPerSecond * TimeDelta;

	if (m_TimeAcc >= m_Duration)
	{
		m_iCurrentKeyFrame = 0;
		m_isFinished = true;
		m_TimeAcc = 0.0;	
	}
	else
		m_isFinished = false;

	for (auto& pAnimBone : m_AnimBones)
	{
		_uint iCurKeyFrame = pAnimBone->Get_CurrentKeyFrameIndex();

		if (iCurKeyFrame > m_iCurrentKeyFrame)
			m_iCurrentKeyFrame = iCurKeyFrame;

		if (true == m_isFinished)
		{
			pAnimBone->Set_CurrentKeyFrameIndex(0);
		}

		vector<KEYFRAME*>	KeyFrames = pAnimBone->Get_KeyFrames();

		_vector		vSourScale, vDestScale, vScale;
		_vector		vSourRotation, vDestRotation, vRotation;
		_vector		vSourPosition, vDestPosition, vPosition;

		_uint iCurrentKeyFrame = pAnimBone->Get_CurrentKeyFrameIndex();

		if (m_TimeAcc >= KeyFrames.back()->Time)
		{
			KEYFRAME*	pKeyFrame = KeyFrames.back();

			vScale = XMLoadFloat3(&pKeyFrame->vScale);
			vRotation = XMLoadFloat4(&pKeyFrame->vRotation);
			vPosition = XMLoadFloat3(&pKeyFrame->vPosition);

			iCurrentKeyFrame = KeyFrames.size() - 1;
			pAnimBone->Set_CurrentKeyFrameIndex(iCurrentKeyFrame);
		}

		else
		{
			while (m_TimeAcc >= KeyFrames[iCurrentKeyFrame + 1]->Time)
			{
				iCurrentKeyFrame = iCurrentKeyFrame + 1;
				pAnimBone->Set_CurrentKeyFrameIndex(iCurrentKeyFrame);
			}

			_float		fRatio = (m_TimeAcc - KeyFrames[iCurrentKeyFrame]->Time)
				/ (KeyFrames[iCurrentKeyFrame + 1]->Time - KeyFrames[iCurrentKeyFrame]->Time);

			vSourScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame]->vScale);
			vSourRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrame]->vRotation);
			vSourPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame]->vPosition);

			vDestScale = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame + 1]->vScale);
			vDestRotation = XMLoadFloat4(&KeyFrames[iCurrentKeyFrame + 1]->vRotation);
			vDestPosition = XMLoadFloat3(&KeyFrames[iCurrentKeyFrame + 1]->vPosition);

			vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
		}
	
		_matrix			TransformationMatrix;

		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		pAnimBone->Set_TransformationMatrix(TransformationMatrix);
	}
}

void CAnimation::Update_NextAnimBoneTransformationMatrix(_double TimeDelta)
{
	if (false == m_bIsLoop)
		m_TimeAcc += (m_TickPerSecond * TimeDelta) * m_lfAnimSpeed;
	else if (true == m_bIsLoop)
	{
		if (m_iMaxTickCount < m_iTickCount)
		{
			m_iSign *= -1;
			m_iTickCount = 0;
		}
		m_TimeAcc += m_TickPerSecond*TimeDelta*m_iSign;

		++m_iTickCount;
	}
	if (m_Duration <= m_TimeAcc)
	{
		m_TimeAcc = 0.0;
		m_isFinished = true;
		m_iCurrentKeyFrame = 0;
		for (auto& pAnimBone : m_AnimBones)
		{
			if (nullptr != pAnimBone)
				pAnimBone->Set_CurrentKeyFrameIndex(0);
		}
		return;
	}
	else
		m_isFinished = false;

	if (m_Duration <= m_TimeAcc + m_TickPerSecond*TimeDelta)
	{
		m_bNextFinish = true;

	}
	else
		m_bNextFinish = false;

	for (auto& pAnimBone : m_AnimBones)
	{

		if (nullptr != pAnimBone)
		{
			_uint iCurrentKeyFrame = pAnimBone->Get_CurrentKeyFrameIndex();

			if (iCurrentKeyFrame > m_iCurrentKeyFrame)
				m_iCurrentKeyFrame = iCurrentKeyFrame;


			pAnimBone->Update_TransformationMatrix(m_TimeAcc, m_isFinished);
		}
	}
}

_bool CAnimation::Update_AnimBoneTransformationMatrix_Lefp(_double _dTimeDelta, CAnimation * _pDestAnimation)
{
	auto iterBone = _pDestAnimation->m_AnimBones.begin();
	_bool	bCheck = false;
	m_bNextFinish = false;
	for (auto& pAnimBone : m_AnimBones)
	{
		if (nullptr != pAnimBone)
		{
			if (true == pAnimBone->Update_TransformationMatrix_Lefp(_dTimeDelta * m_lfBlendSpeed, *iterBone))
			{
				bCheck = true;
				m_bNextFinish = true;
			}
		}

		++iterBone;
	}
	return bCheck;
}

void CAnimation::Set_KeyFrameUpdate(_double TimeDelta, _uint iSetKeyFrameIndex, _bool * bGetTheFinishied)
{
	_bool	bBackKeyFrameCheck = false;

	if (m_iPreKeyFrame <= iSetKeyFrameIndex)
	{
		if (0 < iSetKeyFrameIndex)
		{
			if (m_TimeAcc <= iSetKeyFrameIndex)
				m_TimeAcc += (m_TickPerSecond * TimeDelta) * m_lfAnimSpeed;
		}
		bBackKeyFrameCheck = false;
	}
	else if (m_iPreKeyFrame > iSetKeyFrameIndex)
	{
		if (m_TimeAcc >= iSetKeyFrameIndex)
			m_TimeAcc -= (m_TickPerSecond * TimeDelta) * m_lfAnimSpeed;

		if (0.0 >= m_TimeAcc)
			m_TimeAcc = 0.0;

		bBackKeyFrameCheck = true;
	}

	m_iPreKeyFrame = iSetKeyFrameIndex;

	if (m_Duration <= m_TimeAcc)
	{
		m_isFinished = true;
		*bGetTheFinishied = m_isFinished;
		return;
	}
	else
	{
		m_isFinished = false;
		*bGetTheFinishied = m_isFinished;
	}

	for (auto& pAnimBone : m_AnimBones)
	{
		if (nullptr != pAnimBone)
			pAnimBone->Set_KeyFrameTransformationMatrix(m_TimeAcc, bBackKeyFrameCheck);
	}
}

CAnimation * CAnimation::Create(_double TickPerSecond, _double Duration)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct(TickPerSecond, Duration)))
	{
		MSGBOX("Failed to Created CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pAnimBone : m_AnimBones)
		Safe_Release(pAnimBone);

	m_AnimBones.clear();
}

