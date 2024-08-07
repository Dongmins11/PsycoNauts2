#pragma once

#include "Base.h"

/* 객체들의 사본들을 내가 원하는 기준에따라 나누어서 보관한다. */
/* 원형객체들을 보관한다. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
public:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent*	Get_Components(_uint iSceneIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex);
	class CComponent*	Get_Components_Effect(_uint iSceneIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex);
	
public:
	HRESULT Reserve_Manager(_uint iNumScenes);
	HRESULT Add_Prototype(_uint iSceneIndex, const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_ObjectToLayer(_uint iPrototypeSceneIndex, const _tchar* pPrototypeTag, _uint iSceneIndex, const _tchar* pLayerTag, void* pArg);
	list<class CGameObject*> Get_ObjectLayer(_uint iSceneIndex, const _tchar * pLayerTag);

	_bool Is_Prototype_Existed(_uint iSceneIndex, const _tchar* pPrototypeTag);
	// 클론해서 Layer에 추가함과 동시에 해당 Object를 받아오는 함수
	HRESULT Add_ObjectToLayer_GetImmediately(_uint iPrototypeSceneIndex, const _tchar* pPrototypeTag, _uint iSceneIndex, const _tchar* pLayerTag, CGameObject** ppOut, void* pArg);
	
	_int Tick(_double TimeDelta);
	_int LateTick(_double TimeDelta);
	HRESULT Clear_Object(_uint iSceneIndex);

public:
	_bool RectPicking(_uint iSceneIndex, const _tchar* pLayerTag, _uint iIndex, HWND hWnd);
	_uint Get_LayerSize(_uint iSceneIndex, const _tchar* pLayerTag);
	_uint Get_LayerMapSize(_uint iSceneIndex) const { return (_uint)m_pLayers[iSceneIndex].size(); }
	const _tchar* Get_LayerName(_uint iSceneIndex, _uint idx);
	HRESULT Delete_GameObject(_uint iSceneIndex, const _tchar * pLayerTag, _uint iIndex);

public: /* Effect Data*/
	HRESULT Create_EffectRectData(_uint iPrototypeEffectSceneIndex, const _tchar* pEffectTag, EFFECTRECTINFO* pRectInfo = nullptr);
	HRESULT Create_EffectMeshData(_uint iPrototypeEffectSceneIndex, const _tchar* pEffectTag, EFFECTMESHINFO* pMeshInfo = nullptr);
	HRESULT Create_EffectParticleData(_uint iPrototypeEffectSceneIndex, const _tchar* pEffectTag, EFFECTPARTICLEINFO* pParticleInfo = nullptr);
	
	list<EFFECTRECTINFO*> Get_EffectRectInfoList(_uint iSceneIndex, const _tchar * pEffectTag);
	list<EFFECTMESHINFO*> Get_EffectMeshInfoList(_uint iSceneIndex, const _tchar * pEffectTag);
	list<EFFECTPARTICLEINFO*> Get_EffectParticleInfoList(_uint iSceneIndex, const _tchar * pEffectTag);

	HRESULT Add_ObjectToEffect(_uint iPrototypeSceneIndex, const _tchar* pPrototypeTag, _uint iLayerSceneIndex, const _tchar* pLayerTag, _uint iEffectSceneIndex, const _tchar * pEffectTag);
	HRESULT Add_ObjectToParticle_GetImmediately(_uint iPrototypeSceneIndex, const _tchar* pPrototypeTag, _uint iLayerSceneIndex, const _tchar* pLayerTag, _uint iEffectSceneIndex, const _tchar * pEffectTag, CGameObject** ppOut);
	HRESULT Clear_EffectData(_uint iSceneIndex);

private:
	unordered_map<const _tchar*, class CGameObject*>*			m_pPrototypes = nullptr;
	typedef unordered_map<const _tchar*, class CGameObject*>	PROTOTYPES;
private:
	unordered_map<const _tchar*, class CLayer*>*				m_pLayers = nullptr;
	typedef unordered_map<const _tchar*, class CLayer*>			LAYERS;
private: /* Effect Data */
	unordered_map<const _tchar*, class CEffectData*>*			m_pEffectData = nullptr;
	typedef unordered_map<const _tchar*, class CEffectData*>	EFFECTDATAS;

private:
	_uint			m_iNumScenes = 0;

private:
	CGameObject* Find_Prototype(_uint iSceneIndex, const _tchar* pPrototypeTag);
	CLayer*	Find_Layer(_uint iSceneIndex, const _tchar* pLayerTag);
	CLayer*	Find_Layer(_uint iSceneIndex, _uint idx);
	const _tchar*	Find_LayerName(_uint iSceneIndex, _uint idx);

	/* Effect */
	class CEffectData*	Find_EffectData(_uint iSceneIndex, const _tchar* pEffectDataTag);
public:
	virtual void Free() override;
};

END