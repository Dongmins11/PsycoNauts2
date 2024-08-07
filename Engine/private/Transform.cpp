#include "..\Public\Transform.h"
#include "Navigation.h"
#include "CharacterController.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_vector CTransform::Get_Scale()
{
	return XMVectorSet(XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_RIGHT))),
		XMVectorGetY(XMVector3Length(Get_State(CTransform::STATE_UP))),
		XMVectorGetZ(XMVector3Length(Get_State(CTransform::STATE_LOOK)))
		, 0.f);	
}

_float3 CTransform::Get_ScaleFloat3()
{
	_float x = GET_VECTORLENGTH(Get_State(CTransform::STATE_RIGHT));
	_float y = GET_VECTORLENGTH(Get_State(CTransform::STATE_UP));
	_float z = GET_VECTORLENGTH(Get_State(CTransform::STATE_LOOK));


	return _float3(x,y,z);
}

void CTransform::Set_TransFormDesc(_double lfRotationPerSec, _double lfSpeedPerSec)
{
	m_TransformDesc.RotationPerSec = lfRotationPerSec;
	m_TransformDesc.SpeedPerSec = lfSpeedPerSec;
}

_float4x4 CTransform::Get_BillBoardFloat4x4(_fvector CamPos, _float3 vSize)
{
	_matrix BillboardMatrix = XMMatrixIdentity();
	_float4x4 ReturnBillboard;
	_vector vLook = XMVector3Normalize(CamPos - Get_State(CTransform::STATE_POSITION)) * vSize.z;
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vSize.x;
	_vector	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vSize.y;

	BillboardMatrix.r[0] = vRight;
	BillboardMatrix.r[1] = vUp;
	BillboardMatrix.r[2] = vLook;
	BillboardMatrix.r[3] = Get_State(CTransform::STATE_POSITION);
			
	XMStoreFloat4x4(&ReturnBillboard, BillboardMatrix);
	return ReturnBillboard;
}

_matrix CTransform::Get_BillBoardMatrix(_fvector CamPos, _float3 vSize)
{
	_matrix BillboardMatrix = XMMatrixIdentity();
	_vector vLook = XMVector3Normalize(CamPos - Get_State(CTransform::STATE_POSITION)) * vSize.z;
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vSize.x;
	_vector	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vSize.y;

	BillboardMatrix.r[0] = vRight;
	BillboardMatrix.r[1] = vUp;
	BillboardMatrix.r[2] = vLook;
	BillboardMatrix.r[3] = Get_State(CTransform::STATE_POSITION);

	return BillboardMatrix;
}

const _float3 & CTransform::Get_Velocity() const
{
	return m_vVelocity;
}
void CTransform::Set_Velocity(const _fvector & vVelocity)
{
	XMStoreFloat3(&m_vVelocity, vVelocity);
}

void CTransform::Add_Velocity(const _fvector & vVelocity)
{
	XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vVelocity);
}

void CTransform::Add_Velocity(const STATE _eState, const _float _fValue)
{
	_vector vVelocity = XMVectorZero();

	switch (_eState)
	{
	case STATE_RIGHT:
		vVelocity = XMVectorSet(_fValue, 0.f, 0.f, 0.f);
		break;
	case STATE_UP:
		vVelocity = XMVectorSet(0.f, _fValue, 0.f, 0.f);
		break;
	case STATE_LOOK:
		vVelocity = XMVectorSet(0.f, 0.f, _fValue, 0.f);
		break;
	}

	_vector vScale, vRot, vPos;
	XMMatrixDecompose(&vScale, &vRot, &vPos, XMLoadFloat4x4(&m_WorldMatrix));
	_matrix matRotation = XMMatrixRotationQuaternion(vRot);
	vVelocity = XMVector4Transform(vVelocity, matRotation);
	XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + vVelocity);

}

HRESULT CTransform::NativeConstruct_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

HRESULT CTransform::Go_Straight(_double TimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * _float(m_TransformDesc.SpeedPerSec * TimeDelta);

	//if (nullptr != pNavigation)
	//{
	//	if (false == pNavigation->Move_OnNavigation(vPosition))
	//		return S_OK;
	//}

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Backward(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * _float(m_TransformDesc.SpeedPerSec * TimeDelta);

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Left(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * _float(m_TransformDesc.SpeedPerSec * TimeDelta);

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Right(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * _float(m_TransformDesc.SpeedPerSec * TimeDelta);

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_Straight(_double TimeDelta, CCharacterController * _pController)
{
	Add_Velocity(Get_State(CTransform::STATE_LOOK) * _float(m_TransformDesc.SpeedPerSec * TimeDelta));
	//Safe_AddRef(_pController);
	//_pController->Move(TimeDelta, vDisp);
	//Safe_Release(_pController);
	return S_OK;
}

HRESULT CTransform::Go_Backward(_double TimeDelta, CCharacterController * _pController)
{
	Add_Velocity(Get_State(CTransform::STATE_LOOK) * _float(m_TransformDesc.SpeedPerSec * -TimeDelta));

	//_float3 vDisp = STOREFLOAT3(Get_State(CTransform::STATE_LOOK) * _float(m_TransformDesc.SpeedPerSec * -TimeDelta));

	//XMStoreFloat3(&vDisp, Get_State(CTransform::STATE_LOOK)*-1.f);
	//Safe_AddRef(_pController);
	//_pController->Move(TimeDelta, vDisp);
	//Safe_Release(_pController);
	return S_OK;
}

HRESULT CTransform::Go_Left(_double TimeDelta, CCharacterController * _pController)
{
	Add_Velocity(Get_State(CTransform::STATE_RIGHT) * _float(m_TransformDesc.SpeedPerSec * -TimeDelta));

	//_float3 vDisp = STOREFLOAT3(Get_State(CTransform::STATE_RIGHT) * _float(m_TransformDesc.SpeedPerSec * -TimeDelta));
	//
	//_float3 vDisp;
	//XMStoreFloat3(&vDisp, Get_State(CTransform::STATE_RIGHT)*-1.f);
	//Safe_AddRef(_pController);
	//_pController->Move(TimeDelta, vDisp);
	//Safe_Release(_pController);
	return S_OK;
}

HRESULT CTransform::Go_Right(_double TimeDelta, CCharacterController * _pController)
{
	Add_Velocity(Get_State(CTransform::STATE_RIGHT) * _float(m_TransformDesc.SpeedPerSec * TimeDelta));

	//_float3 vDisp;
	//XMStoreFloat3(&vDisp, Get_State(CTransform::STATE_RIGHT));
	//_float3 vDisp = STOREFLOAT3(Get_State(CTransform::STATE_RIGHT) * _float(m_TransformDesc.SpeedPerSec * TimeDelta));

	//Safe_AddRef(_pController);
	//_pController->Move(TimeDelta, vDisp);
	//Safe_Release(_pController);
	return S_OK;
}

HRESULT CTransform::Go_ByAxis(_fvector vAxis, _double TimeDelta, _fvector vPos, CNavigation * pNavigation)
{
	_vector vPosition;
	if (nullptr != pNavigation)
		vPosition = vPos;
	else
		vPosition = Get_State(CTransform::STATE_POSITION);

	_vector vCurPosition = vPosition;
	_float3 SlidingVector;
	_bool	isOnAir = false;

	//_vector vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vAxis) * _float(m_TransformDesc.SpeedPerSec * TimeDelta);


	//if (nullptr != pNavigation)
	//{
	//	//Set_State(CTransform::STATE_POSITION, vPosition);
	//
	//	if (false == pNavigation->Move_OnNavigation(vPosition, vCurPosition, &SlidingVector, &posY, &m_isOnAir))
	//	{
	//		if (!XMVector3Equal(XMLoadFloat3(&SlidingVector), XMVectorZero()))
	//			Go_ByAxis(XMLoadFloat3(&SlidingVector), TimeDelta, vPosition, pNavigation);
	//		return S_OK;
	//	}
	//	else
	//	{
	//		if (!m_isOnAir)
	//			vPosition = XMVectorSet(XMVectorGetX(vPosition), posY, XMVectorGetZ(vPosition), XMVectorGetW(vPosition));
	//	}
	//}

	Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

HRESULT CTransform::Go_ByAxis(_fvector vAxis, _double TimeDelta, _fvector vPos, CCharacterController * pController)
{
	Add_Velocity(XMVector3Normalize(vAxis) * _float(m_TransformDesc.SpeedPerSec * TimeDelta));
	return S_OK;
}

HRESULT CTransform::RotationPerSec(_fvector vAxis, _double TimeDelta)
{
	_vector vRight = Get_State(CTransform::STATE_RIGHT);
	_vector vUp = Get_State(CTransform::STATE_UP);
	_vector vLook = Get_State(CTransform::STATE_LOOK);

	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis, _float(m_TransformDesc.RotationPerSec * TimeDelta));

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight); 
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::RotationByRadian(_fvector vAxis, _float fRadian)
{
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(Get_Scale());
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(Get_Scale());
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetZ(Get_Scale());

	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::RotationByRadian(_fvector vAxis, _float3 fRadian)
{
	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(Get_Scale());
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(Get_Scale());
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetZ(Get_Scale());

	_matrix RotationMat = XMMatrixRotationRollPitchYaw(fRadian.x, fRadian.y, fRadian.z);

	vRight = XMVector3TransformNormal(vRight, RotationMat);
	vUp = XMVector3TransformNormal(vUp, RotationMat);
	vLook = XMVector3TransformNormal(vLook, RotationMat);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::RotationQuaternion(_fvector vQuaternion)
{
	_vector vScale = Get_Scale();
	_float	fSizeX = XMVectorGetX(vScale);
	_float	fSizeY = XMVectorGetY(vScale);
	_float	fSizeZ = XMVectorGetZ(vScale);

	_matrix RotationMatirx = XMMatrixRotationQuaternion(vQuaternion);

	Set_State(CTransform::STATE_RIGHT, RotationMatirx.r[0] * fSizeX);
	Set_State(CTransform::STATE_UP, RotationMatirx.r[1] * fSizeY);
	Set_State(CTransform::STATE_LOOK, RotationMatirx.r[2] * fSizeZ);
	return S_OK;
}

HRESULT CTransform::Scaling(_float3 vScale)
{
	if (vScale.x <= 0.f || vScale.y <= 0.f || vScale.z <= 0.f)
		return E_FAIL;

	_vector vRight = Get_State(CTransform::STATE_RIGHT);
	_vector vUp = Get_State(CTransform::STATE_UP);
	_vector vLook = Get_State(CTransform::STATE_LOOK);

	vRight = XMVector3Normalize(vRight) * vScale.x;
	vUp = XMVector3Normalize(vUp) * vScale.y;
	vLook = XMVector3Normalize(vLook) * vScale.z;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::Scaling(_fvector vScale)
{
	if (XMVectorGetX(vScale) <= 0.f || XMVectorGetY(vScale) <= 0.f || XMVectorGetZ(vScale) <= 0.f)
		return E_FAIL;

	_vector vRight = Get_State(CTransform::STATE_RIGHT);
	_vector vUp = Get_State(CTransform::STATE_UP);
	_vector vLook = Get_State(CTransform::STATE_LOOK);

	vRight = XMVector3Normalize(vRight) * XMVectorGetX(vScale);
	vUp = XMVector3Normalize(vUp) * XMVectorGetY(vScale);
	vLook = XMVector3Normalize(vLook) * XMVectorGetZ(vScale);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

HRESULT CTransform::BillBoard(_fvector vCamPos, _fvector vPosition)
{
	_vector vPos;
	if (XMVector3Equal(vPosition, XMVectorZero()))
		vPos = Get_State(CTransform::STATE_POSITION);
	else
		vPos = vPosition;

	if (1 > XMVectorGetX(XMVector3Length(vCamPos - vPos)))
		return S_OK;

	_vector vLook = XMVector3Normalize(vCamPos - vPos);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	_vector vScale = Get_Scale();

	Set_State(STATE_RIGHT, vRight * XMVectorGetX(vScale));
	Set_State(STATE_UP, vUp * XMVectorGetY(vScale));
	Set_State(STATE_LOOK, vLook * XMVectorGetZ(vScale));

	return S_OK;
}

HRESULT CTransform::BillBoardY(_fvector vCamPos, _fvector vPosition)
{
	_vector vPos;
	if (XMVector3Equal(vPosition, XMVectorZero()))
		vPos = Get_State(CTransform::STATE_POSITION);
	else
		vPos = vPosition;

	if (1 > XMVectorGetX(XMVector3Length(vCamPos - vPos)))
		return S_OK;

	_vector vPosNoY = XMVectorSetY(vPos, 0.f);
	_vector vCamPosNoY = XMVectorSetY(vCamPos, 0.f);

	_vector vLook = XMVector3Normalize(vCamPosNoY - vPosNoY);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));

	_vector vScale = Get_Scale();

	Set_State(STATE_RIGHT, vRight * XMVectorGetX(vScale));
	Set_State(STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(vScale));
	Set_State(STATE_LOOK, vLook * XMVectorGetZ(vScale));

	return S_OK;
}

HRESULT CTransform::LerpBillBoardRotation(_fvector vSrcLook, _fvector vDestLook, _double lfLerpTimeAcc, _double lfLerpMaxTime)
{
	if (lfLerpTimeAcc >= lfLerpMaxTime)
		lfLerpTimeAcc = lfLerpMaxTime;

	_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vSrcLook, vDestLook, lfLerpTimeAcc / lfLerpMaxTime));
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLerpLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLerpLook, vRight));

	_vector vScale = Get_Scale();

	Set_State(STATE_RIGHT, vRight * XMVectorGetX(vScale));
	Set_State(STATE_UP, vUp * XMVectorGetY(vScale));
	Set_State(STATE_LOOK, vLerpLook * XMVectorGetZ(vScale));

	return S_OK;
}

void CTransform::Jump(_double lfTimeDelta, _bool * isJump, _float * startY, _double * lfJumpTime, _float * jumpPower, _float * fJumpValue, _bool * bFallCheck, _bool * bLandCheck, CCharacterController * _pController, CNavigation * pNavigation)
{
	_vector MyPosVec = Get_State(CTransform::STATE_POSITION);

	_float TempjumpTime = (_float)*lfJumpTime;
	_float final_jumpPower = (*jumpPower);
	if (final_jumpPower > 50)
		final_jumpPower = 50;

	_float Y = ((TempjumpTime * TempjumpTime * (-9.8f / 2.f)) + TempjumpTime * final_jumpPower) *lfTimeDelta;
	_float Velocity = -9.8f * TempjumpTime + final_jumpPower;
	Add_Velocity(CTransform::STATE_UP, Velocity *lfTimeDelta);

	*lfJumpTime += lfTimeDelta;

	if (Y < *fJumpValue)
		*bFallCheck = true;

	*fJumpValue = Y;

	if (true == *bFallCheck)
	{
		if (_pController->IsDown())
		{	
			*fJumpValue = 0.f;
			*jumpPower = 0.f;
			*bLandCheck = true;
			*bFallCheck = false;
			*isJump = false;
			*lfJumpTime = 0.f;
		}
	}
}

void CTransform::Double_Jump(_double lfTimeDelta, _bool * isJump, _float * startY, _float * EndY, _double * lfJumpTime, _float * jumpPower, _float * fJumpValue, _bool * bFallCheck, _bool * bLandCheck, CCharacterController * _pController, CNavigation * pNavigation)
{
	_vector MyPosVec = Get_State(CTransform::STATE_POSITION);

	_float TempjumpTime = (_float)*lfJumpTime;
	_float final_jumpPower = (*jumpPower);
	if (final_jumpPower > 50)
		final_jumpPower = 50;

	_float Y = ((TempjumpTime * TempjumpTime * (-9.8f / 2.f)) + TempjumpTime * final_jumpPower);
	_float Velocity = -9.8f * TempjumpTime + final_jumpPower;
	Add_Velocity(CTransform::STATE_UP, Velocity *lfTimeDelta);

	*lfJumpTime += lfTimeDelta;

	if (Y < *fJumpValue)
		*bFallCheck = true;

	*fJumpValue = Y;


	if (_pController->IsDown())
	{
		*fJumpValue = 0.f;
		*jumpPower = 0.f;
		*bLandCheck = true;
		*bFallCheck = false;
		*isJump = false;
		*lfJumpTime = 0.f;
	}
}

void CTransform::Ledge_Jump(_double lfTimeDelta, _bool * isJump, _float * startY,_double * lfJumpTime, _float * jumpPower, _float * fJumpValue, _bool * bFallCheck, _bool * bLandCheck, _bool * bControllerCheck, CCharacterController * _pController)
{
	_vector MyPosVec = Get_State(CTransform::STATE_POSITION);

	_float TempjumpTime = (_float)*lfJumpTime;
	_float final_jumpPower = (*jumpPower);
	if (final_jumpPower > 50)
		final_jumpPower = 50;

	_float Y = ((TempjumpTime * TempjumpTime * (-9.8f / 2.f)) + TempjumpTime * final_jumpPower);
	_float Velocity = -9.8f * TempjumpTime + final_jumpPower;

	if (true == *bControllerCheck)
	{
		Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(MyPosVec), *startY + Y, XMVectorGetZ(MyPosVec), 1.f));
		_pController->Set_FootPosition(_float3(XMVectorGetX(MyPosVec), *startY + Y, XMVectorGetZ(MyPosVec)));
	}
	else
		Add_Velocity(CTransform::STATE_UP, Velocity *lfTimeDelta);



	*lfJumpTime += lfTimeDelta;

	if (Y < *fJumpValue)
	{
		*bFallCheck = true;
		*bControllerCheck = false;
	}

	*fJumpValue = Y;

	if (true == *bFallCheck)
	{
		if (_pController->IsDown())
		{
			*fJumpValue = 0.f;
			*jumpPower = 0.f;
			*bLandCheck = true;
			*bFallCheck = false;
			*isJump = false;
			*lfJumpTime = 0.f;
		}
	}
}


void CTransform::Set_MyLook(_fvector vLook)
{
	_vector	Scale = Get_Scale();
	_vector vRight;
	_vector vUp;

	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * XMVectorGetZ(Scale));

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * XMVectorGetX(Scale));

	vUp = XMVector3Cross(vLook, vRight);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * XMVectorGetY(Scale));
}

void CTransform::Set_MyUp(_fvector vUp)
{
	_vector	Scale = Get_Scale();
	_vector vRight;
	_vector vLook;

	Set_State(STATE_UP, XMVector3Normalize(vUp) * XMVectorGetY(Scale));

	vLook = XMVector3Cross(XMVectorSet(1.f, 0.f, 0.f, 0.f), vUp);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * XMVectorGetZ(Scale));

	vRight = XMVector3Cross(vUp,vLook);
	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * XMVectorGetX(Scale));
}


CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTransform*	pInstance = new CTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTransform");
		
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*	pInstance = new CTransform(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();


}
