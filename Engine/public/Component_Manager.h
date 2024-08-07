#pragma once

#include "Model.h"
//#include "PhysX.h"
#include "Texture.h"
#include "Collider.h"
#include "Renderer.h"
#include "Transform.h"
#include "Navigation.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Trail.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Triangle.h"
#include "CharacterController.h"
#include "VIBuffer_EffectRect.h"
#include "VIBuffer_RectInstance.h"
#include "VIBuffer_RectInstance_Clone.h"
#include "VIBuffer_PointInstance.h"
#include "BoxCollider.h"
#include "NavMeshCollider.h"
#include "MeshCollider.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "Effect_Option.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
public:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;
public:
	HRESULT Reserve_Manager(_uint iNumScenes);
	HRESULT Add_Prototype(_uint iSceneIndex, const _tchar* pPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(_uint iSceneIndex, const _tchar* pPrototypeTag, void* pArg=nullptr);
	HRESULT Clear_Component(_uint iSceneIndex);

private:
	unordered_map<const _tchar*, class CComponent*>*				m_pPrototypes = nullptr;
	typedef unordered_map<const _tchar*, class CComponent*>		PROTOTYPES;

private:
	_uint				m_iNumScenes = 0;
private:
	CComponent* Find_Prototype(_uint iSceneIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END