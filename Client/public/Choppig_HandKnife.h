#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
class CCollider;
class CPhysXCollider;
END

BEGIN(Client)

class CChoppig_HandKnife final :public CGameObject
{
protected:
	explicit CChoppig_HandKnife(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CChoppig_HandKnife(const CChoppig_HandKnife& rhs);
	virtual ~CChoppig_HandKnife() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void	Set_KeyFrameIndex(_uint iLength) { m_iCurKeyFrame = iLength; }
	void	Set_ButtonCheck(_bool bButtonCheck) { m_bButtonCheck = bButtonCheck; }
	const _double Get_HandAnimDuration() const;
	const _bool	Get_ButtonEnable() const { return m_bButtonEnable; }
	const _bool Get_ButtonCheck() const { return m_bButtonCheck; }
	const _uint	Get_CurKeyFrame() const { return m_iCurKeyFrame; }
	const _bool Get_KnifeAnimFinished() const;
private:
	CRenderer*				m_pRendererCom = nullptr;

	CPhysXCollider*			m_PhysXpColliderCom = nullptr;

	CTransform*				m_pTransformCom = nullptr;

	CModel*					m_pModelCom = nullptr;

private:
	_float3					m_vColPos = { 0,0,0 };
	_float4x4*				m_pColiderMatrix = nullptr;
	_bool					m_bFinishied = false;
	_bool					m_bKeyFrameUpdateCheck = false;
	_bool					m_bButtonEnable = false;
	_bool					m_bButtonCheck = false;
	_uint					m_iCurKeyFrame = 0;

private:
	_float4x4				m_WorldMatrix;
	_float4x4				m_OffsetMatrix;
	_float4x4				m_PivotMatrix;
	const _float4x4*		m_pCombinedMatrix = nullptr;
private:
	_float4x4				m_ColiderMatirx = {};
	_float4x4*				m_pBoneMatrix = nullptr;

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_MyBoneMatrices(const char* pBoneName);

private:
	HRESULT	Colider_PosUpdate();
	HRESULT Model_PosUpdate();
	HRESULT Bone_MatrixCalculation();

public:
	static CChoppig_HandKnife* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END