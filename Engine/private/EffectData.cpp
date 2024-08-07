#include "..\public\EffectData.h"



CEffectData::CEffectData()
{
}

HRESULT CEffectData::Add_RectInfo(EFFECTRECTINFO * tInfo)
{
	if (nullptr == tInfo)
		return E_FAIL;

	m_RectInfoList.push_back(tInfo);

	return S_OK;
}

HRESULT CEffectData::Add_MeshInfo(EFFECTMESHINFO * tInfo)
{
	if (nullptr == tInfo)
		return E_FAIL;

	m_MeshInfoList.push_back(tInfo);

	return S_OK;
}

HRESULT CEffectData::Add_ParticleInfo(EFFECTPARTICLEINFO * tInfo)
{
	if (nullptr == tInfo)
		return E_FAIL;

	m_ParticleInfoList.push_back(tInfo);

	return S_OK;
}

HRESULT CEffectData::Delete_Effect()
{
	for (auto& Rectiter : m_RectInfoList)
	{
		Safe_Delete(Rectiter);
	}
	m_RectInfoList.clear();

	for (auto& Meshiter : m_MeshInfoList)
	{
		Safe_Delete(Meshiter);
	}
	m_MeshInfoList.clear();

	for (auto& Particleiter : m_ParticleInfoList)
	{
		Safe_Delete(Particleiter);
	}
	m_ParticleInfoList.clear();

	return S_OK;
}

HRESULT CEffectData::Delete_NullData()
{
	auto Rectiter = m_RectInfoList.begin();

	for (; Rectiter != m_RectInfoList.end();)
	{
		if (*Rectiter == nullptr)
		{
			Rectiter = m_RectInfoList.erase(Rectiter);
		}
		else
			++Rectiter;
	}
	auto Meshiter = m_MeshInfoList.begin();

	for (; Meshiter != m_MeshInfoList.end();)
	{
		if (*Meshiter == nullptr)
		{
			Meshiter = m_MeshInfoList.erase(Meshiter);
		}
		else
			++Meshiter;
	}
	auto Particleiter = m_ParticleInfoList.begin();

	for (; Particleiter != m_ParticleInfoList.end();)
	{
		if (*Particleiter == nullptr)
		{
			Particleiter = m_ParticleInfoList.erase(Particleiter);
		}
		else
			++Particleiter;
	}

	return S_OK;
}


CEffectData * CEffectData::Create()
{
	CEffectData* pInstance = new CEffectData();

	return pInstance;
}

void CEffectData::Free()
{
	for (auto& Rectiter : m_RectInfoList)
	{
		Safe_Delete(Rectiter);
	}
	m_RectInfoList.clear();

	for (auto& Meshiter : m_MeshInfoList)
	{
		Safe_Delete(Meshiter);
	}
	m_MeshInfoList.clear();

	for (auto& Particleiter : m_ParticleInfoList)
	{
		Safe_Delete(Particleiter);
	}
	m_ParticleInfoList.clear();
}

