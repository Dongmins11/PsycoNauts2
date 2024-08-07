#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
class CCollider;

END

BEGIN(Client)

class CTrampoline final :public CGameObject
{
#define DOWN 0
#define UP 2
#define SETTLE 1

#define DOWN_SPEED 3.5
#define UP_SPEED 2.0
#define SETTLE_SPEED 2.0
public:
	enum ANIM { ANIM_DOWN, ANIM_UP, ANIM_SETTLE, ANIM_END };
protected:
	explicit CTrampoline(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTrampoline(const CTrampoline& rhs);
	virtual ~CTrampoline() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();
	HRESULT Loading_PlateNaviData(const _tchar * pNaviDataFilePath);
	virtual void OnTriggerEnter(CCollisionObject& collision)override;

public:/*For.Get*/
	_bool	Get_StartJump() { return m_bStartPlayerJump; }
	_bool	Get_IsAdjustStartPos() { return m_bAdjustPlayerStartPos; }
	_vector Get_MiddlePos();
	_vector Get_Pos();
private:
	void	UpdateAnimState(_double TimeDelta);

private:
	void	SetUp_AnimValue(_uint iAnimIndex, _double dAnimSpeed);
	void	SetUp_AnimFinish(ANIM eNextAnim, _uint iNextAnimIndex, _double dNextAnimSpeed);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pTriggerCom = nullptr;
	vector<_float3*>		m_vecNaviData;
	CNavMeshCollider*		m_pNaviCollider = nullptr;

private:
	_int					m_NavigationIndex = 0;
	_uint					m_iPX_TYPE = 0;

private:
	ANIM				m_eCurAnimstate = ANIM_END;
	_uint				m_iCurAnimation = 0;
	_double				m_dAnimSpeed = 1.0;
	_bool				m_bIsUpdate = true;
	_bool				m_bIsOneFrame = false;
	_uint				m_iOneFrame = 0;
	_bool				m_bStartPlayerJump = false;
	_bool				m_bAdjustPlayerStartPos = true;
	_bool				m_bisCreated = false;
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();


public:
	static CTrampoline* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END