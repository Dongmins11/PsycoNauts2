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
class CCracked_Denture final :public CGameObject
{
public:
	typedef struct tagDentureInfo
	{
		_float4x4		WorldMatrix;
		_uint			MeshType;
	}DENTUREINFO;

private:
	CCracked_Denture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCracked_Denture(const CCracked_Denture& rhs);
	virtual ~CCracked_Denture() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* _pArg, _uint iSceneIndex);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pBoxCollider = nullptr;

	_double					m_AccTime = 0.0;
	DENTUREINFO				m_tDentureInfo;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CCracked_Denture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END