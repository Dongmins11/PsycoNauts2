#pragma once

#include "Base.h"

/* 이펙트를 불러오기위한 데이터 모음 */

BEGIN(Engine)

class CEffectData final : public CBase
{
private:
	CEffectData();
	virtual ~CEffectData() = default;

public:
	HRESULT Add_RectInfo(EFFECTRECTINFO* tInfo);
	HRESULT Add_MeshInfo(EFFECTMESHINFO* tInfo);
	HRESULT Add_ParticleInfo(EFFECTPARTICLEINFO* tInfo);

public:
	HRESULT Delete_Effect();
	HRESULT Delete_NullData();

public:
	list<EFFECTRECTINFO*> Get_RectInfoList() const { return m_RectInfoList; };
	list<EFFECTMESHINFO*> Get_MeshInfoList() const { return m_MeshInfoList; };
	list<EFFECTPARTICLEINFO*> Get_ParticleInfoList() const { return m_ParticleInfoList; };

private:
	list<EFFECTRECTINFO*>		m_RectInfoList;
	typedef list<EFFECTRECTINFO*> RECTINFO;

	list<EFFECTMESHINFO*>		m_MeshInfoList;
	typedef list<EFFECTMESHINFO*> MESHINFO;

	list<EFFECTPARTICLEINFO*>		m_ParticleInfoList;
	typedef list<EFFECTPARTICLEINFO*> PARTICLEINFO;

public:
	static CEffectData* Create();
	virtual void Free() override;
};

END