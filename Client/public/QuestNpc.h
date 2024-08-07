#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Quest.h"
#include "Camera_Fly.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CSphereCollider;
END

BEGIN(Client)

class CQuestNpc :public CGameObject
{
protected:
	explicit CQuestNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CQuestNpc(const CQuestNpc& rhs);

	virtual ~CQuestNpc() = default;

protected:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	virtual void OnTriggerEnter(CCollisionObject& collision);
	virtual void OnTriggerExit(CCollisionObject& collision);
	virtual void Quest_RenderScripting() {};
protected:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CSphereCollider*		m_pColliderCom = nullptr;

protected:
	_bool					m_isFinishQuest = false;
	_bool					m_isScripting = false;
	_bool					m_isPlayerEnter = false;

	_uint					m_iCurrentQuestNumber = 0;
	_uint					m_iLinkQuestNumber = 0;
	_uint					m_iCurrent_QuestScriptNumber = 0;
	CQuest*					m_pCurrentQuest = nullptr;

protected:
	HRESULT SetUp_Component(_uint iSceneIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg = nullptr);
	HRESULT SetUp_ConstantTable();

public:
	static CQuestNpc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free();
};

END