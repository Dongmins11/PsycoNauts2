#include "stdafx.h"
#include "..\public\Player_2D.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "CollisionObject.h"
#include "Key_2D.h"

CPlayer_2D::CPlayer_2D(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)

{
}

CPlayer_2D::CPlayer_2D(const CPlayer_2D & rhs)
	: CGameObject(rhs)

{
}

HRESULT CPlayer_2D::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_2D::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_fRangeForFrustum = 1.f;

	m_pTransformCom->Scaling(_float3(0.2f, 0.2f, 0.1f));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pPlaneTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STAGE02, TEXT("Layer_Plane_2D"), TEXT("Com_Transform"), 0);
	if (nullptr == pPlaneTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(pPlaneTransform->Get_State(CTransform::STATE_RIGHT)) * 0.25f);
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(pPlaneTransform->Get_State(CTransform::STATE_UP)) * 0.25f);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(pPlaneTransform->Get_State(CTransform::STATE_LOOK)) * 0.1f);

	_matrix RotMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.f), XMConvertToRadians(-90.f), XMConvertToRadians(0.f));

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(m_pTransformCom->Get_Scale());
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(m_pTransformCom->Get_Scale());
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetZ(m_pTransformCom->Get_Scale());

	vRight = XMVector3TransformNormal(vRight, RotMatrix);
	vUp = XMVector3TransformNormal(vUp, RotMatrix);
	vLook = XMVector3TransformNormal(vLook, RotMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	RELEASE_INSTANCE(CGameInstance);

	m_vInitialPos = _float3(3.13f, 1.7f, 43.f);

	m_pTransformCom->Set_State_Float3(CTransform::STATE_POSITION, m_vInitialPos);

	m_isRender = false;

	m_IsKeyMove = true;

	return S_OK;
}

_int CPlayer_2D::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_IsTriggerRender)
		m_isRender = true;
	else
		m_isRender = false;
	
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0));
	if (nullptr == pPlayer)
	{
		RELEASE_INSTANCE(CGameInstance);
		return -1;
	}

	CTransform* pPlaneTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STAGE02, TEXT("Layer_Plane_2D"), TEXT("Com_Transform"), 0);
	if (nullptr == pPlaneTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (false == pPlayer->Get_IsAnimUpdate() && true == m_IsKeyMove)
	{
		if (pGameInstance->Key_Pressing('D'))
		{
			m_pTransformCom->Go_Right(TimeDelta * 0.1f);
		}

		if (pGameInstance->Key_Pressing('A'))
		{
			m_pTransformCom->Go_Right(-TimeDelta * 0.1f);
		}

		if (pGameInstance->Key_Pressing('W'))
			m_pTransformCom->Go_ByAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), TimeDelta * 0.1f, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		if (pGameInstance->Key_Pressing('S'))
			m_pTransformCom->Go_ByAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), -(TimeDelta * 0.1f), m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}

	if (m_pColliderCom)
		m_pColliderCom->Update_TransformPhysX();

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int CPlayer_2D::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_IsCollision)
	{
		pGameInstance->StopSound(CHANNELID::PLAYER);
		pGameInstance->PlaySoundOnce(TEXT("2D_Player_Deaths.wav"), CHANNELID::PLAYER);

		m_IsCollisionDissolve = true;
		m_IsKeyMove = false;
		for (_uint i = 0; i < 3; i++)
		{
			CKey_2D* pKey = dynamic_cast<CKey_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Key_2D"), i));
			if (nullptr == pKey)
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			pKey->Set_IsPlayer_Collision(true);
		}
		m_IsCollision = false;
		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();

	}
	else
	{
		for (_uint i = 0; i < 3; i++)
		{
			CKey_2D* pKey = dynamic_cast<CKey_2D*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_Key_2D"), i));
			if (nullptr == pKey)
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			pKey->Set_IsPlayer_Collision(false);
		}
	}

	if (true == m_IsCollisionDissolve)
	{
		if (!m_isInteraction)
		{
			m_isInteraction = true;
			/* For. Layer_BlueSpark 이펙트 생성 */
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_2DStarParticle"),
				SCENE_STATIC, TEXT("Effect_2DStarParticle"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			/* For. Layer_BlueSpark 상속 */
			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_2DStarParticle"), m_pTransformCom->Get_Position(), true)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
		}
		m_dCollisionDissolveAcc += TimeDelta;
	}

	if (0.5 <= m_dCollisionDissolveAcc)
	{
		m_isInteraction = false;
		m_IsCollisionDissolve = false;
		m_dCollisionDissolveAcc = 0.0;
	}

	Dissolve(TimeDelta);

	if (m_pColliderCom)
		m_pColliderCom->Update_Transform();
	

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

HRESULT CPlayer_2D::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if(m_isRender)
		m_pModelCom->Render(40);
	return S_OK;
}

void CPlayer_2D::OnTriggerEnter(CCollisionObject & collision)
{
	if (LayerType::LAYERTYPE_2DMAP == collision.Get_Object()->Get_ObjectLayerType())
	{
		m_IsCollision = true;
		//OutputDebugString(TEXT("충돌"));
	}

	if (LayerType::LAYERTYPE_2D_MONSTER == collision.Get_Object()->Get_ObjectLayerType())
	{
		m_IsCollision = true;
		//OutputDebugString(TEXT("몬스터충돌"));
	}
}

void CPlayer_2D::Dissolve(_double TimeDelta)
{
	if (true == m_IsCollisionDissolve)
		Set_Dissolve(true, 2, 3, 0.1f, 0.5f);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (true == m_bIsDissolve)
	{
		m_fDissolveTime += TimeDelta*m_dDissolveSpeed;
		if (1.f < m_fDissolveTime)
		{
			m_fDissolveTime = 0.f;
			m_bIsDissolve = false;
			m_isRender = true;
			m_pTransformCom->Set_State_Float3(CTransform::STATE_POSITION, m_vInitialPos);
			m_IsKeyMove = true;
			m_pColliderCom->Update_TransformPhysX();
			m_pColliderCom->Update_Transform();

			if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float))))
				return;
			if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveStart", &m_fDissolveStart, sizeof(_float))))
				return;
			if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &m_bIsDissolve, sizeof(_bool))))
				return;

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

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer_2D::Set_Dissolve(_bool bIsDissolve, _double dSpeed, _uint iTextureIndex, _float fStart, _float fScaleSpeed)
{
	m_bIsDissolve = bIsDissolve;
	m_dDissolveSpeed = dSpeed;
	m_iTextureIndex = iTextureIndex;
	m_fDissolveStart = fStart;
	m_fDissolveScaleSpeed = fScaleSpeed;
	if (false == m_bBackUpDissolve && true == m_bIsDissolve)
		m_vDissolveScale = m_vBackUpScale;
}

HRESULT CPlayer_2D::SetUp_Component()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	///* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STAGE02, TEXT("Prototype_Component_Texture_Player_2D"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTextureCom)))
		return E_FAIL;

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isGravity = false;
	tColliderDesc.isSceneQuery = true;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isTrigger = true;								// Trigger ?

	m_eLayerType = LayerType::LAYERTYPE_2D_PLAYER;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(0.1f, 0.1f, 0.3f);	// scale이 지름값을 가지므로 scale * 2 해야한다.

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_matrix smatPviot = XMMatrixTranslation(m_pTransformCom->Get_State_Float3(CTransform::STATE_POSITION).x, m_pTransformCom->Get_State_Float3(CTransform::STATE_POSITION).y, m_pTransformCom->Get_State_Float3(CTransform::STATE_POSITION).z);
	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Set_ShapeLayer(m_eLayerType);
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer_2D::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pModelCom, 0);

	if (true == m_bIsDissolve)
	{
		if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveStart", &m_fDissolveStart, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &m_bIsDissolve, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Set_TextureValue("g_DissolveTexture", m_pDissolveTextureCom->Get_SRV(m_iTextureIndex))))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CPlayer_2D * CPlayer_2D::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer_2D* pInstance = new CPlayer_2D(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlayer_2D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_2D::Clone(void * pArg, _uint iSceneIndex)
{
	CPlayer_2D* pInstance = new CPlayer_2D(*this);
	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CPlayer_2D");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CPlayer_2D::Free()
{
	__super::Free();

	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pColliderCom);
}
