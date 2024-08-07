#include "stdafx.h"
#include "..\public\Scene_Ending.h"
#include "Loader.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "FadeInOut.h"

CScene_Ending::CScene_Ending(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CScene(pDevice, pDeviceContext)
{
}

HRESULT CScene_Ending::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Light_Desc()))
		return E_FAIL;

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);
	pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0)->Set_IsRender(true);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_INDEPENDENT);
	((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_MouseMove(false);
	_matrix WorldMat = XMMatrixIdentity();
	WorldMat.r[0] = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
	WorldMat.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	WorldMat.r[2] = XMVectorSet(0.f, 0.f, -1.f, 0.f);
	WorldMat.r[3] = XMVectorSet(0.f, 3.0f, 5.f, 1.f);
	_float4x4 CamWorldMat = STOREMATRIX(WorldMat);
	((CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform"), 0))->Set_WorldMatrix(CamWorldMat);
	
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_ENDING, TEXT("Prototype_GameObject_EndingObject"), SCENE_ENDING, TEXT("Layer_EndingObject"), TEXT("Prototype_Component_Model_Egg0"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_ENDING, TEXT("Prototype_GameObject_EndingObject"), SCENE_ENDING, TEXT("Layer_EndingObject"), TEXT("Prototype_Component_Model_Loboto"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_ENDING, TEXT("Prototype_GameObject_EndingObject"), SCENE_ENDING, TEXT("Layer_EndingObject"), TEXT("Prototype_Component_Model_BreadGriddled0"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}	
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_ENDING, TEXT("Prototype_GameObject_EndingObject"), SCENE_ENDING, TEXT("Layer_EndingObject"), TEXT("Prototype_Component_Model_Coach"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_ENDING, TEXT("Prototype_GameObject_EndingObject"), SCENE_ENDING, TEXT("Layer_EndingObject"), TEXT("Prototype_Component_Model_StrawberryBlended0"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_ENDING, TEXT("Prototype_GameObject_EndingObject"), SCENE_ENDING, TEXT("Layer_EndingObject"), TEXT("Prototype_Component_Model_Player"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_ENDING, TEXT("Prototype_GameObject_EndingObject"), SCENE_ENDING, TEXT("Layer_EndingObject"), TEXT("Prototype_Component_Model_CensorBurly0"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	for (_uint i = 0; i < 7; ++i)
	{
		((CTransform*)pGameInstance->Get_Components(SCENE_ENDING, TEXT("Layer_EndingObject"), TEXT("Com_Transform"), i))->Set_State(CTransform::STATE_POSITION, XMVectorSet(-10.f*i, 0.f, 0.f, 1.f));
		pGameInstance->Get_GameObject(SCENE_ENDING, TEXT("Layer_EndingObject"), i)->Set_RenderPass(i);
		_matrix mat = ((CTransform*)pGameInstance->Get_Components(SCENE_ENDING, TEXT("Layer_EndingObject"), TEXT("Com_Transform"), i))->Get_WorldMatrix();
		
		if(i==0)
			Setting_ObjectAnimation(TEXT("Layer_EndingObject"), i, 11);
		else if (i == 4)
			Setting_ObjectAnimation(TEXT("Layer_EndingObject"), i, 19);
		
		mat.r[3] = XMVectorSet(-10.f*i, 4.0f, 0.f,i);
		if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_ENDING, TEXT("Prototype_GameObject_EndingUI"), SCENE_ENDING, TEXT("Layer_EndingUI"), &mat)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}

	if (FAILED(pGameInstance->StopAll()))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	//»ç¿îµå//
	pGameInstance->PlayBGM(TEXT("EndingCredit.wav"));

	RELEASE_INSTANCE(CGameInstance);

	SetWindowText(g_hWnd, TEXT("Ending"));

	return S_OK;
}

_int CScene_Ending::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_OnesCheck)
	{
		m_FadeInTimer += TimeDelta;
		if (2.0 <= m_FadeInTimer)
		{
			((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_IN);
			m_FadeInTimer = 0.0;
			m_OnesCheck = false;
		}
	}


	if (m_isMove)
	{
		((CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform"), 0))->Go_Right(TimeDelta*0.45f);
		if (XMVectorGetX(((CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform"), 0))->Get_Position()) < -61.f)
		{
			((CFadeInOut*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(1.0, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);
			m_isMove = false;
		}
	}

	if (pGameInstance->Key_Down(VK_SPACE))
	{
		m_isMove = !m_isMove; 
	}
	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CScene_Ending::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("PsycoNauts2"));

	return S_OK;
}


HRESULT CScene_Ending::Ready_Light_Desc()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.iType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.079f, -0.997f, 0.0f, 0.f);
	LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CScene_Ending::Ready_Animation_Object()
{

	return S_OK;
}

void CScene_Ending::Setting_ObjectAnimation(const _tchar * LayerTag, _uint Objectindex, _uint AnimationIndex)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Get_GameObject(SCENE_ENDING, LayerTag, Objectindex)->Set_AnimationIndex(AnimationIndex);
	RELEASE_INSTANCE(CGameInstance);

}

CScene_Ending * CScene_Ending::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CScene_Ending*	pInstance = new CScene_Ending(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Created CScene_Ending");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Ending::Free()
{
	__super::Free();
}

