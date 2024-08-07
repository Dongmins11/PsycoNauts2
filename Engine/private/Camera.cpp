#include "..\public\Camera.h"
#include "Transform.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
	, m_pPipeLine(CPipeLine::Get_Instance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::NativeConstruct_Prototype()
{
	

	return S_OK;
}

HRESULT CCamera::NativeConstruct(void * pArg, _uint iSceneIndex)
{
	if (FAILED(__super::NativeConstruct(pArg, iSceneIndex)))
		return E_FAIL;



	return S_OK;
}

_int CCamera::Tick(_float fTimeDelta)
{


	return _int();
}

_int CCamera::LateTick(_float fTimeDelta)
{
	return _int();
}

HRESULT CCamera::Render()
{
	return S_OK;
}

HRESULT CCamera::SetUp_Component(_uint iSceneIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent ** ppOut, void * pArg)
{
	if (FAILED(__super::SetUp_Component(iSceneIndex, pPrototypeTag, pComponentTag, ppOut, pArg)))
		return E_FAIL;

	return S_OK;
}



void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTransform);

}
