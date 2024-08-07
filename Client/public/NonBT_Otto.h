#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CNonBT_Otto final :public CGameObject
{
protected:
	explicit CNonBT_Otto(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNonBT_Otto(const CNonBT_Otto& rhs);
	virtual ~CNonBT_Otto() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

public:
	const _bool		IsIdle()const;

public:
	void	Set_EatMotion();

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CNonBT_Otto* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END