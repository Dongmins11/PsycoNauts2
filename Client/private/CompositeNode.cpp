#include "stdafx.h"
#include "..\Public\CompositeNode.h"


CCompositeNode::CCompositeNode()
	:CBTNode()
{
}

void CCompositeNode::Free()
{
	m_Children.clear();
}
