#include "..\Public\Frustum.h"
#include "PipeLine.h"
#include "Light_Manager.h"
#include "Light.h"
IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{

}	

HRESULT CFrustum::Intialize_Frustum()
{
	m_vOriginalPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginalPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginalPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginalPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vOriginalPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginalPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginalPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginalPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Update()
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix			ProjMatrix = pPipeLine->Get_Transform(CPipeLine::D3DTS_PROJ);
	ProjMatrix = XMMatrixInverse(nullptr, ProjMatrix);

	_matrix			ViewMatrix = pPipeLine->Get_Transform(CPipeLine::D3DTS_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	for (_uint i = 0; i < 8; ++i)
	{
		_vector		vPoint = XMVector3TransformCoord(XMLoadFloat3(&m_vOriginalPoints[i]), ProjMatrix);
		vPoint = XMVector3TransformCoord(vPoint, ViewMatrix);
		XMStoreFloat3(&m_vTransformPoints[i], vPoint);
	}
	Make_Plane(m_WorldPlane, m_vTransformPoints);
	
	RELEASE_INSTANCE(CPipeLine);
}

void CFrustum::Update_Shadow()
{
	CLight_Manager*		pLightManager = GET_INSTANCE(CLight_Manager);
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	const LIGHTDESC*		pLightDir = pLightManager->Get_LightDesc(0);
	_matrix			ProjMatrix = pPipeLine->Get_Transform(CPipeLine::D3DTS_PROJ);
	ProjMatrix = XMMatrixInverse(nullptr, ProjMatrix);

	_matrix			ViewMatrix = pPipeLine->Get_Transform(CPipeLine::D3DTS_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	for (_uint i = 0; i < 8; ++i)
	{
		_vector		vPoint = XMVector3TransformCoord(XMLoadFloat3(&m_vOriginalPoints[i]), ProjMatrix);
		vPoint = XMVector3TransformCoord(vPoint, ViewMatrix);
		XMStoreFloat3(&m_vTransformPoints[i], vPoint);
	}
	Make_Plane(m_WorldPlane, m_vTransformPoints);

	RELEASE_INSTANCE(CLight_Manager);
	RELEASE_INSTANCE(CPipeLine);

}

/*  로컬플레인을 갱신한다. m_vTransformPoints놈은 갱신되지 않느다. 월드상태를 유지한다. */
void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrixInverse)
{
	_float3		vLocalPoint[8];

	for (_uint i = 0; i < 8; ++i)	
		XMStoreFloat3(&vLocalPoint[i], XMVector3TransformCoord(XMLoadFloat3(&m_vTransformPoints[i]), WorldMatrixInverse));

	Make_Plane(m_LocalPlane, vLocalPoint);
}

_bool CFrustum::isIn_WorldSpace(_fvector vWorldPoint, _float fRange)
{
	for (_uint i = 0; i < 6; ++i)
	{
		/* ax + by + cz + d > 0*/
		/* a b c d */
		/* x y z 1 */

		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_WorldPlane[i]), vWorldPoint)))
			return false;	
	}
	return true;
}

_bool CFrustum::isIn_LocalSpace(_fvector vLocalPoint, _float fRange)
{
	for (_uint i = 0; i < 6; ++i)
	{		
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_LocalPlane[i]), vLocalPoint)))
			return false;
	}
	return true;
}

void CFrustum::Make_Plane(_float4 * pOuts, _float3 * pPoints)
{
	/* +x */
	XMStoreFloat4(&pOuts[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));
	/* -x */
	XMStoreFloat4(&pOuts[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), 
		XMLoadFloat3(&pPoints[0]), 
		XMLoadFloat3(&pPoints[3])));

	/* +y */
	XMStoreFloat4(&pOuts[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[1])));
	/* -y */
	XMStoreFloat4(&pOuts[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]),
		XMLoadFloat3(&pPoints[2]),
		XMLoadFloat3(&pPoints[6])));
	/* +z */
	XMStoreFloat4(&pOuts[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[7])));
	/* -z */
	XMStoreFloat4(&pOuts[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]),
		XMLoadFloat3(&pPoints[1]),
		XMLoadFloat3(&pPoints[2])));
	
}

void CFrustum::Free()
{

}
