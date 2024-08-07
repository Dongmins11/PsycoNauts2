#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CTexture;
class CEffect_Option;
END

class CDizziStar final :public CGameObject
{
private:
	explicit CDizziStar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDizziStar(const CDizziStar& rhs);
	virtual ~CDizziStar() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CEffect_Option*			m_pOptionCom = nullptr;
	
private:
	_bool					m_isFadeIn = false;

	_uint					m_iMaxSpriteX = 1;
	_uint					m_iMaxSpriteY = 1;
	_uint					m_iCurSpriteX = 0;
	_uint					m_iCurSpriteY = 0;
	
	_float					m_fTimeAcc = 0.0f;
	_double					m_LiveTimeAcc = 0.0;

	_float4x4				m_MyWorldMat;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_TargetBoneMatrices(const char* pBoneName);
	HRESULT SetUp_ConstantTable();

public:
	static CDizziStar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};
