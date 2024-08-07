#include "stdafx.h"
#include "..\public\SkyBox.h"
#include "GameInstance.h"
#include "Scene_Stage03.h"
CSkyBox::CSkyBox(ID3D11Device * _pGraphic_Device, ID3D11DeviceContext * _pGraphic_DeviceContext)
	:CGameObject(_pGraphic_Device, _pGraphic_DeviceContext)
{
}

CSkyBox::CSkyBox(const CSkyBox & _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CSkyBox::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyBox::NativeConstruct(void * _pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(_pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_vRGBA = _float4(0.f, 0.f, 0.f, 0.f);
	m_StrLayerName = TEXT("SkyBox");
	//if (FAILED(SetUp_TargetWorldMatrix()))
	//	return E_FAIL;

	m_isRender = false;

	return S_OK;
}

_int CSkyBox::Tick(_double _dTimeDelta)
{
	if (0 > __super::Tick(_dTimeDelta))
		return -1;

	if (nullptr == m_pTransform)
		return -1;
	//_vector vTargetPos = XMLoadFloat4(&*(_float4*)&m_pTargetWorldMatrix->m[3]);

	//m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _dTimeDelta*0.07);
	if (!m_isRender)
		return 0;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	
	//m_pTransform->Set_State(CTransform::STATE_POSITION, ((CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"), 0))->Get_Position());
	m_pTransform->Set_State(CTransform::STATE_POSITION, pGameInstance->Get_CamPosition());

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int CSkyBox::LateTick(_double _dTimeDelta)
{
	if (0 > __super::LateTick(_dTimeDelta))
		return -1;

	if (nullptr == m_pRenderer)
		return -1;

	if (!m_isRender)
		return 0;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (SCENE_STAGE03 != pGameInstance->Get_CurrentSceneIndex() && SCENE_STAGE04 != pGameInstance->Get_CurrentSceneIndex()&& SCENE_STAGE05 != pGameInstance->Get_CurrentSceneIndex() && SCENE_ENDING != pGameInstance->Get_CurrentSceneIndex())
	{
		if (FAILED(m_pRenderer->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
			return -1;
	}
	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CSkyBox::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pModel->Render(6)))
		return E_FAIL;
	return S_OK;
}

HRESULT CSkyBox::SetUp_Component()
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/*For.Renderer*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;
	if (nullptr == m_pRenderer)
		return E_FAIL;

	/*For.Transform*/
	CTransform::TRANSFORMDESC tTransformDest;
	ZeroMemory(&tTransformDest, sizeof(CTransform::TRANSFORMDESC));

	tTransformDest.SpeedPerSec = 5.0;
	tTransformDest.RotationPerSec = XMConvertToRadians(60.f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransform, &tTransformDest)))
		return E_FAIL;
	if (nullptr == m_pTransform)
		return E_FAIL;

	/*For.Model_SkyBox*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_SkyBox"), TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;
	if (nullptr == m_pModel)
		return E_FAIL;
	//RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CSkyBox::SetUp_ConstantTable()
{
	if (nullptr == m_pTransform)
		return E_FAIL;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/*For.Model*/
	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransform->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModel->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModel->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModel->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	RELEASE_INSTANCE(CGameInstance);

	
	//_float4 Minus_RGBA = _float4(-0.5f, -0.5f, -0.5f, 0.f);

	//if (FAILED(m_pModel->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
	//	return E_FAIL;
	return S_OK;
}

CSkyBox * CSkyBox::Create(ID3D11Device * _pGraphic_Device, ID3D11DeviceContext * _pGraphic_DeviceContext)
{
	CSkyBox* pInstance = new CSkyBox(_pGraphic_Device, _pGraphic_DeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed To Create CSkyBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkyBox::Clone(void * _pArg, _uint _eLevelIndex)
{
	CSkyBox* pInstance = new CSkyBox(*this);
	if (FAILED(pInstance->NativeConstruct(_pArg, _eLevelIndex)))
	{
		MSGBOX("Failed To Create CSkyBox Clone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkyBox::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
}
