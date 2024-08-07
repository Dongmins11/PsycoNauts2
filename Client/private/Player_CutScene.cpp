#include "stdafx.h"
#include "..\Public\Player_CutScene.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "CollisionObject.h"
#include "Scene_Loading.h"
#include "Player.h"
CPlayer_CutScene::CPlayer_CutScene(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer_CutScene::CPlayer_CutScene(const CPlayer_CutScene & rhs)
	: CGameObject(rhs)

{
}

HRESULT CPlayer_CutScene::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayer_CutScene::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	m_iCurScene = *(_uint*)pArg;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-155.79f, 21.897f, 155.632f, 1.f));
	//m_pTransformCom->RotationByRadian(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(90.f));
	m_isRender = false;
	

	m_iAnimIndex = 0; 

	m_pModelCom->Set_Animation(m_iAnimIndex);

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	return S_OK;
}

_int CPlayer_CutScene::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (SCENE_DEMO == m_iCurScene)
	{
		if (false == m_IsAnimationPlay)
			return 0;


		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);


		if (pGameInstance->Key_Down(VK_F4))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-155.79f, 21.897f, 155.632f, 1.f));
			m_pTransformCom->RotationByRadian(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(0.f));

			Set_Animation_BlendSpeed(TOTU_END);

			m_lfRadianValue = 90.0;
			m_lfTimeAcc = 0.0;
			m_iAnimIndex = 0;
			m_iPunchCount = 0;
			m_dDecreaseY = 0.0;

			m_IsAnimationPlay = true;
		}


		if (SCENE_DEMO == pGameInstance->Get_CurrentSceneIndex() && true == m_IsAnimationPlay)
			Stage_Tuto_CutScene(TimeDelta);


		Safe_Release(pGameInstance);
	}

	if (SCENE_STAGE02 == m_iCurScene && false == m_IsAnimationPlay_StageTwo)
		return 0;

	m_pModelCom->Lerp_Update(TimeDelta*m_dAnimSpeed);

	return 0;
}

_int CPlayer_CutScene::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;


	//if (!m_isRender)
		//return 0;
	if (SCENE_DEMO == m_iCurScene && false == m_IsAnimationPlay)
		return 0;

	if (SCENE_STAGE02 == m_iCurScene && false == m_IsAnimationPlay_StageTwo)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CPlayer_CutScene::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(m_iRenderPass)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_CutScene::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CModel::NODENAMEDESC tNodeDesc;
	ZeroMemory(&tNodeDesc, sizeof(CModel::NODENAMEDESC));

	if (SCENE_DEMO == m_iCurScene)
	{
		tNodeDesc.szFinderBoneName = "Raz_LOBO_WholeBody.ao";
		tNodeDesc.szBottomBoneName = "hipRoot";
		tNodeDesc.szRootBoneName = "Root";

		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Player_CutScene"), TEXT("Com_Model"), (CComponent**)&m_pModelCom, &tNodeDesc)))
			return E_FAIL;
	}
	else if (SCENE_STAGE02 == m_iCurScene)
	{
		tNodeDesc.szFinderBoneName = "Raz_LOBO_WholeBody.ao";
		tNodeDesc.szBottomBoneName = "hipRoot";
		tNodeDesc.szRootBoneName = "Root";

		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Player_CutScene_Two"), TEXT("Com_Model"), (CComponent**)&m_pModelCom, &tNodeDesc)))
			return E_FAIL;
	}
	
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayer_CutScene::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	_float4x4 TransPoseWorldMat, ViewTransPoseMat, ProjectionTransPoseMat;

	XMStoreFloat4x4(&TransPoseWorldMat, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewTransPoseMat, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjectionTransPoseMat, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	if (FAILED(m_pModelCom->Set_RawValue("g_WorldMatrix", &TransPoseWorldMat, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewTransPoseMat, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjectionTransPoseMat, sizeof(_float4x4))))
		return E_FAIL;
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer_CutScene::Stage_Tuto_CutScene(_double TimeDelta)
{
	Anim_Playing(TimeDelta);
	

	Lerp_Check();
	
	return S_OK;
}

void CPlayer_CutScene::Anim_Playing(_double TimeDelta)
{
	if (nullptr == m_pModelCom)
		return;


	switch (m_iAnimIndex)
	{
	case PLAYER_TUTO_ANIM::TOTU_ONE:

		Animation_IndexCheck(TOTU_ONE);

		break;
	case PLAYER_TUTO_ANIM::TOTU_TWO:

		if (1==m_iPunchCount)
		{
			m_bLerpCheck = true;
			m_iAnimIndex = TOTU_TWO + 1;
		}
		break;
	case PLAYER_TUTO_ANIM::TOTU_THREE:

		//if (m_pModelCom->Get_IsAnimFinishied(TOTU_THREE))
		//{
		//	m_bLerpCheck = true;
		//	m_iAnimIndex = TOTU_THREE +  2;
		//}
		if (2 <= m_iPunchCount)
		{
			m_bLerpCheck = true;
			m_iAnimIndex = TOTU_FOR + 1;
			XMStoreFloat3(&m_vBackUpPos, m_pTransformCom->Get_Position());
		}
		if (180.f >= m_lfRadianValue)
		{
			m_lfRadianValue += TimeDelta * 120.0;
			m_pTransformCom->RotationByRadian(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians((_float)m_lfRadianValue));
		}
		else
			m_pTransformCom->Go_Straight(TimeDelta * 0.1);

		break;
	case PLAYER_TUTO_ANIM::TOTU_FOR:

		//if (5 <= m_pModelCom->Get_AnimationCurrentFrame(TOTU_FOR))
		//{
		//	m_bLerpCheck = true;
		//	m_iAnimIndex = TOTU_FOR + 1;
		//}
		if (2<=m_iPunchCount)
		{
			m_bLerpCheck = true;
			m_iAnimIndex = TOTU_FOR + 1;
			XMStoreFloat3(&m_vBackUpPos, m_pTransformCom->Get_Position());
		}
		m_pTransformCom->Go_Straight(TimeDelta * 0.4);

		break;
	case PLAYER_TUTO_ANIM::TOTU_FIVE:
		m_dEndTimeAcc += TimeDelta;
		if (3.0 < m_dEndTimeAcc)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->Open_Scene(SCENE_LOADING, CScene_Loading::Create(m_pDevice, m_pDeviceContext, SCENE_STAGE01));
			dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_IsAnimUpdate(true);

			RELEASE_INSTANCE(CGameInstance);
		}
		if (0.2 >= m_lfTimeAcc)
		{
			m_lfTimeAcc += TimeDelta*0.8 - TimeDelta*TimeDelta*0.8;
			
			_float4 vMyPosition;
			XMStoreFloat4(&vMyPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			vMyPosition.y += (_float)m_lfTimeAcc;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vMyPosition));
		}
		else
		{
			m_dDecreaseY += TimeDelta;
			_float4 vMyPosition;
			XMStoreFloat4(&vMyPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			vMyPosition.y -= (_float)m_dDecreaseY*(_float)m_dDecreaseY;

			if (m_vBackUpPos.y >= vMyPosition.y)
			{
				vMyPosition.y = m_vBackUpPos.y;
				m_iCurrentScene = TOTU_END;
				m_dDecreaseY = 0.0;
			}
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vMyPosition));
		}
		m_pTransformCom->Go_Straight(TimeDelta * 0.4);
		break;
	case PLAYER_TUTO_ANIM::TOTU_END:
		break;
	}

	m_pModelCom->Set_Animation(m_iAnimIndex);
}

void CPlayer_CutScene::Scene_Move(_double TimeDelta)
{

}

void CPlayer_CutScene::Lerp_Check()
{
	_bool	bLerpCheck = false;

	if (true == m_bLerpCheck)
	{
		bLerpCheck = true;
		m_bLerpCheck = false;
	}

	if (true == bLerpCheck)
		m_pModelCom->Set_IsAnimLefp(bLerpCheck);

}

void CPlayer_CutScene::Animation_IndexCheck(_uint iAnimIndex)
{
	if (m_pModelCom->Get_IsAnimFinishied(iAnimIndex))
	{
		m_bLerpCheck = true;
		m_iAnimIndex = iAnimIndex + 1;
	}
}

void CPlayer_CutScene::Set_Animation_BlendSpeed(_uint iMaxIndex)
{
	for (_uint i = 0; i < iMaxIndex - 1; ++i)
		m_pModelCom->Set_AnimBlendSpeed(i, 6.0);
}



CPlayer_CutScene * CPlayer_CutScene::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer_CutScene*	pInstance = new CPlayer_CutScene(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlayer_CutScene");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_CutScene::Clone(void* pArg, _uint iSceneIndex)
{
	CPlayer_CutScene*	pInstance = new CPlayer_CutScene(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CPlayer_CutScene");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_CutScene::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);


}
