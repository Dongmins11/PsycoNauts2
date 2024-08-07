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
class CPuke_Ball final :public CGameObject
{
public:
	typedef struct BallMotionDesc
	{
		_float3		vDir;
		_double		dSpeed;
		_float		fStartMotionY;
		_double		dMotionTime;
		_float		fBallPosY;
		_double		dMotionPower;
		_bool		bMotion;
	}BALLDESC;
	typedef struct BallWorldDesc
	{
		_float3		vDir = { 0.f,0.f,0.f };
		_float3		vPos = { 0.f, 0.f, 0.f };
		_float		fDistance = 0.f;
	}BALLWORLDDESC;
private:
	CPuke_Ball(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPuke_Ball(const CPuke_Ball& rhs);
	virtual ~CPuke_Ball() = default;

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
	void	Create_Effect();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CSphereCollider*		m_pSphereColliderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	BALLDESC				m_tBallDesc;
	_float					m_fDistance = 0.f;
	_float3					m_vTargetPos;
	_double					m_dLerpTime = 0.0;
	_double					m_dAccTime = 0.0;
	_bool					m_bStopRotate = false;
	_double					m_dSpeed = 1.0;

	_uint					m_iLayerSize = 0;
private:
	_double					m_TimeAcc = 0.0;
	_double					m_lfDeadTime = 0.0;
	_bool					m_bDeadCheck = false;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CPuke_Ball* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END