#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CEffect_Option final : public CComponent
{
private:
	CEffect_Option(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Option(const CEffect_Option& rhs);
	virtual ~CEffect_Option() = default;

public:
	_bool	Get_NonBillboard() { return m_isNonBillboard; }
	_bool	Get_Once() { return m_isOnce; }
	_bool	Get_WorldPosEnable() { return m_isWorldPosEnable; }
	_bool	Get_NonRotion() { return m_isNonRotation; }
	_double Get_EffectSpeed() { return m_EffectSpeed; }
	_double Get_UVSpriteTime() { return m_UVSpriteTime; }
	_float3 Get_MovePos() { return m_vMovePos; }
	_float3 Get_Scale() { return m_vScale; }
	_uint	Get_TransformPosType() { return m_iTransformPosType; }
	_float	Get_TransformPosScale() { return m_fScale; }

	_bool	Get_SetPos() { return m_isSetPos; }
	_bool	Get_SetScale() { return m_isSetScale; }

	_bool	Get_SetTransformPos() { return m_isSetTransformPos; }

	_bool	Get_LocalBillboard() { return m_isLocalBillboard; }

public:
	_matrix Get_ParentMatrix();
	_matrix Get_NormalParentMatrix();
	_matrix Get_ParentBillboardMatrix(_vector vCamPos);
	_matrix	Get_WorldPosMatrix();
	
	_matrix Get_ParentBoneMatrix();

public:
	class CModel* Get_ParentModel() { return m_pParentModel; }
	class CTransform* Get_ParentTransform() { return m_pParentTransform; }

public:
	const _float4x4* Get_CombinedMatrix() const { return m_pCombinedMatrix; }

public:
	void Set_EffectSpeed(_double EffectSpeed) { m_EffectSpeed = EffectSpeed; }
	void Set_UVSpriteTime(_double UVSpriteTime) { m_UVSpriteTime = UVSpriteTime; }
	void Set_WorldPosEnable(_bool isWorldPosEnable) { m_isWorldPosEnable = isWorldPosEnable; }
	void Set_MovePos(_vector vPos) { XMStoreFloat3(&m_vMovePos, vPos); }
	void Set_Scale(_vector vScale) { XMStoreFloat3(&m_vScale, vScale); }
	void Set_NonRotation(_bool isNonRotation) { m_isNonRotation = isNonRotation; }

	void Set_ParentTransform(_bool isOnce ,_bool isNonBillboard, class CTransform* pTransfrom);
	void Set_ParentModel(class CModel* pModel);

	void Set_TransformPosType(_uint iType) { m_iTransformPosType = iType; }
	void Set_TransformPosScale(_float fScale) { m_fScale = fScale; }

	void Set_SetPos() { m_isSetPos = true; }
	void Set_SetScale() { m_isSetScale = true; }

	void Set_SetTransformPos() { m_isSetTransformPos = true; }

	void Set_LocalBillboard() { m_isLocalBillboard = true; }

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	void Setting_BoneMatrix(const char* pBoneName);

private:
	_double m_EffectSpeed = 1.0;
	_double m_UVSpriteTime = 0.1;
	_bool	m_isNonBillboard = false;
	_bool	m_isWorldPosEnable = false;
	_bool	m_isOnce = false;
	_bool	m_isNonRotation = false;

	_bool	m_isSetPos = false;
	_bool	m_isSetScale = false;

	_bool	m_isSetTransformPos = false;

	_bool	m_isLocalBillboard = false;

	_uint	m_iTransformPosType = false;
	_float	m_fScale = 1.f;

	_float3 m_vMovePos = _float3(0.f, 0.f, 0.f);
	_float3 m_vScale = _float3(1.f, 1.f, 1.f);

	class CTransform*	m_pParentTransform = nullptr;

private:
	_float4x4			m_OffsetMatrix;
	_float4x4			m_PivotMatrix;
	const _float4x4*	m_pCombinedMatrix = nullptr;
	const _float4x4*	m_pTargetWorldMatrix;

	class CModel*		m_pParentModel = nullptr;

public:
	static CEffect_Option* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};
END
