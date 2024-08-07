#include "..\Public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

void CPipeLine::Tick()
{
	_matrix ViewMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_StateMatrix[D3DTS_VIEW]));

	XMStoreFloat3(&m_vCamPosition, ViewMatrixInv.r[3]);
}

void CPipeLine::Free()
{
}

