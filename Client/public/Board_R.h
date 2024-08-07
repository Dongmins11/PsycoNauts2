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

class CBoard_R final :public CGameObject
{
public:
	enum TEX_TYPE { TYPE_WATERMELON, TYPE_BEEHIVE, TYPE_BREAD, TYPE_EGG, TYPE_ONION, TYPE_PIG, TYPE_STRAWBERRY, TYPE_END };

protected:
	explicit CBoard_R(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBoard_R(const CBoard_R& rhs);
	virtual ~CBoard_R() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void	Set_Type(TEX_TYPE eType) { m_eType = eType; }

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	TEX_TYPE				m_eType = TYPE_END;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CBoard_R* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END