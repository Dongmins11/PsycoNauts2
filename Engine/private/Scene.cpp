#include "..\public\Scene.h"

CScene::CScene(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CScene::NativeConstruct()
{
	return S_OK;
}

_int CScene::Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CScene::Render()
{
	return S_OK;
}

void CScene::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
