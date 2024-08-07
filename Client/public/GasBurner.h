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
class CGasBurner final : public CGameObject
{
private:
	explicit CGasBurner(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGasBurner(const CGasBurner& rhs);
	virtual ~CGasBurner() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	_bool Get_LightedOn() { return m_bLightedOn; }
	void Set_LightedOn(_bool bCheck) { m_bLightedOn = bCheck; }

private:
	CModel*			m_pModelCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;

private:
	_bool			m_bLightedOn = false;
	_uint			m_iLayerIndex = 0;
	_double			m_dRandTimeAcc = 0.0;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CGasBurner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END