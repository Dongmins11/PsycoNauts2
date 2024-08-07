#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
class CCharacterController;
END

BEGIN(Client)
class CPlayer_Balling final : public CGameObject
{
private:
	CPlayer_Balling(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer_Balling(const CPlayer_Balling& rhs);
	virtual ~CPlayer_Balling() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pAABBCom = nullptr;
	CCollider*				m_pOBBCom = nullptr;
	CCharacterController*	m_pCharacterController = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CPlayer_Balling* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};
END