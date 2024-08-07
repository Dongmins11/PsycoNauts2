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

class CNpcSasha final :public CQuestNpc
{
protected:
	explicit CNpcSasha(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNpcSasha(const CNpcSasha& rhs);
	virtual ~CNpcSasha() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	virtual void OnTriggerEnter(CCollisionObject& collision);
	virtual void OnTriggerExit(CCollisionObject & collision);
protected:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

private:
	CGameObject* pQuestOK = nullptr;

public:
	static CNpcSasha* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free();
};

END