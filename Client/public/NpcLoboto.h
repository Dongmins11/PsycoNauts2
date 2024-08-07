#pragma once
#include "Client_Defines.h"
#include "Quest.h"
#include "QuestNpc.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CSphereCollider;
END

BEGIN(Client)

class CNpcLoboto final :public CQuestNpc
{
protected:
	explicit CNpcLoboto(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNpcLoboto(const CNpcLoboto& rhs);
	virtual ~CNpcLoboto() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	const _bool IsReady() const { return m_IsReady; }

protected:
	virtual void OnTriggerEnter(CCollisionObject& collision);
	virtual void OnTriggerExit(CCollisionObject & collision);
protected:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

private:
	_bool			m_IsReady = false;
	
public:
	static CNpcLoboto* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free();
};

END