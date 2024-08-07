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

class CMoveNpc final:public CGameObject
{
protected:
	explicit CMoveNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoveNpc(const CMoveNpc& rhs);
	virtual ~CMoveNpc() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	void MoveIDLE(_double TimeDelta);
	void MoveWalk(_double TimeDelta, _uint Index);
	void MoveLookCheck(_double TimeDelta); /*  뒤돌게 만들것임 */
	void MoveRoation(_double TimeDelta);
	void IDLEAnimation(_double TimeDelta);
	void MoveWalkDoor(_double TimeDelta);

public:
	void Set_GoDoor(_bool bMove);
	void Set_MoveStart(_bool bStartMove);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pColliderCom = nullptr;

private:
	_int					m_NavigationIndex = 0;
	_uint					m_iPX_TYPE = 0;
	_uint					m_iMyCount = 0;
	vector<_float3>			m_vectorMove;
	_bool					m_bMove[2] = { false,false };
	_bool					m_bEnd[3] = { false,false ,false};
	_bool					m_bGoDoor = false;
	_bool					m_bClose= false;
	_bool					m_bStartMove = false;
	_bool					m_oneStart = true;

	_float					m_RotationY = 0.f;
	_double					Time = 0.0;
	_float4					m_MoveRoation = { 0.f,0.f,0.f,0.f };
	_bool					m_bLerpCheck = true;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CMoveNpc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;

};

END