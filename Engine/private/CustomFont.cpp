#include "..\Public\CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}


HRESULT CCustomFont::NativeConstruct(const _tchar * pFontFilePath)
{
	m_pSpriteBatch = new SpriteBatch(m_pDeviceContext);
	m_pSpriteFont = new SpriteFont(m_pDevice, pFontFilePath);

	return S_OK;
}

HRESULT CCustomFont::Draw(const _tchar* pText, _float2 vPosition, _fvector vColor)
{
	if (nullptr == m_pSpriteBatch ||
		nullptr == m_pSpriteFont)
		return E_FAIL;

	m_pSpriteBatch->Begin();

	m_pSpriteFont->DrawString(m_pSpriteBatch, pText, vPosition, vColor);

	m_pSpriteBatch->End();

	return S_OK;
}

CCustomFont * CCustomFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pFontFilePath)
{
	CCustomFont*	pInstance = new CCustomFont(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pFontFilePath)))
	{
		MSGBOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCustomFont::Free()
{
	Safe_Delete(m_pSpriteBatch);
	Safe_Delete(m_pSpriteFont);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
