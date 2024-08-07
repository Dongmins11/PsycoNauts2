#include "stdafx.h"
#include "..\public\MoveNpc.h"
#include "GameInstance.h"
#include "NpcDoor.h"

CMoveNpc::CMoveNpc(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CMoveNpc::CMoveNpc(const CMoveNpc & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMoveNpc::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMoveNpc::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	//m_AnimationState = STATE_IDLE;
	//m_pModelCom->Set_Animation(0);
	m_iRenderPass = 0;
	m_bMove[0] = true;
	
	_vector vLook = XMLoadFloat3(&m_vectorMove[0]) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_MyLook(vLook);
	return S_OK;
}

_int CMoveNpc::Tick(_double TimeDelta)
{
	if (true == m_bClose)
		return 0;
	if (!m_isRender)
		return 0;
	
	if (false == m_bStartMove)
		MoveIDLE(TimeDelta);
	if (true == m_bMove[0] && true == m_bStartMove)
		MoveWalk(TimeDelta,0);
	if (m_bEnd[0] == true)
		MoveLookCheck(TimeDelta);
	if (true == m_bMove[1])
		MoveWalk(TimeDelta, 1);
	if (m_bEnd[1] == true)
		MoveRoation(TimeDelta);
	if (m_bEnd[2] == true && m_bGoDoor == false)
		IDLEAnimation(TimeDelta);
	if (true == m_bGoDoor&& m_bClose == false)
		MoveWalkDoor(TimeDelta);

	//m_pTransformCom->Set_MyLook(XMLoadFloat3(&_float3(m_vectorMove[0].x,0, m_vectorMove[0].z)));
	//CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
	//if (pGameInstnace->Key_Pressing('Z'))
	//{
	//	if (2 == m_iMyCount)
	//	{
	//		m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 1.f, 0.f, 0.f), _float3(0.f, XMConvertToRadians(m_RotationY), 0.f));
	//		m_RotationY -= 1.f;
	//	}
	//}
	//if (pGameInstnace->Key_Pressing(VK_DOWN))
	//{
	//	if (0 == m_iMyCount)
	//	{
	//		m_bLerpCheck = true;
	//		m_bGoDoor = true;
	//	}
	//}
	//if (pGameInstnace->Key_Pressing(VK_LEFT))
	//{
	//	if (1 == m_iMyCount)
	//	{
	//		m_bLerpCheck = true;
	//		m_bGoDoor = true;
	//	}
	//}
	//
	//if (pGameInstnace->Key_Pressing(VK_RIGHT))
	//{
	//	if (2 == m_iMyCount)
	//	{
	//		m_bLerpCheck = true;
	//		m_bGoDoor = true;
	//	}
	//}
	//if (pGameInstnace->Key_Pressing(VK_UP))
	//{
	//	if (3 == m_iMyCount)
	//	{
	//		m_bLerpCheck = true;
	//		m_bGoDoor = true;
	//	}
	//}
	//
	////m_pModelCom->Lerp_Update(TimeDelta);
	//RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int CMoveNpc::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;
	if (!m_isRender)
		return 0;

	if (true == m_bClose)
		return 0;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CMoveNpc::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(m_iRenderPass)))
		return E_FAIL;

	return S_OK;
}

void CMoveNpc::MoveIDLE(_double TimeDelta)
{
	m_pModelCom->Lerp_Update(TimeDelta);
}

void CMoveNpc::MoveWalk(_double TimeDelta,_uint Index)
{
	_bool    bLerpCheck = false;

	if (true == m_bLerpCheck)
	{
		bLerpCheck = true;
		m_bLerpCheck = false;
	}
	if (true == bLerpCheck && true == m_oneStart)
	{
		m_oneStart = false;
		m_pModelCom->Set_IsAnimLefp(bLerpCheck);
		switch (m_iMyCount)
		{
		case 0:
			m_pModelCom->Set_Animation(2);
			m_pModelCom->Set_AnimBlendSpeed(2, 6.0);
			break;
		case 1:
			m_pModelCom->Set_Animation(3);
			m_pModelCom->Set_AnimBlendSpeed(3, 6.0);
			break;
		case 2:
			m_pModelCom->Set_Animation(2);
			m_pModelCom->Set_AnimBlendSpeed(2, 6.0);
			break;
		case 3:
			m_pModelCom->Set_Animation(2);
			m_pModelCom->Set_AnimBlendSpeed(2, 6.0);
			break;
		}
	}

	m_pTransformCom->Go_Straight(TimeDelta*0.6f);
	m_pModelCom->Lerp_Update(TimeDelta);
	if (0.1 >= XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat3(&m_vectorMove[Index]))))
	{
		m_bMove[Index] = false;
		m_bEnd[Index] = true;
		m_bLerpCheck = true;
	}
}

void CMoveNpc::MoveLookCheck(_double TimeDelta)
{
	_vector vLook = XMLoadFloat3(&m_vectorMove[1]) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_MyLook(vLook);
	m_bMove[1] = true;
	m_bEnd[0] = false;
}

void CMoveNpc::MoveRoation(_double TimeDelta)
{
	Time += 0.016f;
	if (1.00f >= Time)
	{
		switch (m_iMyCount)
		{
		case 0:
			XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(-29.f, 0.f, 0.f, 0.f), XMVectorSet(85.f, 0.f, 0.f, 0.f), Time));
			m_pTransformCom->Go_Straight(TimeDelta*0.15f);
			break;
		case 1:
			XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(37.f, 0.f, 0.f, 0.f), XMVectorSet(276.f, 0.f, 0.f, 0.f), Time));
			m_pTransformCom->Go_Straight(TimeDelta*0.2f);
			break;
		case 2:
			XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(-90.f, 0.f, 0.f, 0.f), XMVectorSet(90.f, 0.f, 0.f, 0.f), Time));
			m_pTransformCom->Go_Straight(TimeDelta*0.2f);
			break;
		case 3:
			XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(90.f, 0.f, 0.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), Time));
			m_pTransformCom->Go_Straight(TimeDelta*0.2f);
			break;
		}
		m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 0.f, 0.f), _float3(0.f, XMConvertToRadians(m_MoveRoation.x), 0.f));
		//m_pTransformCom->Go_Straight(TimeDelta*0.2f);
	}
	else
	{
		m_bEnd[1] = false;
		m_bEnd[2] = true;
	}
	m_pModelCom->Lerp_Update(TimeDelta);
}

void CMoveNpc::IDLEAnimation(_double TimeDelta)
{
	_bool    bLerpCheck = false;

	if (true == m_bLerpCheck)
	{
		bLerpCheck = true;
		m_bLerpCheck = false;
		m_pModelCom->Set_Animation(0);
		Time = 0.0;
	}

	if (true == bLerpCheck)
	{
		m_pModelCom->Set_IsAnimLefp(bLerpCheck);
		switch (m_iMyCount)
		{
		case 0:
			m_pModelCom->Set_AnimBlendSpeed(2,6.0);
			break;
		case 1:
			m_pModelCom->Set_AnimBlendSpeed(3, 6.0);
			break;
		case 2:
			m_pModelCom->Set_AnimBlendSpeed(2, 6.0);
			break;
		case 3:
			m_pModelCom->Set_AnimBlendSpeed(2, 6.0);
			break;
		}
	}
	
	//if (true == m_pModelCom->Get_IsAnimLerp())
	//	m_pTransformCom->Go_Straight(TimeDelta*0.2f);

	m_pModelCom->Lerp_Update(TimeDelta);
}

void CMoveNpc::MoveWalkDoor(_double TimeDelta)
{
	_bool    bLerpCheck = false;

	if (true == m_bLerpCheck)
	{
		bLerpCheck = true;
		m_bLerpCheck = false;
		switch (m_iMyCount)
		{
		case 0:
			m_pModelCom->Set_Animation(2);
			break;
		case 1:
			m_pModelCom->Set_Animation(3);
			break;
		case 2:
			m_pModelCom->Set_Animation(2);
			break;
		case 3:
			m_pModelCom->Set_Animation(2);
			break;
		}

	}
	if (true == bLerpCheck)
	{
		m_pModelCom->Set_IsAnimLefp(bLerpCheck);
	}
	switch (m_iMyCount)
	{
	case 0:
		m_pModelCom->Lerp_Update(TimeDelta*0.8f);
		break;
	case 1:
		m_pModelCom->Lerp_Update(TimeDelta);
		break;
	case 2:
		m_pModelCom->Lerp_Update(TimeDelta*0.8f);
		break;
	case 3:
		m_pModelCom->Lerp_Update(TimeDelta*0.8f);
		break;
	}
	Time += 0.016f;
	if (1.00f >= Time)
	{
		switch (m_iMyCount)
		{
		case 0:
			XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(85.f, 0.f, 0.f, 0.f), XMVectorSet(270.f, 0.f, 0.f, 0.f), Time));
			break;
		case 1:
			XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(276.f, 0.f, 0.f, 0.f), XMVectorSet(450.f, 0.f, 0.f, 0.f), Time));
			break;
		case 2:
			XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(90.f, 0.f, 0.f, 0.f), XMVectorSet(270.f, 0.f, 0.f, 0.f), Time));
			break;
		case 3:
			XMStoreFloat4(&m_MoveRoation, XMVectorLerp(XMVectorSet(-90.f, 0.f, 0.f, 0.f), XMVectorSet(-270.f, 0.f, 0.f, 0.f), Time));
			break;
		}
		m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 0.f, 0.f, 0.f), _float3(0.f, XMConvertToRadians(m_MoveRoation.x), 0.f));
		m_pTransformCom->Go_Straight(TimeDelta*0.2f);
	}
	else
	{
		_float3 Pos;
		XMStoreFloat3(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
		switch (m_iMyCount)
		{
		case 0:
			if (0.f <= Pos.x)
				m_pTransformCom->Go_Straight(TimeDelta*0.2f);
			else
			{
				static_cast<CNpcDoor*>(pGameInstnace->Get_GameObject(m_iCurrentScene, TEXT("Layer_NpcDoor"), 2))->Set_Update_Modle(true);
				m_bClose = true;
			}
			break;
		case 1:
			if (20.f >= Pos.x)
				m_pTransformCom->Go_Straight(TimeDelta*0.2f);
			else
			{
				static_cast<CNpcDoor*>(pGameInstnace->Get_GameObject(m_iCurrentScene, TEXT("Layer_NpcDoor"),3))->Set_Update_Modle(true);
				m_bClose = true;
			}
			break;
		case 2:
			if (0.f <= Pos.x)
				m_pTransformCom->Go_Straight(TimeDelta*0.2f);
			else
			{
				static_cast<CNpcDoor*>(pGameInstnace->Get_GameObject(m_iCurrentScene, TEXT("Layer_NpcDoor"),1))->Set_Update_Modle(true);
				m_bClose = true;
			}
			break;
		case 3:
			if (20.f >= Pos.x)
				m_pTransformCom->Go_Straight(TimeDelta*0.2f);
			else
			{
				static_cast<CNpcDoor*>(pGameInstnace->Get_GameObject(m_iCurrentScene, TEXT("Layer_NpcDoor"),0))->Set_Update_Modle(true);
				m_bClose = true;
			}
			break;
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CMoveNpc::Set_GoDoor(_bool bMove)
{
	m_bGoDoor = true;
	m_bLerpCheck = true;
}

void CMoveNpc::Set_MoveStart(_bool bStartMove)
{
	if (false == m_bStartMove)
		m_bStartMove = bStartMove;
}


HRESULT CMoveNpc::SetUp_Component()
{
	CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
	m_iMyCount = pGameInstnace->Get_LayerSize(SCENE_DEMO, TEXT("Layer_MoveNpc"));
	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 4.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	m_vectorMove.reserve(2);

	switch (m_iMyCount)
	{
	case 0:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Coach"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(6.89f, 0.1f, 23.07f, 1.f));
		m_vectorMove.emplace_back(_float3(6.81f, 0.1f, 38.97f));
		m_vectorMove.emplace_back(_float3(3.92f, 0.1f, 42.42f));
		m_pModelCom->Set_Animation(0);
	}
	break;
	case 1:
	{
		/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Lili"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(11.51f, 0.1f, 23.13f, 1.f));
		m_vectorMove.emplace_back(_float3(11.17f, 0.1f, 39.04f));
		m_vectorMove.emplace_back(_float3(13.70f, 0.1f, 42.40f));
		m_pModelCom->Set_Animation(0);
	}
	break;
	case 2:
	{/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Sasha"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(6.82f, 0.1f, 20.7f, 1.f));
		m_vectorMove.emplace_back(_float3(6.89f, 0.1f, 27.20f));
		m_vectorMove.emplace_back(_float3(3.67f, 0.1f, 27.03f));
		m_pModelCom->Set_Animation(0);
	}
	break;
	case 3:
	{/* Com_Model*/
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Milla"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(11.31f, 0.1f, 20.58f, 1.f));
		m_vectorMove.emplace_back(_float3(11.89f, 0.1f, 27.31f));
		m_vectorMove.emplace_back(_float3(14.50f, 0.1f, 27.33f));
		m_pModelCom->Set_Animation(0);
	}
	break;
	}

	return S_OK;
}

HRESULT CMoveNpc::SetUp_ConstantTable()
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMoveNpc * CMoveNpc::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMoveNpc*	pInstance = new CMoveNpc(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CMoveNpc");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMoveNpc::Clone(void * pArg, _uint iCurrentScene)
{
	CMoveNpc*	pInstance = new CMoveNpc(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CMoveNpc");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMoveNpc::Free()
{
	__super::Free();

	//if (m_pColliderCom)
	//	m_pColliderCom->Remove_Actor();

	m_vectorMove.clear();

	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
