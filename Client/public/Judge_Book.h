#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
class CBoxCollider;
END

BEGIN(Client)
class CJudge_Book final :public CGameObject
{
public:
	typedef struct tagWeaponInfo
	{
		const	_float4x4*		pParentMatrix;
		_uint	iWeaponType;
	}WEAPONINFO;

private:
	CJudge_Book(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CJudge_Book(const CJudge_Book& rhs);
	virtual ~CJudge_Book() = default;

public:
	virtual HRESULT NativeConstruct_Prototype()override;
	virtual HRESULT NativeConstruct(void* _pArg, _uint iCurrentScene)override;
	virtual _int Tick(_double TimeDelta)override;
	virtual _int LateTick(_double TimeDelta)override;
	virtual HRESULT Render()override;
	virtual void Setting_Throwing(_fvector vDir)override;

protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnCollisionEnter(CCollisionObject& collision) override;
	virtual void OnControllerCollisionHit(CCollisionObject& collision)override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

public:
	void Set_Drop(_bool _bDrop);

public:
	void SetOn_Rotate(_bool _bRotate);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pBoxCollider = nullptr;
	_bool					m_IsDrop = false;
	_bool					m_IsRotation = false;
	_bool					m_IsThrow = false;
	_float					m_fDegree = 270.f;
	_float3					m_vPrePos;

	CGameObject*			m_pEffect = nullptr;

private:
	WEAPONINFO				m_tWeaponInfo;
	_float4x4	m_NewPivotMatrix;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CJudge_Book* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END