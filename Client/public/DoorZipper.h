#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
class CTexture;
END

BEGIN(Client)

class CDoorZipper final :public CGameObject
{
protected:
	explicit CDoorZipper(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDoorZipper(const CDoorZipper& rhs);
	virtual ~CDoorZipper() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	void Dissolve(_double  TimeDelta);

	void Blending(_double  TimeDelta);

public:
	void Setting_Dissolve();


private:
	HRESULT Animation_Playing(_double TimeDelta);
	HRESULT	Animation_Stop();
	void	Bone_MatrixCalculation();
public:
	void	Set_Delete() { m_isDestroy = true; }
	void    Set_Blending(_bool Blending) {m_bBlending = Blending;}
	
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pCollider = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	//CBoxCollider*			m_pCollider = nullptr;
private:
	_float4x4				m_OffsetMatrix;
	_float4x4				m_PivotMatrix;
	_float4x4				m_MyWorldMatrix;
	const _float4x4*		m_pCombinedMatrix = nullptr;

private:
	_float4x4				m_ColiderMatirx;
	_float4x4*				m_pBoneMatrix = nullptr;
private:
	_bool					m_isUpdate = false;
	_int					m_NavigationIndex = 0;
	_uint					m_iPX_TYPE = 0;
	_uint					m_iSeparatePassIndex = 8;

private:

	_bool					m_bIsDissolve = false;
	_bool					m_bBackUpDissolve = false;
	_bool					m_OnesFade = true;
	_float					m_fDissolveTime = 1.f;
	_double					m_dDissolveSpeed = 1.4f;
	_uint					m_iTextureIndex = 3;
	_float					m_fDissolveStart = 0.1f;
	_float3					m_vBackUpScale;
	_float3					m_vDissolveScale;
	_float					m_fDissolveScaleSpeed = 0.5f;


private:
	_bool					m_bBlending = false;
	_float					m_BlendingTimer = 0.f;


private:
	HRESULT SetUp_Component();
	HRESULT SetUp_MyBoneMatrices(const char* pBoneName);
	HRESULT SetUp_ConstantTable();

public:
	static CDoorZipper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END