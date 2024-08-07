#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Transform.h"
BEGIN(Engine)
class CVIBuffer_Cube;
class CRenderer;
class CTexture;
class CTransform;
class CPipeLine;
class CCollider_OBB;
class CCollider_Sphere;
class CModel;
END
BEGIN(Client)
class CSkyBox final : public CGameObject
{
protected:
	explicit CSkyBox(ID3D11Device* _pGraphic_Device, ID3D11DeviceContext* _pGraphic_DeviceContext);
	explicit CSkyBox(const CSkyBox& _rhs);
	virtual ~CSkyBox() = default;

public:
	virtual HRESULT NativeConstruct_Prototype()override;
	virtual HRESULT NativeConstruct(void* _pArg, _uint iCurrentScene)override;
	virtual _int	Tick(_double _dTimeDelta)override;
	virtual _int	LateTick(_double _dTimeDelta)override;
	virtual HRESULT Render()override;

public:/*For.Set*/
	void Set_RGBA(_float4 _vRgba) {
		m_vRGBA = _vRgba;
	}
	void Set_Pos(_fvector vPos) {
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
	}
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_TargetWorldMatrix();

private:
	class CRenderer*				m_pRenderer = nullptr;
	class CTransform*				m_pTransform = nullptr;
	class CModel*					m_pModel = nullptr;

private:
	_float4							m_vRGBA;
	//const _float4x4*				m_pTargetWorldMatrix = nullptr;

public:
	static CSkyBox* Create(ID3D11Device* _pGraphic_Device, ID3D11DeviceContext* _pGraphic_DeviceContext);
	virtual CGameObject* Clone(void* _pArg, _uint _eLevelIndex)override;
	virtual void Free()override;
};
END
