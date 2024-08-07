#include "stdafx.h"
#include "..\public\Stage02Button.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "PlayerTrigger_Body.h"
#include "VentDoor.h"
#include "Camera_Fly.h"
CStage02Button::CStage02Button(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CStage02Button::CStage02Button(const CStage02Button & rhs)
	:CGameObject(rhs)
{
}

HRESULT CStage02Button::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStage02Button::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;


	m_fX = g_iWinCX >> 1;
	m_fY = g_iWinCY >> 1;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f));

	// 버튼처리
	m_isRender = false;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-9.4f, 2.5f, 50.44f, 1.f));

	m_pTriggerCom->Update_TransformPhysX();
	m_pTriggerCom->Update_Transform();

	return S_OK;
}

_int CStage02Button::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_bool		isCorrect = false;

	_bool		isClicked = false;
	
	if (m_isOpenPasswordUI)
	{
		if (pGameInstance->Key_Down(VK_HOME))
		{
			if (!m_isRender)
			{
				m_isRender = true;
				pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Set_IsRender(false);
				dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_MouseMove(false);
				dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_MouseFix(false);

			}
			else
			{
				m_isRender = false;
				pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Set_IsRender(true);

			}
		}
	}

	if (!m_isRender)
	{
		RELEASE_INSTANCE(CGameInstance);

		return 0;
	}

	if (pGameInstance->Key_Down(VK_LBUTTON))
	{
		_int iNumber = Picking_NumberPad();

		if (iNumber != -1)
		{
			for (_uint i = 0; i < 4; ++i)
			{
				if (m_iKeyCheck[i] == -1)
				{
					switch (iNumber)
					{
					case 0:
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					{
						m_iKeyCheck[i] = iNumber;
						isClicked = true;
					}
						break;
					case 10:
					{
						if(i>0)
						m_iKeyCheck[i-1] = -1;
						isClicked = true;
					}
						break;
					case 11:
					{
						m_isCorrect = Checking_Password();
						isClicked = true;
					}
						break;
					default:
						break;
					}
				}

				if (m_iKeyCheck[3] != -1 && iNumber == 11)
				{
					m_isCorrect = Checking_Password();
					isClicked = true;
				}
				else if (m_iKeyCheck[3] != -1 && iNumber == 10)
				{
					m_iKeyCheck[3] = -1;
					isClicked = true;
				}

				if (isClicked)
					break;
			}
		}
	}

	// 비밀번호가 맞았을때
	if (m_isCorrect)
	{
		m_isRender = false;
		pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Set_IsRender(true);
		dynamic_cast<CVentDoor*>(pGameInstance->Get_GameObject(SCENE_STAGE02, TEXT("Layer_VentDoor"), 0))->Set_Dissolve(true, 1.f, 0, 0.1f, 1.f);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_DependentType_Value(_float4(0.8f, 0.4f, 5.21f, 5.005f), 0.5f, 1.f);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Fovy(30.f, 1.f);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_MouseMove(true);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_MouseFix(true);
	}

	RELEASE_INSTANCE(CGameInstance);
	
	return _int();
}

_int CStage02Button::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CStage02Button::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

void CStage02Button::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_TRIGGER)
	{
		if (((CPlayerTrigger_Body*)collision.Get_Object())->Get_PlayerTriggerType() == PlayerTriggerType::PTTYPE_CHECKTRIGGER)
		{
			if (!m_isCorrect)
				m_isOpenPasswordUI = true;
		}
	}
}

void CStage02Button::OnTriggerExit(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_TRIGGER)
	{
		if (((CPlayerTrigger_Body*)collision.Get_Object())->Get_PlayerTriggerType() == PlayerTriggerType::PTTYPE_CHECKTRIGGER)
		{
			if (!m_isCorrect)
			{
				m_isOpenPasswordUI = false;
				m_isRender = false;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0)->Set_IsRender(true);
				RELEASE_INSTANCE(CGameInstance);
				for (_uint i = 0; i < 4; ++i)
					m_iKeyCheck[i] = -1;
			}
		}
	}
}

HRESULT CStage02Button::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	/* Com_ButtonShowTexture */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Stage02ButtonShow"), TEXT("Com_ButtonShowTexture"), (CComponent**)&m_pButtonShowTextureCom)))
		return E_FAIL;

	/* Com_Texture_Skill_BG */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Stage02Button"), TEXT("Com_ButtonTexture"), (CComponent**)&m_pButtonTextureCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_RectBackground"), (CComponent**)&m_pBackground_BufferCom)))
		return E_FAIL;

	for (_uint i = 0; i < 4; ++i)
	{
		m_szLayerTags[i] = new _tchar[24];
		swprintf_s(m_szLayerTags[i], 24, TEXT("Com_RectKeyNumber%d"), i);
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), m_szLayerTags[i], (CComponent**)&m_pKey_BufferCom[i])))
			return E_FAIL;
	}

	for (_uint i = 0; i < 12; ++i)
	{
		m_szLayerTags[i+4] = new _tchar[32];
		swprintf_s(m_szLayerTags[i+4], 32, TEXT("Com_RectKeyPadNumber%d"), i);
		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), m_szLayerTags[i+4], (CComponent**)&m_pNumPadButtons_BufferCom[i])))
			return E_FAIL;
	}
	
	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isTrigger = true;
	tColliderDesc.pGameObject = this;

	tColliderDesc.isSceneQuery = true;

	CSphereCollider::DESC tSphereColliderDesc;
	tSphereColliderDesc.tColliderDesc = tColliderDesc;
	tSphereColliderDesc.vScale = {3.f,3.f,3.f};

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_Collider"), (CComponent**)&m_pTriggerCom, &tSphereColliderDesc)))
		return E_FAIL;

	m_eLayerType = LayerType::LAYERTYPE_TRIGGER;
	m_pTriggerCom->Set_ShapeLayer(m_eLayerType);

	m_pTriggerCom->Update_TransformPhysX();
	m_pTriggerCom->Update_Transform();

	return S_OK;
}

HRESULT CStage02Button::SetUp_ConstantTable()
{
	// Background
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	m_fSizeX = 500, m_fSizeY = 700;
	m_fX = 390;
	m_fY = 10;
	WorldMatrix._11 = m_fSizeX;
	WorldMatrix._22 = m_fSizeY;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fX + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -(m_fY + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);

	_float4x4	ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pBackground_BufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pBackground_BufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pBackground_BufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pButtonShowTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pBackground_BufferCom, 10);

	m_pBackground_BufferCom->Render(11);

	// 비밀번호
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	m_fSizeX = 40, m_fSizeY = 60;
	m_fX = 515;
	m_fY = 110;
	for (_uint i = 0; i < 4; ++i)
	{
		WorldMatrix._11 = m_fSizeX;
		WorldMatrix._22 = m_fSizeY;
		WorldMatrix._33 = 1.f;
		WorldMatrix._41 = (m_fX+(75*i) + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
		WorldMatrix._42 = -(m_fY + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);

		_float4x4 WorldMat;
		XMStoreFloat4x4(&WorldMat, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
		XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

		m_pBackground_BufferCom->Set_RawValue("g_WorldMatrix", &WorldMat, sizeof(_float4x4));
		m_pBackground_BufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
		m_pBackground_BufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

		if (m_iKeyCheck[i] != -1)
		{
			m_pButtonShowTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pKey_BufferCom[i], m_iKeyCheck[i]);
			m_pKey_BufferCom[i]->Render(11);
		}

	}

	// 키패드
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	m_fSizeX = 75, m_fSizeY = 75;
	m_fX = 520;
	m_fY = 230;
	for (_uint i = 0; i < 4; ++i)
	{
		for(_uint j = 0; j < 3; ++j)
		{
			WorldMatrix._11 = m_fSizeX;
			WorldMatrix._22 = m_fSizeY;
			WorldMatrix._33 = 1.f;
			WorldMatrix._41 = (m_fX + (90 * j) + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
			WorldMatrix._42 = -(m_fY+(i*85) + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);

			m_NumberPad[(i * 3) + j].left = 520 + j * 90;
			m_NumberPad[(i * 3) + j].right = (520 + j * 90)+m_fSizeX;
			m_NumberPad[(i * 3) + j].top = 230 + i * 85;
			m_NumberPad[(i * 3) + j].bottom = (230 + i * 85)+m_fSizeY;

			_float4x4 WorldMat;
			XMStoreFloat4x4(&WorldMat, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
			XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
			XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

			m_pBackground_BufferCom->Set_RawValue("g_WorldMatrix", &WorldMat, sizeof(_float4x4));
			m_pBackground_BufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
			m_pBackground_BufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

			m_pButtonTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pNumPadButtons_BufferCom[(i * 3) + j], (i*3)+j);
			
			m_pNumPadButtons_BufferCom[(i * 3) + j]->Render(11);
		}	
	}
	return S_OK;
}

_int CStage02Button::Picking_NumberPad()
{
	POINT  pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	for (int i = 0; i < 12; ++i)
	{
		if (m_NumberPad[i].left <= pt.x && m_NumberPad[i].right >= pt.x
			&&m_NumberPad[i].top <= pt.y && m_NumberPad[i].bottom >= pt.y)
		{
			return i;
		}
	}

	return -1;
}

_bool CStage02Button::Checking_Password()
{
	_uint password[4] = {0,7,2,6};
	_bool isCorrect = true;
	for (_uint i = 0; i < 4; ++i)
	{
		if (password[i] != m_iKeyCheck[i])
			isCorrect = false;
	}
	
	if (!isCorrect)
	{
		for (_uint i = 0; i < 4; ++i)
			m_iKeyCheck[i] = -1;
	}

	return isCorrect;
}

CStage02Button * CStage02Button::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CStage02Button*	pInstance = new CStage02Button(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CStage02Button");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStage02Button::Clone(void * pArg, _uint iCurrentScene)
{
	CStage02Button*	pInstance = new CStage02Button(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CStage02Button");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStage02Button::Free()
{
	__super::Free();

	if (m_pTriggerCom)
		m_pTriggerCom->Remove_Actor();

	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pButtonTextureCom);
	Safe_Release(m_pButtonShowTextureCom);
	Safe_Release(m_pBackground_BufferCom);

	for (_uint i = 0; i < 16; i++)
	{
		Safe_Delete_Array(m_szLayerTags[i]);
		if (i < 4)
		{
			Safe_Release(m_pKey_BufferCom[i]);
		}
		else
		{
			Safe_Release(m_pNumPadButtons_BufferCom[i-4]);

		}
	}

}
