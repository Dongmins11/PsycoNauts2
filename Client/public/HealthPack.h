#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
class CModel;
class CPhysXCollider;
END

BEGIN(Client)
class CHealthPack final : public CGameObject
{
private:
	CHealthPack(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHealthPack(const CHealthPack& rhs);
	virtual ~CHealthPack() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnCollisionEnter(CCollisionObject& collision) override;
	virtual void OnControllerCollisionHit(CCollisionObject& collision)override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;
private:
	void	Bone_MatrixCalculation();
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pRenderColider = nullptr;
	CPhysXCollider*			m_pColliderCom = nullptr;

private:	/* For.Bone_Root*/
	_float4x4				m_OffsetMatrix;
	_float4x4				m_PivotMatrix;
	const _float4x4*		m_pCombinedMatrix = nullptr;
private:
	_uint					m_iAnimIndex = 0;
private:
	_bool					m_bItemPickUpCheck = false;
	_double					m_lfDeadTime = 0.0;
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_BoneMatrices(const char* pBoneName);
	HRESULT SetUp_ConstantTable();

public:
	static CHealthPack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};
END
