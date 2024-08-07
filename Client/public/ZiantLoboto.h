#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CZiantLoboto final :public CGameObject
{
#define APPEAR 0
#define IDLE 2
#define PUNCH 3
#define LAUGH 4
#define PUNCH_TWO 5

#define PUNCH_FRAME 51
#define PUNCH_TWO_FRAME 63
public:
	enum ANIM{ANIM_READY, ANIM_APPEAR, ANIM_PUNCH,ANIM_LAUGH,ANIM_PUNCH_TWO,ANIM_IDLE,ANIM_END};
protected:
	explicit CZiantLoboto(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CZiantLoboto(const CZiantLoboto& rhs);
	virtual ~CZiantLoboto() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:/*For.Set*/
	void Set_Active() { m_bIsUpdate = true , m_isRender =true; }
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
	_bool   Setting_CurlingIndex(_int* pCurlingIndex);

private:
	void	UpdateAnimState(_double TimeDelta);

private:
	void	Appear(_double TimeDelta);
	void	Punch();
	void	Punch_Two();
private:

	ANIM				m_eCurAnimstate = ANIM_READY;
	_uint				m_iCurAnimation = 0;
	_double				m_dAnimSpeed = 0.5;
	_bool				m_bIsUpdate = false;
	
	/*For.Ready*/
	_double				m_dTimeAcc = 0.0;
	const _double		m_dMaxReadyTime = 3.0;
	/*For.Appear*/
	_bool				m_bIsAppearEnd = false;
	_float3				m_vSourPos;
	_float3				m_vDestPos;
	_double				m_dRatio = 0.;
	_double				m_dLerpSpeed = 3.5;
	/*For.Pubch*/
	_bool				m_bIsPunch = false;
public:
	static CZiantLoboto* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END