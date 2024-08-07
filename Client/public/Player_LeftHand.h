#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CModel;
class CSphereCollider;
END

BEGIN(Client)
class CPlayer_LeftHand final : public CGameObject
{
private:
	CPlayer_LeftHand(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer_LeftHand(const CPlayer_LeftHand& rhs);
	virtual ~CPlayer_LeftHand() =default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:		/*For.Set*/
	void	Set_ChangeAnimCheck(_bool bChangeCheck);
	void	Set_HandState(_uint iHandState);

public:		/*For.Get*/
	_bool	Get_RenderCheck() const;
	_uint	Get_HandState() const;
	_bool	Get_DishGrabCheck() const;
	_bool	Get_DishThrowCheck() const;
	const _float4x4* Get_BoneMatrix() const;
private:
	void	Bone_MatrixCalculation();
	void	ThrowBone_MatrixCalculation();
private:
	virtual void OnTriggerEnter(CCollisionObject& collision);
	//virtual void OnTriggerExit(CCollisionObject& collision);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pSphereCom = nullptr;
	CSphereCollider*		m_pColliderCom = nullptr;

	/* ÀÌÆåÆ® °ü·Ã */
	CTexture*				m_pTextureCom = nullptr;

private:
	class CLeft_Hand_Anim*		m_Left_HandAnim = nullptr;

private:	/* For.Target Bone_DeltaTrans*/
	_float4x4				m_OffsetMatrix;
	_float4x4				m_PivotMatrix;
	const _float4x4*		m_pCombinedMatrix = nullptr;
	const _float4x4*		m_pTargetWorldMatrix;

private:	/* For. My Bone_RightHand */
	_float4x4				m_MyOffsetMatrix;
	_float4x4				m_MyPivotMatrix;
	const _float4x4*		m_pMyCombinedMatrix = nullptr;

private:
	_float4x4				m_MyWorldMatrix;

private:
	_float4x4				m_ColiderMatirx = {};
	_float4x4*				m_pBoneMatrix = nullptr;

private:
	_bool					m_bFirstBoneCheck = false;
private:
	_uint					m_iAnimIndex = 0;

private: /* For. Effect */
	_float3					m_vTargetEffectPos;

private:
	PlayerTriggerType		m_eTriggerType = PlayerTriggerType::PTTYPE_CHECKHAND;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_TargetBoneMatrices(const char* pBoneName);
	HRESULT SetUp_MyBoneMatrices(const char* pBoneName);
	HRESULT SetUp_ConstantTable();

public:
	static CPlayer_LeftHand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};
END
