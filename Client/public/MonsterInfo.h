#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CMonsterInfo final : public CGameObject
{
public:
	typedef struct tagMonsterInfo {
		_uint iNumTextrue;
		_bool*	IsSpawn;
	}MONSTERINFO;

public:
	enum MONSTERTYPE { MONSTER_BADIDEA=1, MONSTER_REGRET=2, MONSTER_CENSOR=3, MONSTER_CENSORB=4, MONSTER_JUDGE=5, MONSTER_GOATS=6, MONSTER_DENTURE=7, MONSTER_END};

private:
	enum TYPE { TYPE_BASE, TYPE_SHADOW, TYPE_END };
	enum SCALE { SCALE_DOWN, SCALE_UP, SCALE_END };
	enum ROTATE { ROTATE_BASE, ROTATE_SHADOW, ROTATE_END };

private:
	explicit CMonsterInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMonsterInfo(const CMonsterInfo& rhs);
	virtual ~CMonsterInfo() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CTransform*			m_pTransform = nullptr;
	CVIBuffer_Rect*		m_pModelCom = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTableShadowTexture();
	HRESULT SetUp_ConstantTableBaseTexture();

private:
	_float				m_fX[TYPE_END], m_fY[TYPE_END];
	_float				m_fLogoSizeX, m_fLogoSizeY;
	_float4x4			m_ProjMatrix;
	_float				m_fAlpha = 0.01f;
	_float				m_fAngle[ROTATE_END];
	_bool				m_IsScaling[SCALE_END];
	_bool				m_IsRotation = false;
	_uint				m_iNumTexture = 0;
	const	_bool*		m_IsCheckSpawn = nullptr;

public:
	static CMonsterInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END