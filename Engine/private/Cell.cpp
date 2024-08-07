#include "..\Public\Cell.h"
#include "VIBuffer_LineStrip.h"
#include "VIBuffer_Triangle.h"
#include "GameInstance.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CCell::NativeConstruct(NAVIMESHDESC& NaviDesc, _uint iIndex)
{	
	ZeroMemory(&m_NaviDesc, sizeof(NAVIMESHDESC));
	m_NaviDesc = NaviDesc;

	memcpy(m_vOriginalPoints, m_NaviDesc.vPoint, sizeof(_float3) * POINT_END);
	memcpy(&m_iOption, &m_NaviDesc.iByOption, sizeof(_uint));

	ZeroMemory(m_vPoints, sizeof(_float3) * POINT_END);
	ZeroMemory(m_pNeighbor, sizeof(CCell*) * LINE_END);
	
	m_iIndex = iIndex;


#ifdef _DEBUG
	if (FAILED(Ready_Debug_Buffer()))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

HRESULT CCell::Change_Pos(POINT ePoint, _float3 vNewPos)
{
#ifdef _DEBUG
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	//XMStoreFloat3(&vNewPos, XMLoadFloat3(&m_vOriginalPoints[ePoint]) + XMLoadFloat3(&vNewPos));

	m_pVIBuffer->UpdateSub(ePoint, vNewPos);
#endif // _DEBUG

	return S_OK;
}


_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
	if (XMVector3Equal(Get_Point(CCell::POINT_A), vSourPoint))
	{
		if (XMVector3Equal(Get_Point(CCell::POINT_B), vDestPoint))
			return true;
		
		if (XMVector3Equal(Get_Point(CCell::POINT_C), vDestPoint))
			return true;	
	}

	if (XMVector3Equal(Get_Point(CCell::POINT_B), vSourPoint))
	{
		if (XMVector3Equal(Get_Point(CCell::POINT_C), vDestPoint))
			return true;

		if (XMVector3Equal(Get_Point(CCell::POINT_A), vDestPoint))
			return true;
	}

	if (XMVector3Equal(Get_Point(CCell::POINT_C), vSourPoint))
	{
		if (XMVector3Equal(Get_Point(CCell::POINT_A), vDestPoint))
			return true;

		if (XMVector3Equal(Get_Point(CCell::POINT_B), vDestPoint))
			return true;
	}
	
	return false;
}

void CCell::Update(_fmatrix TransformMatrix)
{
	for (_uint i = 0; i < POINT_END; ++i)
	{
		XMStoreFloat3(&m_vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vOriginalPoints[i]), TransformMatrix));		
	}	

	XMStoreFloat3(&m_vLines[LINE_AB], XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	XMStoreFloat3(&m_vLines[LINE_BC], XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	XMStoreFloat3(&m_vLines[LINE_CA], XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));

}


_bool CCell::isIn(_fvector vPosition, CCell** ppNeighbor, _uint * iLineIndex)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector	vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
		_vector	vNormal = XMVector3Normalize(XMVectorSet(m_vLines[i].z * -1.f, 0.f, m_vLines[i].x, 0.f));

		if (0 < XMVectorGetX(XMVector3Dot(vDir, vNormal)))
		{
			*ppNeighbor = m_pNeighbor[i];
			if(nullptr != iLineIndex)
				*iLineIndex = i;
			return false;
		}
	}

	return true;
}


#ifdef _DEBUG
HRESULT CCell::Render(_uint iPassIndex)
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pVIBuffer->Set_RawValue("g_WorldMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)), sizeof(_matrix));

	_float4 vColor;
	if (0 == m_iOption)
		vColor = _float4(0.f, 0.f, 0.f, 1.f);
	else if(1 == m_iOption)
		vColor = _float4(0.f, 1.f, 0.f, 0.5f);
	else if (2 == m_iOption)
		vColor = _float4(0.f, 0.f, 1.f, 0.5f);
	else if (3 == m_iOption)
		vColor = _float4(1.f, 1.f, 0.f, 0.5f);
	else if (4 == m_iOption)
		vColor = _float4(0.f, 1.f, 1.f, 0.5f);

	m_pVIBuffer->Set_RawValue("g_Rgba", &vColor, sizeof(_float4));

	m_pVIBuffer->Render(iPassIndex);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}
#endif // _DEBUG

#ifdef _DEBUG

HRESULT CCell::Ready_Debug_Buffer()
{
	_float3		vPoints[4];

	vPoints[0] = m_vOriginalPoints[POINT_A];
	vPoints[1] = m_vOriginalPoints[POINT_B];
	vPoints[2] = m_vOriginalPoints[POINT_C];
	vPoints[3] = m_vOriginalPoints[POINT_A];

	m_pVIBuffer = CVIBuffer_Triangle::Create(m_pDevice, m_pDeviceContext, vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	return S_OK;
}
#endif // _DEBUG

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, NAVIMESHDESC& NaviDesc, _uint iIndex)
{
	CCell*	pInstance = new CCell(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(NaviDesc, iIndex)))
	{
		MSGBOX("Failed to Created CCell");

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{	

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif // _DEBUG

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
