#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
END

BEGIN(Client)
class CTooth final :public CGameObject
{
private:
	CTooth(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTooth(const CTooth& rhs);
	virtual ~CTooth() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* _pArg, _uint iSceneIndex);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void Set_Drop(_bool _bDrop);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pBoxCollider = nullptr;
	_bool					m_IsDrop = false;

private:
	const	_float4x4*		m_pParentMatrix = nullptr;

private:
	void Update_PropHandMatrix();

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CTooth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END