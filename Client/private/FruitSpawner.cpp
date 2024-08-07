#include "stdafx.h"
#include "..\public\FruitSpawner.h"
#include "GameInstance.h"
#include "Fruit.h"
CFruitSpawner::CFruitSpawner(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CFruitSpawner::CFruitSpawner(const CFruitSpawner & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFruitSpawner::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CFruitSpawner::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	
	if (nullptr != pArg)
		m_SpawnerMatrix = *(_matrix*)pArg;

	return S_OK;
}

_int CFruitSpawner::Tick(_double TimeDelta)
{
	if (nullptr == m_pFruitObject)
	{
		m_dCreateTimer += TimeDelta;

		if (5.0 <= m_dCreateTimer)
		{
			Create_Fruit(m_iFruitIndex);
		}
	}
	else
		m_dCreateTimer = 0.0;

	return 0;
}

_int CFruitSpawner::LateTick(_double TimeDelta)
{
	return _int();
}

HRESULT CFruitSpawner::Render()
{
	return S_OK;
}

void CFruitSpawner::Create_Fruit(FruitType iFruit)
{
	CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
	if (nullptr == m_pFruitObject)
	{
		switch (iFruit)
		{
		case FruitType::FRUIT_EGG:
			{
				if(FAILED(pGameInstnace->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototpye_GameObject_EGG"), SCENE_STAGE03, TEXT("Layer_EGG"), (CGameObject**)&m_pFruitObject, &m_SpawnerMatrix)))
					break;

				break;
			}
		case FruitType::FRUIT_WATERMELON:
			{
				if (FAILED(pGameInstnace->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototpye_GameObject_AWATERMELON"), SCENE_STAGE03, TEXT("Layer_AWATERMELON"), (CGameObject**)&m_pFruitObject, &m_SpawnerMatrix)))
					break;

				break;
			}
		case FruitType::FRUIT_BEEHIVE:
			{
				if (FAILED(pGameInstnace->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototpye_GameObject_BEEHIVE"), SCENE_STAGE03, TEXT("Layer_BEEHIVE"), (CGameObject**)&m_pFruitObject, &m_SpawnerMatrix)))
					break;

				break;
			}
		case FruitType::FRUIT_BREAD:
			{
				if (FAILED(pGameInstnace->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototpye_GameObject_BREAD"), SCENE_STAGE03, TEXT("Layer_BREAD"), (CGameObject**)&m_pFruitObject, &m_SpawnerMatrix)))
					break;

				break;
			}
		case FruitType::FRUIT_ONION:
			{
				if (FAILED(pGameInstnace->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototpye_GameObject_ONION"), SCENE_STAGE03, TEXT("Layer_ONION"), (CGameObject**)&m_pFruitObject, &m_SpawnerMatrix)))
					break;

				break;
			}
		case FruitType::FRUIT_PORK:
			{
				if (FAILED(pGameInstnace->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototpye_GameObject_PORK"), SCENE_STAGE03, TEXT("Layer_PORK"), (CGameObject**)&m_pFruitObject, &m_SpawnerMatrix)))
					break;

				break;
			}
		case FruitType::FRUIT_STRAWBERRY:
			{
				if (FAILED(pGameInstnace->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototpye_GameObject_STRAWBERRY"), SCENE_STAGE03, TEXT("Layer_STRAWBERRY"), (CGameObject**)&m_pFruitObject, &m_SpawnerMatrix)))
					break;
				break;
			}
		}
		if (m_pFruitObject)
		{
			((CFruit*)m_pFruitObject)->Set_FruitType(iFruit, m_SpwnerIndex);
		}
		//Safe_AddRef(m_pFruitObject);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CFruitSpawner::Stting_Create_Cook_And_Mesh_Setting()
{
	m_pFruitObject->Cooking_MeshSetting();
}

void CFruitSpawner::Cooking_EndCut_SceneAnimation()
{
	switch (m_SpwnerIndex)
	{
	//case 0:
	//	m_pFruitObject->Set_CutSceneAnimation();
	//	break;
	//case 1:
	//	m_pFruitObject->Set_CutSceneAnimation();
	//	break;
	//case 2:
	//	m_pFruitObject->Set_CutSceneAnimation();
	//	break;
	//case 3:
	//	m_pFruitObject->Set_CutSceneAnimation();
	//	break;
	case 4:
		m_pFruitObject->Set_CutSceneAnimation(19);
		break;
	case 5:
		m_pFruitObject->Set_CutSceneAnimation(14);
		break;
	case 6:
		m_pFruitObject->Set_CutSceneAnimation(13);
		break;
	case 7:
		m_pFruitObject->Set_CutSceneAnimation(12);
		break;
	case 8:
		m_pFruitObject->Set_CutSceneAnimation(11);
		break;
	case 9:
		m_pFruitObject->Set_CutSceneAnimation(9);
		break;
	case 10:
		m_pFruitObject->Set_CutSceneAnimation(8);
		break;
	case 11:
		m_pFruitObject->Set_CutSceneAnimation(5);
		break;
	case 12:
		m_pFruitObject->Set_CutSceneAnimation(4);
		break;
	case 13:
		m_pFruitObject->Set_CutSceneAnimation(3);
		break;
	case 14:
		m_pFruitObject->Set_CutSceneAnimation(2);
		break;
	case 15:
		m_pFruitObject->Set_CutSceneAnimation(1);
		break;
	}
	
}

CFruitSpawner * CFruitSpawner::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CFruitSpawner*	pInstance = new CFruitSpawner(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CFruitSpawner");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFruitSpawner::Clone(void * pArg, _uint iCurrentScene)
{
	CFruitSpawner*	pInstance = new CFruitSpawner(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CFruitSpawner");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFruitSpawner::Free()
{
	__super::Free();

	//Safe_Release(m_pFruitObject);
}
