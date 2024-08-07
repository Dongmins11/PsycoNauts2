#pragma once

#include "BTNode.h"

BEGIN(Client)
class CCompositeNode abstract : public CBTNode
{

protected:
	CCompositeNode();
	virtual ~CCompositeNode() = default;

public:
	void AddChild(CBTNode* pNode) {
		m_Children.emplace_back(pNode);
	}

	const list<CBTNode*>& GetChildren() {
		return m_Children;
	}

	virtual void Free()override;

private:
	list<CBTNode*> m_Children;
};

END