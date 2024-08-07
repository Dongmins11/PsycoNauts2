#include "stdafx.h"
#include "..\public\Zip.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "CollisionObject.h"

CZip::CZip(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CZip::CZip(const CZip & rhs)
	: CGameObject(rhs)
{
}

HRESULT CZip::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CZip::NativeConstruct(void * pArg)
{
	_vector Postion;
	if (nullptr != pArg)
		Postion = *(_vector*)pArg;


	m_StrLayerName = TEXT("Layer_Zip");
	m_StrModelName = TEXT("Prototype_Component_Model_Zip0");
	m_StrObjectFullName = TEXT("Prototype_GameObject_Layer_Zip");
	m_eLayerType = LayerType::LAYERTYPE_ZIP;

	_float3 vColSize = _float3(0.5f, 0.5f, 0.5f);
	_float3 vColPos = _float3(0.f, 0.f, 0.f);
	m_fRangeForFrustum = MaxScaleFromFloat3(vColSize);
	m_fRangeForFrustum = 1.f;
	_float3 vObjScale;
	if (FAILED(SetUp_Component()))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Postion + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
	m_iRenderPass = 10;
	m_iRenderPassBackUp = m_iRenderPass;

	m_isRender = true;
	m_bIsRenderingShadow = true;

	for (_uint i = 0; i < 3; ++i)
		m_ArrKey[i] = 0.f;

	return S_OK;
}
_int CZip::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_RenderTimer += TimeDelta;

	//if (1.0 < m_RenderTimer)
	//	m_RenderCheck = true;

	if (4.0 < m_RenderTimer && true == m_OnesCreate)
	{
		CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
		m_OnesCreate = false;
		static_cast<CCamera_Fly*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Cloud_CAM(CCamera_Fly::CLOUD_7);
		RELEASE_INSTANCE(CGameInstance);
	}


	Dissolve(TimeDelta);

	Blending(TimeDelta);

	if (m_Endblending)
		End_Blending(TimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), m_fRangeForFrustum))
	{
		_int iPlayerCurlingIndex = 0;
		_int iDeleteCurlingIndex = 0;
		if (Setting_CurlingIndex(&iPlayerCurlingIndex, &iDeleteCurlingIndex))
		{
			if (m_iDeleteCurlingIndex == iDeleteCurlingIndex)
			{
				m_isDestroy = true;
				RELEASE_INSTANCE(CGameInstance);
				return 0;
			}
			Stage_RenderBranch(iPlayerCurlingIndex);
		}
	}
	else
	{
		if (m_isRender && m_pColliderCom)
		{
			m_pColliderCom->Remove_Actor();
		}
		m_isRender = false;
	}
	RELEASE_INSTANCE(CGameInstance);
	if (!m_isRender)
		return 0;

	m_pTransformCom->RotationPerSec(m_pTransformCom->Get_State(CTransform::STATE_UP), TimeDelta);
	/* 빙글 빙글 회전 */

	if (m_pColliderCom)
		m_pColliderCom->Update_TransformPhysX();

	CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
	
	
	/*if (true == pGameInstance->Key_Down(VK_NUMPAD4))
		m_BlendingTimer = 0.f;

	if (true == pGameInstance->Key_Pressing(VK_NUMPAD3))
	{
		m_BlendingTimer += TimeDelta;

		if (1.0f <= m_BlendingTimer)
			m_BlendingTimer = 1.0f;
	}*/

	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

_int CZip::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if (m_RenderCheck && false == m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONSHADOW_LAST, this);
	return 0;
}

HRESULT CZip::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}

void CZip::Blending(_double TimeDelta)
{
	if (true == m_bBlending)
	{
		m_BlendingTimer +=(_float)(TimeDelta * 0.5);

		if (0.5f > m_BlendingTimer)
		{
			if (FAILED(m_pModelCom->Set_RawValue("g_fBlendingTime", &m_BlendingTimer, sizeof(_float))))
				return;
		}
		else
		{
			m_BlendingTimer = 0.5f;
			m_bBlending = false;
		}
	}
}

void CZip::End_Blending(_double TimeDelta)
{
	m_BlendingTimer += TimeDelta * 0.5;
	//m_iRenderPass = 10;

	if (1.0f <= m_BlendingTimer)
		m_BlendingTimer = 1.0f;

	//if (FAILED(m_pModelCom->Set_RawValue("g_fBlendingTime", &m_BlendingTimer, sizeof(_float))))
	//	return;

	if (1.0f <= m_BlendingTimer)
	{
		/* 잘 안됌 이유 모르겠음 */
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Cloud_CAM(CCamera_Fly::CLOUD_14);
		RELEASE_INSTANCE(CGameInstance);
		m_iRenderPass = 0;
		m_BlendingTimer = 1.0f;
		m_Endblending = false;
	}
}

void CZip::Dissolve(_double TimeDelta)
{
	if (true == m_bIsDissolve && false == m_isDestroy)
	{
		m_isUpdateCollide = false;
		m_fDissolveTime += TimeDelta*m_dDissolveSpeed;
		if (1.f < m_fDissolveTime)
		{
			m_fDissolveTime = 0.f;
			m_bIsDissolve = false;
			m_isDestroy = true;

			if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float))))
				return;
			if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveStart", &m_fDissolveStart, sizeof(_float))))
				return;
			if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &m_bIsDissolve, sizeof(_bool))))
				return;

			CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);

			static_cast<CCamera_Fly*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Cloud_CAM(CCamera_Fly::CLOUD_16); /* 디졸브가 끝나면 부른다 */
			static_cast<CCamera_Fly*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_CLOUD);

			RELEASE_INSTANCE(CGameInstance);
		}
	}
	else
	{
		_bool bCheck = false;
		_float fTime = 0.f;
		_float fStartTime = 0.1f;
		if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveTime", &fTime, sizeof(_float))))
			return;
		if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveStart", &fStartTime, sizeof(_float))))
			return;
		if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &bCheck, sizeof(_bool))))
			return;
	}
	m_bBackUpDissolve = m_bIsDissolve;
}

void CZip::Set_Dissolve(_bool bIsDissolve, _double dSpeed, _uint iTextureIndex, _float fStart, _float fScaleSpeed)
{
	m_bIsDissolve = bIsDissolve;
	m_dDissolveSpeed = dSpeed;
	m_iTextureIndex = iTextureIndex;
	m_fDissolveStart = fStart;
	m_fDissolveScaleSpeed = fScaleSpeed;
	if (false == m_bBackUpDissolve && true == m_bIsDissolve)
		m_vDissolveScale = m_vBackUpScale;
}

void CZip::Fluctuate(_double TimeDelta)
{
}

void CZip::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
}

void CZip::Setting_Throwing(_fvector vDir)
{
}

void CZip::OnCollisionEnter(CCollisionObject & collision)
{
}

void CZip::OnCollisionStay(CCollisionObject & collision)
{
}

void CZip::OnCollisionExit(CCollisionObject & collision)
{
}

void CZip::OnTriggerEnter(CCollisionObject & collision)
{
	/* 플레이어랑 충돌이 된다면 */
	//OutputDebugStringW(TEXT("Zip - 충돌됌\n"));
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_TRIGGER)
	{
		if (false == m_isDestroy)
		{
			if (false == m_bIsDissolve)
				Set_Dissolve(true, 1.4, 3, 0.1f, 0.5f);
		}
	}
}

void CZip::SettingActorActive()
{
}

_bool CZip::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* Player = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	if (Player)
	{
		*pCurlingIndex = Player->Get_CurlingIndex();
		*pDeleteIndex = Player->Get_Delete_CurlingIndex();
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}
	else
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}
}

void CZip::Stage_RenderBranch(_int iPlayerCurlingIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_iCurrentScene)
	{
	case SCENE_STAGE01:
	{
		if (iPlayerCurlingIndex == 0) /* 0번 것만 출력 할것 */
		{
			if (iPlayerCurlingIndex == m_iCurlingIndex/* || NextIndex == m_iCurlingIndex*/)
			{
				if (!m_isRender && m_pColliderCom)
					m_pColliderCom->Add_Actor();
				m_isRender = true;
			}
			else
			{
				if (m_isRender && m_pColliderCom)
				{
					m_pColliderCom->Remove_Actor();
				}
				m_isRender = false;
			}
		}
		else if (iPlayerCurlingIndex == 1)/* 0 ~ 1 출력 할것 */
		{
			_uint  OldIndex = iPlayerCurlingIndex - 1;
			if (iPlayerCurlingIndex == m_iCurlingIndex || OldIndex == m_iCurlingIndex)
			{
				if (!m_isRender && m_pColliderCom)
					m_pColliderCom->Add_Actor();
				m_isRender = true;
			}
			else
			{
				if (m_isRender && m_pColliderCom)
				{
					m_pColliderCom->Remove_Actor();
				}
				m_isRender = false;
			}
		}
		else if (iPlayerCurlingIndex == 2)/* 1 출력 할것 */
		{
			_uint curIndex = iPlayerCurlingIndex - 1;
			if (curIndex == m_iCurlingIndex)
			{
				if (!m_isRender && m_pColliderCom)
					m_pColliderCom->Add_Actor();
				m_isRender = true;
			}
			else
			{
				if (m_isRender && m_pColliderCom)
				{
					m_pColliderCom->Remove_Actor();
				}
				m_isRender = false;
			}
		}
		break;
	}
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CZip::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_StrModelName.c_str(), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;

	tColliderDesc.isGravity = false;
	tColliderDesc.isTrigger = true;
	tColliderDesc.isSceneQuery = true;
	tColliderDesc.fRestitution = 0.f;
	tColliderDesc.pGameObject = this;
	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = MULTI_FLOAT3(_float3(0.5f, 0.5f, 0.5f), 1.99f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	_matrix smatPviot = XMMatrixTranslation(0.f, 0.f, 0.f);
	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Set_ShapeLayer(m_eLayerType);
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();

	m_isActorActive = false;
	SettingActorActive();

	/* Com_Texture*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CZip::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;
	_float fLightRange = 30.f;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr != m_pRendererCom)
	{
		m_vRGBA = m_pRendererCom->Get_RimLightColorFloat4();
		m_fRimPower = m_pRendererCom->Get_RimLightPower();
	}
	_bool		bMotionBlur = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_MOTIONBLUR);
	_bool		bShadow = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_SHADOW);
	_bool		bRimlight = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_RIMLIGHT);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));



	if (true == m_bIsDissolve)
	{
		if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveStart", &m_fDissolveStart, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &m_bIsDissolve, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_TextureValue("g_DissolveTexture", m_pTextureCom->Get_SRV(m_iTextureIndex))))
			return E_FAIL;
	}

	if (FAILED(m_pModelCom->Set_RawValue("g_fBlendingTime", &m_BlendingTimer, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CZip::OnControllerCollisionHit(CCollisionObject & collision)
{
}

CZip * CZip::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CZip*	pInstance = new CZip(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CZip");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CZip::Clone(void * pArg, _uint iSceneIndex)
{
	CZip*	pInstance = new CZip(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CZip");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CZip::Free()
{
	__super::Free();

	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
