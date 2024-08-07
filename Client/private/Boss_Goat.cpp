#include "stdafx.h"
#include "Boss_Goat.h"
#include "GameInstance.h"
#include "GoatFord.h"
#include "GoatHollis.h"
#include "GoatOtto.h"
#include "Spatula.h"
#include "Beater.h"
#include "Fruits.h"
#include "FruitsPortal_L.h"
#include "FruitsPortal_R.h"
#include "Boss_Dish.h"
#include "Camera_Fly.h"
#include "Board_L.h"
#include "Board_R.h"
#include "BossHpBar.h"

CBoss_Goat::CBoss_Goat(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBoss_Goat::CBoss_Goat(const CBoss_Goat & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBoss_Goat::NativeConstruct(void* _pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(_pArg, iCurrentScene)))
		return E_FAIL;

	_float4x4		OrgWorldMatrix;

	if (nullptr != _pArg)
		OrgWorldMatrix = *(_float4x4*)_pArg;

	_matrix WorldMatrix;

	WorldMatrix = XMLoadFloat4x4(&OrgWorldMatrix);
	WorldMatrix.r[3] += WorldMatrix.r[0] * -13.5f;
	XMStoreFloat4x4(&m_SpawnPointMatrix[POINT_L], WorldMatrix);

	WorldMatrix = XMLoadFloat4x4(&OrgWorldMatrix);
	WorldMatrix.r[3] += XMVectorSet(0.f, 0.f, -7.f, 0.f);
	XMStoreFloat4x4(&m_SpawnPointMatrix[POINT_C], WorldMatrix);
	m_SpawnPointMatrix[POINT_C] = OrgWorldMatrix;

	WorldMatrix = XMLoadFloat4x4(&OrgWorldMatrix);
	WorldMatrix.r[3] += WorldMatrix.r[0] * 13.5f;
	XMStoreFloat4x4(&m_SpawnPointMatrix[POINT_R], WorldMatrix);

	BOSSGOATINFO  tBossGoatInfo;
	ZeroMemory(&tBossGoatInfo, sizeof(BOSSGOATINFO));

	tBossGoatInfo.WorldMatrix = m_SpawnPointMatrix[POINT_R];
	tBossGoatInfo.pParent = this;
	m_pBosses[BOSS::BOSS_H] = CGoatHollis::Create(m_pDevice, m_pDeviceContext, &tBossGoatInfo);

	tBossGoatInfo.WorldMatrix = m_SpawnPointMatrix[POINT_C];
	m_pBosses[BOSS::BOSS_F] = CGoatFord::Create(m_pDevice, m_pDeviceContext, &tBossGoatInfo);

	tBossGoatInfo.WorldMatrix = m_SpawnPointMatrix[POINT_L];
	m_pBosses[BOSS::BOSS_O] = CGoatOtto::Create(m_pDevice, m_pDeviceContext, &tBossGoatInfo);

	CFruitsPortal_L::FRUITPORTALINFO tFruitLPortalInfo;
	ZeroMemory(&tFruitLPortalInfo, sizeof(CFruitsPortal_L::FRUITPORTALINFO));
	tFruitLPortalInfo.pParent = this; 
	tFruitLPortalInfo.Worldmatrix = m_SpawnPointMatrix[POINT_L];
	tFruitLPortalInfo.Worldmatrix._41 = 72.f;
	tFruitLPortalInfo.Worldmatrix._42 = 2.f;
	tFruitLPortalInfo.Worldmatrix._43 = 48.f;

	m_pPortal[PORTAL_L] = CFruitsPortal_L::Create(m_pDevice, m_pDeviceContext, &tFruitLPortalInfo);

	CFruitsPortal_R::FRUITPORTALINFO tFruitRPortalInfo;
	ZeroMemory(&tFruitRPortalInfo, sizeof(CFruitsPortal_R::FRUITPORTALINFO));
	tFruitRPortalInfo.pParent = this;
	tFruitRPortalInfo.Worldmatrix = m_SpawnPointMatrix[POINT_R];
	tFruitRPortalInfo.Worldmatrix._41 = 12.f;
	tFruitRPortalInfo.Worldmatrix._42 = 3.f;
	tFruitRPortalInfo.Worldmatrix._43 = 48.f;

	m_pPortal[PORTAL_R] = CFruitsPortal_R::Create(m_pDevice, m_pDeviceContext, &tFruitRPortalInfo);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_BoardR"), m_iCurrentScene, TEXT("Layer_BoardR"), (CGameObject**)&m_pBoardR, nullptr);
	pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_BoardL"), m_iCurrentScene, TEXT("Layer_BoardL"), (CGameObject**)&m_pBoardL, nullptr);

	CBossHpBar::MONSTERHPBARINFO tHpBarInfo;
	ZeroMemory(&tHpBarInfo, sizeof(CBossHpBar::MONSTERHPBARINFO));

	tHpBarInfo.m_pParentMatrix = nullptr;
	tHpBarInfo.m_pParentHp = &m_iHp;

	pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_BossHpBar"), m_iCurrentScene, TEXT("Layer_HpBar"), &tHpBarInfo);

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

HRESULT CBoss_Goat::NativeConstruct_Prototype()
{
	return S_OK;
}

_int CBoss_Goat::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_IsAttPhase[ATTPHASE_1] &&
		!m_IsSpawnDish[PHASE_1])
	{
		m_IsSpawnDish[PHASE_1] = true;
		_uint BossDishArg = CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_DISH;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Dish"), SCENE_STAGE05, TEXT("Boss_Dish_One"), &BossDishArg);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_BOSSDISH_POSE);
	}
	else if (m_IsAttPhase[ATTPHASE_2] &&
			!m_IsSpawnDish[PHASE_2])
		{
			m_IsSpawnDish[PHASE_2] = true;
			_uint BossDishArg = CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_SOUP;
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Dish"), SCENE_STAGE05, TEXT("Boss_Dish_One"), &BossDishArg);
			dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_BOSSDISH_POSE);
		}
	else if (m_IsAttPhase[ATTPHASE_3] &&
		!m_IsSpawnDish[PHASE_3])
	{
		m_IsSpawnDish[PHASE_3] = true;
		_uint BossDishArg = CBoss_Dish::BOSS_FOODTYPE::BOSS_FOODTYPE_COOKIE;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Dish"), SCENE_STAGE05, TEXT("Boss_Dish_One"), &BossDishArg);
		dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_BOSSDISH_POSE);
	}

	RELEASE_INSTANCE(CGameInstance);

	Update_Wave(TimeDelta);

	Update_Phase(TimeDelta);

	LinkSkill(TimeDelta);

	Bosses_Tick(TimeDelta);

	Portal_Tick(TimeDelta);

	return 0;
}

_int CBoss_Goat::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	Bosses_LateTick(TimeDelta);

	Portal_LateTick(TimeDelta);

	return 0;
}

HRESULT CBoss_Goat::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Bosses_Render();

	return S_OK;
}

void CBoss_Goat::Lock_Boss(_bool * _bArr)
{
	_bool* bArr = _bArr;

	for (_uint i = BOSS::BOSS_H; i < BOSS::BOSS_END; ++i) {
		if (nullptr != m_pBosses[i])
			dynamic_cast<CEnemy*>(m_pBosses[i])->Lock(_bArr[i]);
	}
}

void CBoss_Goat::Pattern_Lock()
{
	for (_uint i = BOSS::BOSS_H; i < BOSS::BOSS_END; ++i) {
		if (nullptr != m_pBosses[i])
			dynamic_cast<CEnemy*>(m_pBosses[i])->PatternLock(true);
	}
}

void CBoss_Goat::Pattern_UnLock()
{
	for (_uint i = BOSS::BOSS_H; i < BOSS::BOSS_END; ++i) {
		if (nullptr != m_pBosses[i])
			dynamic_cast<CEnemy*>(m_pBosses[i])->PatternLock(false);
	}
}

CModel * CBoss_Goat::Get_Model(BOSS eBoss)
{
	if (nullptr == dynamic_cast<CEnemy*>(m_pBosses[eBoss]))
		return nullptr;

	return dynamic_cast<CEnemy*>(m_pBosses[eBoss])->Get_Model();
}

CTransform * CBoss_Goat::Get_Transform(BOSS eBoss)
{
	if (nullptr == dynamic_cast<CEnemy*>(m_pBosses[eBoss]))
		return nullptr;

	return dynamic_cast<CEnemy*>(m_pBosses[eBoss])->Get_Transform();
}

CGameObject * CBoss_Goat::Get_Bosses(BOSS eBoss)
{
	if (nullptr == dynamic_cast<CEnemy*>(m_pBosses[eBoss]))
		return nullptr;

	return m_pBosses[eBoss];
}

void CBoss_Goat::Damaged(_int _iDamage)
{
	m_iHp -= _iDamage;
}

const _float4x4 CBoss_Goat::Get_MouthMatrix(BOSS eBoss) const
{
	_float4x4 WorldMatrix;
	switch (eBoss)
	{
	case BOSS::BOSS_H:
		WorldMatrix = ((CGoatHollis*)m_pBosses[BOSS_H])->Get_BoneMatrix();
		break;
	case BOSS::BOSS_O:
		WorldMatrix = ((CGoatOtto*)m_pBosses[BOSS_O])->Get_BoneMatrix();
		break;
	case BOSS::BOSS_F:
		WorldMatrix = ((CGoatFord*)m_pBosses[BOSS_F])->Get_BoneMatrix();
		break;
	default:
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
		break;
	}
	return WorldMatrix;
}

const _float4x4 CBoss_Goat::Get_BeaterMatrix() const
{
	return ((CBeater*)m_pBosses[BOSS_B])->Get_BoneMatrix();
}

CFruits::FRUITTYPE CBoss_Goat::Get_FruitType()
{
	return m_eFruitType;
}

void CBoss_Goat::LinkSkill(_double TimeDelta)
{
	if (!IsSpawned())
		return;

	if (m_IsEnterPhase[PHASE_1] || m_IsEnterPhase[PHASE_2] || m_IsEnterPhase[PHASE_3])
		return;

	if (m_pBosses[BOSS::BOSS_B] && m_pBosses[BOSS::BOSS_F])
	{
		m_dLinkSkillTime += TimeDelta;

		if (20.0 < m_dLinkSkillTime
			&& !m_IsPatternLock)
		{
			Pattern_Lock();
			m_IsPatternLock = true;
		}
		else if (30.0 < m_dLinkSkillTime &&
			!m_IsLinkSkillStart)
		{
			((CBeater*)m_pBosses[BOSS::BOSS_B])->Set_LinkSkill(true);
			((CGoatFord*)m_pBosses[BOSS::BOSS_F])->Set_LinkSkill(true);
			_bool	bArr[5] = { false, false , false, false, false };
			Lock_Boss(bArr);
			m_IsLinkSkillStart = true;
		}

		if (!((CBeater*)m_pBosses[BOSS::BOSS_B])->IsLinkSkill() &&
			!((CGoatFord*)m_pBosses[BOSS::BOSS_F])->IsLinkSkill() &&
			m_IsLinkSkillStart)
		{
			m_dLinkSkillTime = 0.0;
			m_IsLinkSkillStart = false;
			m_IsPatternLock = false;
			Pattern_UnLock();
		}
	}
}

void CBoss_Goat::Bosses_Tick(_double TimeDelta)
{
	if (nullptr != m_pBosses[BOSS::BOSS_H]) {
		m_pBosses[BOSS::BOSS_H]->Tick(TimeDelta);
		if (m_pBosses[BOSS::BOSS_H]->Get_IsDestroy() &&
			nullptr == m_pBosses[BOSS::BOSS_S])
		{
			Safe_Release(m_pBosses[BOSS::BOSS_H]);
			m_pBosses[BOSS::BOSS_H] = nullptr;
			BOSSGOATINFO  tBossGoatInfo;
			ZeroMemory(&tBossGoatInfo, sizeof(BOSSGOATINFO));

			m_SpawnPointMatrix[POINT_R]._42 = -1.f;
			tBossGoatInfo.WorldMatrix = m_SpawnPointMatrix[POINT_R];
			tBossGoatInfo.pParent = this;
			m_pBosses[BOSS::BOSS_S] = CSpatula::Create(m_pDevice, m_pDeviceContext, &tBossGoatInfo);

			Reset_Bosses();
			Reset_State();
		}
	}

	if (nullptr != m_pBosses[BOSS::BOSS_F]) {
		m_pBosses[BOSS::BOSS_F]->Tick(TimeDelta);
	}

	if (nullptr != m_pBosses[BOSS::BOSS_O]) {
		m_pBosses[BOSS::BOSS_O]->Tick(TimeDelta);
		if (m_pBosses[BOSS::BOSS_O]->Get_IsDestroy() &&
			nullptr == m_pBosses[BOSS::BOSS_B])
		{
			Safe_Release(m_pBosses[BOSS::BOSS_O]);
			m_pBosses[BOSS::BOSS_O] = nullptr;
			BOSSGOATINFO  tBossGoatInfo;
			ZeroMemory(&tBossGoatInfo, sizeof(BOSSGOATINFO));

			m_SpawnPointMatrix[POINT_L]._42 = -1.f;

			tBossGoatInfo.WorldMatrix = m_SpawnPointMatrix[POINT_L];
			tBossGoatInfo.pParent = this;
			m_pBosses[BOSS::BOSS_B] = CBeater::Create(m_pDevice, m_pDeviceContext, &tBossGoatInfo);

			Reset_Bosses();
			Reset_State();
		}
	}

	if (nullptr != m_pBosses[BOSS::BOSS_S]) {
		m_pBosses[BOSS::BOSS_S]->Tick(TimeDelta);
	}

	if (nullptr != m_pBosses[BOSS::BOSS_B]) {
		m_pBosses[BOSS::BOSS_B]->Tick(TimeDelta);
	}
}

void CBoss_Goat::Bosses_LateTick(_double TimeDelta)
{
	if (nullptr != m_pBosses[BOSS::BOSS_H]) {
		m_pBosses[BOSS::BOSS_H]->LateTick(TimeDelta);
	}

	if (nullptr != m_pBosses[BOSS::BOSS_F]) {
		m_pBosses[BOSS::BOSS_F]->LateTick(TimeDelta);
	}

	if (nullptr != m_pBosses[BOSS::BOSS_O]) {
		m_pBosses[BOSS::BOSS_O]->LateTick(TimeDelta);
	}

	if (nullptr != m_pBosses[BOSS::BOSS_S]) {
		m_pBosses[BOSS::BOSS_S]->LateTick(TimeDelta);
	}

	if (nullptr != m_pBosses[BOSS::BOSS_B]) {
		m_pBosses[BOSS::BOSS_B]->LateTick(TimeDelta);
	}
}

void CBoss_Goat::Bosses_Render()
{
	if (nullptr != m_pBosses[BOSS::BOSS_H]) {
		m_pBosses[BOSS::BOSS_H]->Render();
	}

	if (nullptr != m_pBosses[BOSS::BOSS_F]) {
		m_pBosses[BOSS::BOSS_F]->Render();
	}

	if (nullptr != m_pBosses[BOSS::BOSS_O]) {
		m_pBosses[BOSS::BOSS_O]->Render();
	}

	if (nullptr != m_pBosses[BOSS::BOSS_S]) {
		m_pBosses[BOSS::BOSS_S]->Render();
	}

	if (nullptr != m_pBosses[BOSS::BOSS_B]) {
		m_pBosses[BOSS::BOSS_B]->Render();
	}
}

void CBoss_Goat::Portal_Tick(_double TimeDelta)
{
	m_pPortal[PORTAL_L]->Tick(TimeDelta);
	m_pPortal[PORTAL_R]->Tick(TimeDelta);
}

void CBoss_Goat::Portal_LateTick(_double TimeDelta)
{
	m_pPortal[PORTAL_L]->LateTick(TimeDelta);
	m_pPortal[PORTAL_R]->LateTick(TimeDelta);
}

void CBoss_Goat::Update_Phase(_double TimeDelta)
{
	if (!IsSpawned())
		return;

	if (m_IsAttPhase[ATTPHASE_1] || m_IsAttPhase[ATTPHASE_2] || m_IsAttPhase[ATTPHASE_3])
		return;

	if (m_IsEnter)
		return;

	m_dPhaseTime += TimeDelta;

	if (40.0 < m_dPhaseTime &&
		!m_IsReadyPhase)
	{
		Pattern_Lock();
		m_IsReadyPhase = true;
	}

	if (50.0 < m_dPhaseTime)
	{
		if (!m_IsEndPhase[PHASE_1])
		{
			m_IsEnterPhase[PHASE_1] = true;
			Taunt_Bosses(true);
			m_IsEnter = true;
			m_pBoardL->Set_Type((CBoard_L::TEX_TYPE)m_iCount);
			m_pBoardR->Set_Type((CBoard_R::TEX_TYPE)m_iCount);
			m_eFruitType = (CFruits::FRUITTYPE)m_iCount++;
		}
		else if (!m_IsEndPhase[PHASE_2])
		{
			m_IsEnterPhase[PHASE_2] = true;
			Taunt_Bosses(true);
			m_IsEnter = true;
			m_pBoardL->Set_Type((CBoard_L::TEX_TYPE)m_iCount);
			m_pBoardR->Set_Type((CBoard_R::TEX_TYPE)m_iCount);
			m_eFruitType = (CFruits::FRUITTYPE)m_iCount++;
		}
		else if (!m_IsEndPhase[PHASE_3])
		{
			m_IsEnterPhase[PHASE_3] = true;
			Taunt_Bosses(true);
			m_IsEnter = true;
			m_pBoardL->Set_Type((CBoard_L::TEX_TYPE)m_iCount);
			m_pBoardR->Set_Type((CBoard_R::TEX_TYPE)m_iCount);
			m_eFruitType = (CFruits::FRUITTYPE)m_iCount++;
		}
	}
}

void CBoss_Goat::Update_Wave(_double TimeDelta)
{
	if (m_IsAttPhase[ATTPHASE_1] || m_IsAttPhase[ATTPHASE_2] || m_IsAttPhase[ATTPHASE_3])
		return;

	if (m_IsEnter)
	{
		Generate_Fruits();
		Generate_Monsters();
	}
}

void CBoss_Goat::Taunt_Bosses(_bool _bTaunt)
{
	for (_uint i = BOSS::BOSS_H; i < BOSS::BOSS_END; ++i) {
		if (nullptr != m_pBosses[i])
			dynamic_cast<CEnemy*>(m_pBosses[i])->Taunt(_bTaunt);
	}
	Reset_AttackLoop();
	Reset_State();
}

void CBoss_Goat::Generate_Fruits()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iLayerSize = pGameInstance->Get_LayerSize(m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"));

	if (0 == iLayerSize)
	{
		_uint iRandGenerateType = rand() % 3;

		switch (iRandGenerateType)
		{
		case 0:
		{
			CFruits::FRUITSINFO tFruitsInfo;
			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_WATERMELON;
			tFruitsInfo.vPos = _float3(44.f, 0.f, 20.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_BEEHIVE;
			tFruitsInfo.vPos = _float3(28.f, 0.f, 28.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_BREAD;
			tFruitsInfo.vPos = _float3(35.f, 0.f, 36.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_EGG;
			tFruitsInfo.vPos = _float3(42.f, 0.f, 37.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_ONION;
			tFruitsInfo.vPos = _float3(42.f, 0.f, 41.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_PIG;
			tFruitsInfo.vPos = _float3(48.f, 0.f, 39.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_STRAWBERRY;
			tFruitsInfo.vPos = _float3(52.f, 0.f, 36.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);
		}
		break;
		case 1:
		{
			CFruits::FRUITSINFO tFruitsInfo;
			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_WATERMELON;
			tFruitsInfo.vPos = _float3(44.f, 0.f, 20.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_BEEHIVE;
			tFruitsInfo.vPos = _float3(35.f, 0.f, 36.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_BREAD;
			tFruitsInfo.vPos = _float3(41.f, 0.f, 28.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_EGG;
			tFruitsInfo.vPos = _float3(39.f, 0.f, 25.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_ONION;
			tFruitsInfo.vPos = _float3(42.f, 0.f, 41.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_PIG;
			tFruitsInfo.vPos = _float3(48.f, 0.f, 39.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_STRAWBERRY;
			tFruitsInfo.vPos = _float3(28.f, 0.f, 28.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);
		}
		break;
		case 2:
		{
			CFruits::FRUITSINFO tFruitsInfo;
			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_WATERMELON;
			tFruitsInfo.vPos = _float3(39.f, 0.f, 25.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_BEEHIVE;
			tFruitsInfo.vPos = _float3(28.f, 0.f, 28.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_BREAD;
			tFruitsInfo.vPos = _float3(35.f, 0.f, 36.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_EGG;
			tFruitsInfo.vPos = _float3(41.f, 0.f, 28.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_ONION;
			tFruitsInfo.vPos = _float3(34.f, 0.f, 24.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_PIG;
			tFruitsInfo.vPos = _float3(48.f, 0.f, 39.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);

			ZeroMemory(&tFruitsInfo, sizeof(CFruits::FRUITSINFO));
			tFruitsInfo.eFruitsType = CFruits::FRUITTYPE::TYPE_STRAWBERRY;
			tFruitsInfo.vPos = _float3(26.f, 0.f, 20.f);
			pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), m_iCurrentScene, TEXT("Layer_Fruit_ForFinalStage"), &tFruitsInfo);
		}
		break;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CBoss_Goat::Generate_Monsters()
{
	if (m_IsGenerate)
		return; 

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_IsGenerate = true;

	_uint iRandGenerateType = rand() % 3;

	switch (iRandGenerateType)
	{
	case 0:
	{
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
		WorldMatrix._11 = -0.984f;
		WorldMatrix._13 = 0.178f;
		WorldMatrix._31 = -0.178f;
		WorldMatrix._33 = -0.984f;
		WorldMatrix._41 = 40.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 50.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		WorldMatrix._11 = -0.920f;
		WorldMatrix._13 = -0.392f;
		WorldMatrix._31 = 0.392f;
		WorldMatrix._33 = -0.920f;
		WorldMatrix._41 = 31.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 47.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		WorldMatrix._11 = -0.647f;
		WorldMatrix._13 = -0.762f;
		WorldMatrix._31 = 0.762f;
		WorldMatrix._33 = -0.647f;
		WorldMatrix._41 = 26.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 41.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		WorldMatrix._11 = -0.781f;
		WorldMatrix._13 = 0.624f;
		WorldMatrix._31 = -0.624f;
		WorldMatrix._33 = -0.781f;
		WorldMatrix._41 = 47.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 48.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		WorldMatrix._11 = -0.707f;
		WorldMatrix._13 = 0.708f;
		WorldMatrix._31 = -0.708f;
		WorldMatrix._33 = -0.707f;
		WorldMatrix._41 = 54.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 43.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
	}
	break;
	case 1:
	{
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
		WorldMatrix._11 = -0.984f;
		WorldMatrix._13 = 0.178f;
		WorldMatrix._31 = -0.178f;
		WorldMatrix._33 = -0.984f;
		WorldMatrix._41 = 40.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 50.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		WorldMatrix._11 = -0.920f;
		WorldMatrix._13 = -0.392f;
		WorldMatrix._31 = 0.392f;
		WorldMatrix._33 = -0.920f;
		WorldMatrix._41 = 31.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 47.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		WorldMatrix._11 = -0.647f;
		WorldMatrix._13 = -0.762f;
		WorldMatrix._31 = 0.762f;
		WorldMatrix._33 = -0.647f;
		WorldMatrix._41 = 26.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 41.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		WorldMatrix._11 = -0.781f;
		WorldMatrix._13 = 0.624f;
		WorldMatrix._31 = -0.624f;
		WorldMatrix._33 = -0.781f;
		WorldMatrix._41 = 47.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 48.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		WorldMatrix._11 = -0.707f;
		WorldMatrix._13 = 0.708f;
		WorldMatrix._31 = -0.708f;
		WorldMatrix._33 = -0.707f;
		WorldMatrix._41 = 54.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 43.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
	}
	break;
	case 2:
	{
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
		WorldMatrix._11 = -0.984f;
		WorldMatrix._13 = 0.178f;
		WorldMatrix._31 = -0.178f;
		WorldMatrix._33 = -0.984f;
		WorldMatrix._41 = 40.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 50.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		WorldMatrix._11 = -0.920f;
		WorldMatrix._13 = -0.392f;
		WorldMatrix._31 = 0.392f;
		WorldMatrix._33 = -0.920f;
		WorldMatrix._41 = 31.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 47.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		WorldMatrix._11 = -0.647f;
		WorldMatrix._13 = -0.762f;
		WorldMatrix._31 = 0.762f;
		WorldMatrix._33 = -0.647f;
		WorldMatrix._41 = 26.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 41.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		WorldMatrix._11 = -0.781f;
		WorldMatrix._13 = 0.624f;
		WorldMatrix._31 = -0.624f;
		WorldMatrix._33 = -0.781f;
		WorldMatrix._41 = 47.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 48.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		WorldMatrix._11 = -0.707f;
		WorldMatrix._13 = 0.708f;
		WorldMatrix._31 = -0.708f;
		WorldMatrix._33 = -0.707f;
		WorldMatrix._41 = 54.f;
		WorldMatrix._42 = 3.f;
		WorldMatrix._43 = 43.f;
		pGameInstance->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), m_iCurrentScene, TEXT("Layer_FinalStageMonsters"), &WorldMatrix);
		break;
	}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CBoss_Goat::Reset_Phase()
{
	if (m_IsEnter)
	{
	}
}

void CBoss_Goat::Reset_Bosses()
{
	_bool bArr[5] = { false, false, false, false, false };
	m_dPhaseTime = 0.0;
	m_dLinkSkillTime = 0.0;
	m_IsClearWave[WAVE_1] = false;
	m_IsClearWave[WAVE_2] = false;
	m_IsAttPhase[ATTPHASE_1] = false;
	m_IsAttPhase[ATTPHASE_2] = false;
	m_IsAttPhase[ATTPHASE_3] = false;
	m_IsEnterPhase[PHASE_1] = false;
	m_IsEnterPhase[PHASE_2] = false;
	m_IsEnterPhase[PHASE_3] = false;
	m_IsGenerate = false;
	m_IsEnter = false;
	m_IsLinkSkillStart = false;
	m_IsReadyPhase = false;
	Pattern_UnLock();
	Lock_Boss(bArr);
	Taunt_Bosses(false);
}

void CBoss_Goat::Reset_AttackLoop()
{
	for (_uint i = BOSS::BOSS_H; i < BOSS::BOSS_END; ++i) {
		if (nullptr != m_pBosses[i])
			dynamic_cast<CEnemy*>(m_pBosses[i])->Set_AttackLoop(false);
	}
}

void CBoss_Goat::Reset_State()
{
	for (_uint i = BOSS::BOSS_H; i < BOSS::BOSS_END; ++i) {
		if (nullptr != m_pBosses[i])
			dynamic_cast<CEnemy*>(m_pBosses[i])->Reset();
	}
}

_bool CBoss_Goat::IsSpawned()
{
	for (_uint i = BOSS::BOSS_H; i < BOSS::BOSS_END; ++i) {
		if (nullptr != m_pBosses[i])
		{
			if (!dynamic_cast<CEnemy*>(m_pBosses[i])->IsSpawned())
				return false;
			else
				continue;
		}
	}
	return true;
}

void CBoss_Goat::Set_Ready(BOSS eBoss)
{
	dynamic_cast<CEnemy*>(m_pBosses[eBoss])->Set_Ready(true);
}

void CBoss_Goat::Taunt_Boss(BOSS eBoss, _bool bTaunt)
{
	Reset_State();
	dynamic_cast<CEnemy*>(m_pBosses[eBoss])->Taunt(bTaunt);
}

void CBoss_Goat::Check_ClearWave()
{
	if (!m_IsClearWave[WAVE_1])
	{
		Taunt_Bosses(false);
		Pattern_UnLock();
		m_IsEnter = false;
		m_IsClearWave[WAVE_1] = true;
		m_dPhaseTime = 0.0;
		m_IsReadyPhase = false;
		Reset_State();
		m_pBoardL->Set_Type((CBoard_L::TEX_TYPE::TYPE_END));
		m_pBoardR->Set_Type((CBoard_R::TEX_TYPE::TYPE_END));
	}
	else if (!m_IsClearWave[WAVE_2])
	{
		m_IsClearWave[WAVE_2] = true;
		Pattern_UnLock();
		Reset_State();
		m_IsReadyPhase = false;
		m_pBoardL->Set_Type((CBoard_L::TEX_TYPE::TYPE_END));
		m_pBoardR->Set_Type((CBoard_R::TEX_TYPE::TYPE_END));
		if (!m_IsEndPhase[PHASE_1])
		{
			m_IsEndPhase[PHASE_1] = true;
			m_IsAttPhase[ATTPHASE_1] = true;
			Taunt_Bosses(true);
			if (nullptr != m_pBosses[BOSS_O])
				((CEnemy*)m_pBosses[BOSS_O])->HitPhase(true);
		}
		else if (!m_IsEndPhase[PHASE_2])
		{
			m_IsEndPhase[PHASE_2] = true;
			m_IsAttPhase[ATTPHASE_2] = true;
			if (nullptr != m_pBosses[BOSS_H])
				((CEnemy*)m_pBosses[BOSS_H])->HitPhase(true);
			Taunt_Bosses(true);
		}
		else if (!m_IsEndPhase[PHASE_3])
		{
			m_IsEndPhase[PHASE_3] = true;
			m_IsAttPhase[ATTPHASE_3] = true;
			((CEnemy*)m_pBosses[BOSS_F])->HitPhase(true);
			Taunt_Bosses(true);
		}
	}
}

CBoss_Goat * CBoss_Goat::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBoss_Goat*	pInstance = new CBoss_Goat(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBoss_Goat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBoss_Goat::Clone(void * pArg, _uint iSceneIndex)
{
	CBoss_Goat*	pInstance = new CBoss_Goat(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CBoss_Goat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_Goat::Free()
{
	__super::Free();

	if(nullptr!=m_pPortal[PORTAL_L])
		Safe_Release(m_pPortal[PORTAL_L]);
	if (nullptr != m_pPortal[PORTAL_R])
		Safe_Release(m_pPortal[PORTAL_R]);

	for (_uint i = BOSS::BOSS_H; i < BOSS::BOSS_END; ++i)
		Safe_Release(m_pBosses[i]);

}
