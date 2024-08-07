#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CSphereCollider;
class CVIBuffer_Trail;
END

BEGIN(Client)
class CBulb final :public CGameObject
{
public:
	typedef struct BulbMotionDesc
	{
		_float3		vDir;
		_double		dSpeed;
		_float		fStartMotionY;
		_double		dMotionTime;
		_float		fBulbPosY;
		_double		dMotionPower;
		_bool		bMotion;
	}BULBDESC;
	typedef struct BulbWorldDesc
	{
		_float3		vDir = { 0.f,0.f,0.f };
		_float3		vPos = { 0.f, 0.f, 0.f };
		_float		fDistance = 0.f;
	}BULBWORLDDESC;
private:
	CBulb(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBulb(const CBulb& rhs);
	virtual ~CBulb() = default;

public:
	virtual HRESULT NativeConstruct_Prototype()override;
	virtual HRESULT NativeConstruct(void* _pArg, _uint iSceneIndex)override;
	virtual _int Tick(_double TimeDelta)override;
	virtual _int LateTick(_double TimeDelta)override;
	virtual HRESULT Render()override;

protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnCollisionEnter(CCollisionObject& collision) override;
	virtual void OnControllerCollisionHit(CCollisionObject& collision)override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

private:
	void	ParabolicMotion(_double TimeDelta);
	void	CalculateDistance();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CSphereCollider*		m_pSphereColliderCom = nullptr;
	CGameObject*			m_pEffect = nullptr;
	CGameObject*			m_pEffectTrail = nullptr;
	BULBDESC				m_tBulbDesc;
	_float					m_fDistance = 0.f;
	_float3					m_vTargetPos;
	_double					m_dLerpTime = 0.0;
	_double					m_dAccTime = 0.0;
	_bool					m_bStopRotate = false;

	_bool					m_bTrailCreate = false;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CBulb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END