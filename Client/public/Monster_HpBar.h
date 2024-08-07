#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CMonster_HpBar final :public CGameObject
{
public:
	typedef struct tagMonsterHpBarInfo
	{
		const _float4x4*	m_pParentMatrix;
		const _int*			m_pParentHp;
	}MONSTERHPBARINFO;

private:
	enum TEX_TYPE { TYPE_BG, TYPE_DELAY, TYPE_FILL, TYPE_END};

protected:
	explicit CMonster_HpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMonster_HpBar(const CMonster_HpBar& rhs);
	virtual ~CMonster_HpBar() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

	MONSTERHPBARINFO		m_tMonsterHpBarInfo;

private:
	_uint					m_iCurrentHp = 0;
	_uint					m_iDelayHp = 0;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTableForBG();
	HRESULT SetUp_ConstantTableForDELAY();
	HRESULT SetUp_ConstantTableForFILL();

public:
	static CMonster_HpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END