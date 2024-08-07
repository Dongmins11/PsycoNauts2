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
class CJudge_Weapon final :public CGameObject
{
public:
	typedef struct tagWeaponInfo
	{
		const	_float4x4*		pParentMatrix;
		_uint	iWeaponType;
	}WEAPONINFO;

private:
	CJudge_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CJudge_Weapon(const CJudge_Weapon& rhs);
	virtual ~CJudge_Weapon() = default;

public:
	virtual HRESULT NativeConstruct_Prototype()override;
	virtual HRESULT NativeConstruct(void* _pArg, _uint iCurrentScene)override;
	virtual _int Tick(_double TimeDelta)override;
	virtual _int LateTick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public:
	void Set_Drop(_bool _bDrop);
	_bool	IsDrop();
	_bool	IsRotate();

public:
	void SetOn_Rotate(_bool _bRotate);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pBoxCollider = nullptr;
	_bool					m_IsDrop = false;
	_bool					m_IsReadyDrop = false;
	_bool					m_IsRotation = false;
	_float					m_fDegree = 270.f;

private:
	WEAPONINFO				m_tWeaponInfo;
	_float4x4	m_NewPivotMatrix;
	_float4x4	m_NewPivotMatrix2;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CJudge_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END