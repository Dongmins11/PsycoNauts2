#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
class CCharacterController;
class CPhysXScene_Manager;
class CPhysXCollider;
END


BEGIN(Client)
class CSkill_BrainStrom final : public CGameObject
{
private:
	CSkill_BrainStrom(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSkill_BrainStrom(const CSkill_BrainStrom& rhs);
	virtual ~CSkill_BrainStrom() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
public:
	void	Set_DeadCheck(_bool bDeadCheck) { m_bDeadCheck = bDeadCheck; }

private:
	HRESULT	Frist_SetPosition();
	void	Scale_Up(_double TimeDelta);
	void	Bone_MatrixCalculation(_double TimeDelta);
private:
	void	BrainStrom_Collide();
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CPhysXCollider*			m_pColliderCom = nullptr;
	CCollider*				m_pOBBCom = nullptr;
	CCharacterController*	m_pCharacterController = nullptr;
	CTexture*				m_pTextureCom = nullptr;


private:	/* For.Matrix */
	_float4x4				m_OffsetMatrix;
	_float4x4				m_PivotMatrix;
	const _float4x4*		m_pCombinedMatrix = nullptr;
	const _float4x4*		m_pTargetWorldMatrix;
	const _float4x4*		m_pMyWorldMatrix;
private:	/* For.Distance */
	_float4					m_vDistance;

private:	/* For.Height */
	_float					m_fHeight = 0.f;

private:	/* For.Scale */
	_float					m_fScale = 0.f;

private:	/* For.MoveCheck */
	_bool					m_bMoveCheck = false;

private:
	_double					m_TimeAcc = 0.0;

private:
	_bool					m_bDeadCheck = false;
	_double					m_lfDeadTime = 0.0;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT	SetUp_BoneTargetMatrix(const char* pBoneName);

public:
	static CSkill_BrainStrom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END