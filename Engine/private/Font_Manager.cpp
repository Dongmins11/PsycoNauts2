#include "..\Public\Font_Manager.h"
#include "CustomFont.h"

IMPLEMENT_SINGLETON(CFont_Manager)

CFont_Manager::CFont_Manager()
{

}

HRESULT CFont_Manager::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr != Find_Font(pFontTag))
		return E_FAIL;

	CCustomFont*		pFont = CCustomFont::Create(pDevice, pDeviceContext, pFontFilePath);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(pFontTag, pFont);

	return S_OK;
}

HRESULT CFont_Manager::Draw_Font(const _tchar * pFontTag, const _tchar* pText, _float2 vPosition, _fvector vColor)
{
	CCustomFont*		pFont = Find_Font(pFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Draw(pText, vPosition, vColor);	
}

CCustomFont * CFont_Manager::Find_Font(const _tchar * pFontTag)
{
	auto	iter = find_if(m_Fonts.begin(), m_Fonts.end(), CTagFinder(pFontTag));

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;	
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)	
		Safe_Release(Pair.second);

	m_Fonts.clear();	
}
