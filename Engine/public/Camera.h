#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		_float3			vEye, vAt, vAxisY;
		_float			fFovy, fAspect, fNear, fFar;
		CTransform::TRANSFORMDESC		TransformDesc;
	}CAMEREADESC;
protected:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iSceneIndex);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT SetUp_Component(_uint iSceneIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent ** ppOut, void * pArg = nullptr);

protected:
	class CTransform*				m_pTransform = nullptr;
	CAMEREADESC						m_CameraDesc;
	class CPipeLine*				m_pPipeLine = nullptr;

public:
	virtual CGameObject* Clone(void* pArg,_uint iSceneIndex) = 0;
	virtual void Free() override;
};

END