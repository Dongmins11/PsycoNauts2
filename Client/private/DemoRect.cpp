#include "stdafx.h"
#include "..\public\DemoRect.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "CollisionObject.h"
#include "Static_Mesh.h"
CDemoRect::CDemoRect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CDemoRect::CDemoRect(const CDemoRect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDemoRect::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CDemoRect::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	m_eLayerType = LayerType::LAYERTYPE_TRIGGER;
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_iRenderPass = 28;
	m_iRenderPassBackUp = m_iRenderPass;

	m_isRender = false;

	m_vBackUpPos = m_vPos = _float3(-116.09f, 3.002f, 107.241f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vPos));
	m_pTransformCom->Set_MyLook(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Scaling(_float3(9.945f, 8.63f, 1.f));

	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();

	m_fSourSpeed[0] = 0.2f;
	m_fDestSpeed[0] = 1.7f;

	m_fSourSpeed[1] = 0.2f;
	m_fDestSpeed[1] = 1.5f;

	m_fMaxTime[0] = 2.3f;
	m_fMaxTime[1] = 2.f;

	m_vScale = _float3(1.f, 1.f, 1.f);
	return S_OK;
}

_int CDemoRect::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;
	//if (GetKeyState('C') & 0x8000)
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vBackUpPos));
	//Update(TimeDelta);
	if (false == m_isRender)
		return 0;
	if (m_pColliderCom)
	{
		m_pColliderCom->Update_TransformPhysX();
	}

	return _int();
}

_int CDemoRect::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if (m_pColliderCom)
	{
		m_pColliderCom->Update_Transform();
	}
	if (false == m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	return 0;
}

HRESULT CDemoRect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pVIBufferRectCom->Render(m_iRenderPass);

	return S_OK;
}

void CDemoRect::OnTriggerEnter(CCollisionObject & collision)
{
	if (LayerType::LAYERTYPE_STATIC == collision.Get_Object()->Get_ObjectLayerType())
	{
		CStatic_Mesh* pStaticMesh = dynamic_cast<CStatic_Mesh*>(collision.Get_Object());
		if (nullptr == pStaticMesh)
			return;
		if (!lstrcmp(TEXT("Layer_Light3"), pStaticMesh->Get_LayerName()))
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->Set_LightRender(true, m_iLightCount);

			++m_iLightCount;
			RELEASE_INSTANCE(CGameInstance);
		}
		pStaticMesh->Set_Fluctuate();
		OutputDebugString(TEXT("렉트 이벤트 트리거 충돌!!!!"));
	}
}

void CDemoRect::Set_RectMove(RECTMOVE eRectMove)
{
	m_eRectMove = eRectMove;
	m_bIsLerp = true;
}

void CDemoRect::Set_Pos(_float3 vPos)
{
	m_vPos = vPos;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vPos));

	if (m_pColliderCom)
	{
		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();
	}

}

HRESULT CDemoRect::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBufferRect */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer_Rect"), (CComponent**)&m_pVIBufferRectCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_DEMO, TEXT("Prototype_Component_Texture_DemoRect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isTrigger = true;
	tColliderDesc.pGameObject = this;

	tColliderDesc.isSceneQuery = true;

	CSphereCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(15.f, 10.f, 1.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_pColliderCom->Set_ShapeLayer(m_eLayerType);



	return S_OK;
}

HRESULT CDemoRect::SetUp_ConstantTable()
{
	if (nullptr == m_pVIBufferRectCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pVIBufferRectCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferRectCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferRectCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferRectCom, 1);
	//m_pVIBufferRectCom->Set_TextureValue("g_DiffuseTexture", pGameInstance->Get_SRV(TEXT("Target_Specular")));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CDemoRect::Update(_double TimeDelta)
{
	switch (m_eRectMove)
	{
	case Client::CDemoRect::RECTMOVE_NORMAL:
	{
		m_dTimeAcc += (_float)TimeDelta;
		if (m_fMaxTime[RECTMOVE_NORMAL] < m_dTimeAcc)
		{
			m_dTimeAcc = 0.0;
			m_fRatio = 0.f;
			m_bIsLerp = true;
			m_eRectMove = RECTMOVE_END;
		}
		_float fSpeed = 0.f;
		if (true == m_bIsLerp)
		{
			m_fRatio += (_float)TimeDelta;
			if (1.f < m_fRatio)
			{
				m_bIsLerp = false;
				fSpeed = m_fDestSpeed[RECTMOVE_NORMAL];
				m_fRatio = 0.f;
			}

			fSpeed = (1 - m_fRatio)*m_fSourSpeed[RECTMOVE_NORMAL] + m_fRatio*m_fDestSpeed[RECTMOVE_NORMAL];
		}
		else
		{
			fSpeed = m_fDestSpeed[RECTMOVE_NORMAL];
		}
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		m_pTransformCom->Set_MyLook(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
		m_pTransformCom->Go_Straight(TimeDelta*fSpeed);
		m_pTransformCom->Set_MyLook(vLook);

	}
	break;
	case Client::CDemoRect::RECTMOVE_SPEEDUP:
	{
		m_dTimeAcc += (_float)TimeDelta;
		if (m_fMaxTime[RECTMOVE_SPEEDUP] < m_dTimeAcc)
		{
			m_dTimeAcc = 0.0;
			m_fRatio = 0.f;
			m_bIsLerp = true;
			m_eRectMove = RECTMOVE_END;
			//m_isDestroy = true;
		}

		_float fSpeed = 0.f;
		if (true == m_bIsLerp)
		{
			m_fRatio += (_float)TimeDelta;
			if (1.f < m_fRatio)
			{
				m_bIsLerp = false;
				fSpeed = m_fDestSpeed[RECTMOVE_SPEEDUP];
				m_fRatio = 0.f;
			}

			fSpeed = (1 - m_fRatio)*m_fSourSpeed[RECTMOVE_SPEEDUP] + m_fRatio*m_fDestSpeed[RECTMOVE_SPEEDUP];
		}
		else
		{
			fSpeed = m_fDestSpeed[RECTMOVE_SPEEDUP];
		}
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		m_pTransformCom->Set_MyLook(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
		m_pTransformCom->Go_Straight(TimeDelta*fSpeed);
		m_pTransformCom->Set_MyLook(vLook);

	}
	break;
	case Client::CDemoRect::RECTMOVE_END:
		break;
	default:
		break;
	}
}

CDemoRect * CDemoRect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDemoRect*	pInstance = new CDemoRect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CDemoRect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDemoRect::Clone(void * pArg, _uint iCurrentScene)
{
	CDemoRect*	pInstance = new CDemoRect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CDemoRect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDemoRect::Free()
{
	__super::Free();
	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferRectCom);
	Safe_Release(m_pTextureCom);
}
