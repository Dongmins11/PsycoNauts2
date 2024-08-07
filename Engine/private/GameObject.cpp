#include "..\public\GameObject.h"
#include "GameInstance.h"
//#include "Transform.h"
#include "Component.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

CComponent * CGameObject::Get_Components(const _tchar * pComponentTag)
{
	return Find_Component(pComponentTag);
}



HRESULT CGameObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	m_iCurrentScene = iCurrentScene;
	return S_OK;
}

_int CGameObject::Tick(_double TimeDelta)
{
	return _int();
}

_int CGameObject::LateTick(_double TimeDelta)
{
	return _int();
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

_bool CGameObject::RectPicking(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);

	if (m_tRect.left <= pt.x && m_tRect.right >= pt.x &&
		m_tRect.top <= pt.y && m_tRect.bottom >= pt.y)
		return true;

	return false;
}


void CGameObject::Compute_ViewZ(CTransform * pTransform)
{
	_vector		vPosition = pTransform->Get_State(CTransform::STATE_POSITION);

	_float4x4		ViewMatrix;
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	_vector vCamPos = pGameInstance->Get_CamPosition();


	m_fViewZ = XMVectorGetX(XMVector3Length(vPosition - vCamPos));

	RELEASE_INSTANCE(CGameInstance);
}

/* 맵에도 추가. 자식에도 추가. */
HRESULT CGameObject::SetUp_Component(_uint iSceneIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;	

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CComponent*		pComponent = pGameInstance->Clone_Component(iSceneIndex, pPrototypeTag, pArg);

	m_Components.insert(COMPONENTS::value_type(pComponentTag, pComponent));

	*ppOut = pComponent;

	Safe_AddRef(pComponent);
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
