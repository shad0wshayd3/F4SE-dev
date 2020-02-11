#pragma once

#include "NiObjects.h"
#include "NiNodes.h"

template <typename T>
bool VisitNiAVObject(T& functor, NiAVObject* avObject) {
	if (functor(avObject))
		return true;

	NiPointer<NiNode> node(avObject->GetAsNiNode());
	if (node) {
		node->m_children.m_emptyRunStart;
		for (UInt32 i = 0; i < node->m_children.m_emptyRunStart; i++) {
			NiPointer<NiAVObject> object(node->m_children.m_data[i]);
			if (object) {
				if (VisitNiAVObject(functor, object))
					return true;
			}
		}
	}

	return false;
}