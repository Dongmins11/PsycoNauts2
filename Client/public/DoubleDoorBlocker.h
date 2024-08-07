#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
//class CBoxCollider;
END

BEGIN(Client)

class CDoubleDoorBlocker final:public CGameObject
{
private:
	enum AnimSTATE{STATE_LOOKOPEN, STATE_MINUS_LOOKOPEN, 
				   STATE_CLOSE_OPEN,STATE_OPEN_CLOSE, STATE_END};/* 순차적으로 :룩방향,-룩방향,close->open,open->close */

protected:
	explicit CDoubleDoorBlocker(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDoubleDoorBlocker(const CDoubleDoorBlocker& rhs);
	virtual ~CDoubleDoorBlocker() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void Setting_Throwing(_fvector vDir);

private:
	void  StageStartAnim();
	void  PlayAnimation(_double TimeDelta);
public:
	_bool Get_AnimationStartCheck() { return m_AnimStart; }
	void  AnimationStart() { m_AnimStart = true; }

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	//CBoxCollider*			m_pCollider = nullptr;
	_bool					m_AnimStart = false;
	_bool					m_Finish = false;
	_uint					m_MYObjectCount = 0;
	AnimSTATE				m_AnimationSTATE;

private:
	_int					m_NavigationIndex = 0;
	_uint					m_iPX_TYPE = 0;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CDoubleDoorBlocker* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END