#include "..\Public\AnimBone.h"




CAnimBone::CAnimBone()
{
}

void CAnimBone::Reset_State()
{
	m_iCurrentKeyFrame = 0;
	m_vPos_BackUp = m_KeyFrames[m_iCurrentKeyFrame]->vPosition;
	XMStoreFloat4(&m_vRotation_BackUp, XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame]->vRotation));
	m_vScale_BackUp = m_KeyFrames[m_iCurrentKeyFrame]->vScale;
}

void CAnimBone::Reset_State_TransformationMatrix()
{
	m_iCurrentKeyFrame = 0;
	m_vPos_BackUp = m_KeyFrames[m_iCurrentKeyFrame]->vPosition;
	XMStoreFloat4(&m_vRotation_BackUp, XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame]->vRotation));
	m_vScale_BackUp = m_KeyFrames[m_iCurrentKeyFrame]->vScale;

	_vector vPosition, vRotation, vScale;

	vPosition = XMLoadFloat3(&m_vPos_BackUp);
	vRotation = XMLoadFloat4(&m_vRotation_BackUp);
	vScale = XMLoadFloat3(&m_vScale_BackUp);

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));
}

void CAnimBone::Update_TransformationMatrix(_double _dTimeAcc, _bool _bIsFinished)
{
	if (true == _bIsFinished)
		m_iCurrentKeyFrame = 0;

	_vector vSourScale, vDestScale, vScale;
	_vector vSourRotation, vDestRotation, vRotation;
	_vector vSourPosition, vDestPosition, vPosition;

	_double dRatio = 0.f;

	if (_dTimeAcc >= m_KeyFrames.back()->Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back()->vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back()->vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back()->vPosition);

		m_iCurrentKeyFrame = (_uint)m_KeyFrames.size() - 1;
	}
	else
	{
		while (_dTimeAcc >= m_KeyFrames[m_iCurrentKeyFrame + 1]->Time)
			m_iCurrentKeyFrame += 1;

		dRatio = (_dTimeAcc - m_KeyFrames[m_iCurrentKeyFrame]->Time) / (m_KeyFrames[m_iCurrentKeyFrame + 1]->Time - m_KeyFrames[m_iCurrentKeyFrame]->Time);

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame]->vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)dRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)dRatio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)dRatio);

		XMStoreFloat3(&m_vScale_BackUp, vScale);
		XMStoreFloat4(&m_vRotation_BackUp, vRotation);
		XMStoreFloat3(&m_vPos_BackUp, vPosition);
	}
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));


	return;
}

_bool CAnimBone::Update_TransformationMatrix_Lefp(_double _dTimeAcc, CAnimBone * _pDestAnimBone)
{
	_vector vSourScale, vDestScale, vScale;
	_vector vSourRotation, vDestRotation, vRotation;
	_vector vSourPosition, vDestPosition, vPosition;

	m_fRatio += (_float)_dTimeAcc;


	vSourScale = XMLoadFloat3(&m_vScale_BackUp);
	vSourRotation = XMLoadFloat4(&m_vRotation_BackUp);
	vSourPosition = XMLoadFloat3(&m_vPos_BackUp);

	vDestScale = XMLoadFloat3(&_pDestAnimBone->m_KeyFrames[0]->vScale);
	vDestRotation = XMLoadFloat4(&_pDestAnimBone->m_KeyFrames[0]->vRotation);
	vDestPosition = XMLoadFloat3(&_pDestAnimBone->m_KeyFrames[0]->vPosition);

	vScale = XMVectorLerp(vSourScale, vDestScale, m_fRatio);
	vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, m_fRatio);
	vPosition = XMVectorLerp(vSourPosition, vDestPosition, m_fRatio);

	if (1.f <= m_fRatio + (_dTimeAcc * 2))
	{
		vScale = XMLoadFloat3(&_pDestAnimBone->m_KeyFrames[0]->vScale);
		vRotation = XMLoadFloat4(&_pDestAnimBone->m_KeyFrames[0]->vRotation);
		vPosition = XMLoadFloat3(&_pDestAnimBone->m_KeyFrames[0]->vPosition);

		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));

		m_fRatio = 0.f;

		return true;
	}

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));

	return false;
}

void CAnimBone::Set_KeyFrameTransformationMatrix(_double _dTimeAcc, _bool bBackKeyFrameCheck)
{
	_vector vSourScale, vDestScale, vScale;
	_vector vSourRotation, vDestRotation, vRotation;
	_vector vSourPosition, vDestPosition, vPosition;

	_double dRatio = 0.f;

	if (false == bBackKeyFrameCheck)
	{
		if (_dTimeAcc >= m_KeyFrames.back()->Time)
		{
			vScale = XMLoadFloat3(&m_KeyFrames.back()->vScale);
			vRotation = XMLoadFloat4(&m_KeyFrames.back()->vRotation);
			vPosition = XMLoadFloat3(&m_KeyFrames.back()->vPosition);

			m_iCurrentKeyFrame = (_uint)m_KeyFrames.size() - 1;
		}
		else
		{
			while (_dTimeAcc >= m_KeyFrames[m_iCurrentKeyFrame + 1]->Time)
				m_iCurrentKeyFrame += 1;

			dRatio = (_dTimeAcc - m_KeyFrames[m_iCurrentKeyFrame]->Time) / (m_KeyFrames[m_iCurrentKeyFrame + 1]->Time - m_KeyFrames[m_iCurrentKeyFrame]->Time);

			vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vScale);
			vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame]->vRotation);
			vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vPosition);

			vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vScale);
			vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vRotation);
			vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1]->vPosition);

			vScale = XMVectorLerp(vSourScale, vDestScale, (_float)dRatio);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)dRatio);
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)dRatio);

			XMStoreFloat3(&m_vScale_BackUp, vScale);
			XMStoreFloat4(&m_vRotation_BackUp, vRotation);
			XMStoreFloat3(&m_vPos_BackUp, vPosition);
		}
	}
	else
	{
		if (_dTimeAcc <= m_KeyFrames.front()->Time)
		{
			vScale = XMLoadFloat3(&m_KeyFrames.front()->vScale);
			vRotation = XMLoadFloat4(&m_KeyFrames.front()->vRotation);
			vPosition = XMLoadFloat3(&m_KeyFrames.front()->vPosition);

			m_iCurrentKeyFrame = 0;
		}
		else
		{
			if (0 >= m_iCurrentKeyFrame)
			{
				m_iCurrentKeyFrame = 0;

				vScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vScale);
				vRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame]->vRotation);
				vPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vPosition);

				XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));

				return;
			}

			while (_dTimeAcc <= m_KeyFrames[m_iCurrentKeyFrame - 1]->Time)
				m_iCurrentKeyFrame -= 1;

			dRatio = (_dTimeAcc - m_KeyFrames[m_iCurrentKeyFrame]->Time) / (m_KeyFrames[m_iCurrentKeyFrame]->Time - m_KeyFrames[m_iCurrentKeyFrame - 1]->Time);
			if (0 >= dRatio)
				dRatio *= -1;

			vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vScale);
			vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame]->vRotation);
			vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame]->vPosition);

			vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame - 1]->vScale);
			vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame - 1]->vRotation);
			vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame - 1]->vPosition);

			vScale = XMVectorLerp(vSourScale, vDestScale, (_float)dRatio);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)dRatio);
			vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)dRatio);

			XMStoreFloat3(&m_vScale_BackUp, vScale);
			XMStoreFloat4(&m_vRotation_BackUp, vRotation);
			XMStoreFloat3(&m_vPos_BackUp, vPosition);
		}
	}
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition));

	return;
}

HRESULT CAnimBone::NativeConstruct()
{
	m_iCurrentKeyFrame = 0;

	return S_OK;
}

CAnimBone * CAnimBone::Create()
{
	CAnimBone*	pInstance = new CAnimBone();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Created CAnimBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimBone::Free()
{
	for (auto& pKeyFrame : m_KeyFrames)
		Safe_Delete(pKeyFrame);

	m_KeyFrames.clear();
}

