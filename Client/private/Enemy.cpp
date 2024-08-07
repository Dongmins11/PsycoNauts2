#include "stdafx.h"
#include "..\public\Enemy.h"
#include "CharacterController.h"
#include "GameInstance.h"

CEnemy::CEnemy(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CEnemy::CEnemy(const CEnemy & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnemy::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CEnemy::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_TargetPosPtr()))
		return E_FAIL;

	return S_OK;
}

_int CEnemy::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return -1;

	return _int();
}

_int CEnemy::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	Update_TargetData();

	return _int();
}

const _bool CEnemy::IsGround() const
{
	return m_pCharacterController->IsDown();
}

const _bool CEnemy::IsSlided() const
{
	return m_pCharacterController->IsSides();
}

void CEnemy::Attacked(CEnemy::ATTACKED eAttackType)
{
	m_AttackedType = eAttackType;
	m_IsHit = true;
}

void CEnemy::Damaged(_int iDamage)
{
	m_iHp -= iDamage;
}

void CEnemy::Update_TargetData()
{
	_vector vTargetPos = XMLoadFloat4(m_pTargetPos);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vTargetDir = vTargetPos - vPos;
	vTargetDir = XMVectorSetY(vTargetDir, 0.f);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_vector  SrcDir = XMVector3Normalize(XMVectorSetY(vLook, 0.f));
	_vector  DstDir = XMVector3Normalize(XMVectorSetY(vTargetDir, 0.f));

	_float	 radians = acosf(XMVectorGetX(XMVector3Dot(SrcDir, DstDir)));
	_float	 degree = XMConvertToDegrees(radians);

	_float	 direction = XMVectorGetY(XMVector3Cross(SrcDir, DstDir));

	if (direction < 0)
		degree = 360 - degree;

	m_fDegree = degree;

	XMStoreFloat4(&m_vTargetDir, XMVector4Normalize(vTargetDir));
	m_fTargetDistance = XMVectorGetX(XMVector4Length(vTargetDir));
}

void CEnemy::Set_AttackLoop(_bool _bAttackLoop)
{
	m_IsAttackLoop = _bAttackLoop;
}

void CEnemy::Reset_Hp()
{
	m_iHp = 100;
}

void CEnemy::Reset_HitState()
{
	m_IsHit = false;
}

void CEnemy::Lock(_bool _bLock)
{
	m_IsLock = _bLock;
}

void CEnemy::Taunt(_bool _bTaunt)
{
	m_IsTaunt = _bTaunt;
}

void CEnemy::HitPhase(_bool _bHitPhase)
{
	m_IsHitPhase = _bHitPhase;
}

void CEnemy::Set_Ready(_bool _bReady)
{
	m_IsReady = _bReady;
}

void CEnemy::PatternLock(_bool _bPatternLock)
{
	m_IsPatternLock = _bPatternLock;
}

void CEnemy::Spawned(_bool _bSpawned)
{
	m_IsSpawned = _bSpawned;
}

HRESULT CEnemy::SetUp_TargetPosPtr()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	const _float4x4* TempMatrixPtr = pTransform->Get_WorldMatrixPtr();
	m_pTargetPos = (_float4*)TempMatrixPtr->m[3];
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CEnemy::Free()
{
	if (m_pCharacterController)
		m_pCharacterController->Remove_CharacterController();
	Safe_Release(m_pCharacterController);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	__super::Free();
}
