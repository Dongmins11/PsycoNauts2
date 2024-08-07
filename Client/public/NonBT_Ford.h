#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CNonBT_Ford final :public CGameObject
{
protected:
	explicit CNonBT_Ford(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNonBT_Ford(const CNonBT_Ford& rhs);
	virtual ~CNonBT_Ford() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	const _bool		IsIdle()const;

public:
	void	Set_EatMotion();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CNonBT_Ford* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END