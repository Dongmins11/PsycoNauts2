#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CTexture;
END

BEGIN(Client)
class CTumbleAttack final :public CGameObject
{
private:
	explicit CTumbleAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTumbleAttack(const CTumbleAttack& rhs);
	virtual ~CTumbleAttack() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pNoiseTexture = nullptr;
	CTexture*				m_pAlphaTexture = nullptr;

	_float					m_fTimeAcc = 0.0f;
	_bool					m_isEffectCreate = false;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CTumbleAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};
END