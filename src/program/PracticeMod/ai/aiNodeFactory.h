#pragma once
#include "../misc.h"
using namespace PracticeMod;
#include "aiNode.h"
#include <container/seadOffsetList.h>
#include <heap/seadHeap.h>
#include <type_traits>

//extern "C" {
//void* malloc(size_t size);
//}

namespace ai {

struct NodeFactoryEntry {
    using CreateFunc = ai::Node* (*)(sead::Heap*);

    u32 hash;
    CreateFunc createFunc;
    sead::ListNode node;
};

using NodeFactoryList = sead::OffsetList<NodeFactoryEntry>;
NodeFactoryList* GetNodeFactoryList();

template <typename T>
struct RegisterNode {
    static_assert(std::is_base_of<ai::Node, T>::value, "T must derive from ai::Node");

    RegisterNode(NodeFactoryEntry* entry) {
        entry->createFunc = Create;
        entry->node = {};
        ai::GetNodeFactoryList()->pushBack(entry);
        //svcOutputDebugString("lotuskit::ai::RegisterNode(zuggle)", 34);
    }

    static ai::Node* Create(sead::Heap* heap) {
        //char buf[200];
        //nn::util::SNPrintf(buf, sizeof(buf), "lotuskit::ai::NodeFactoryEntry(zuggle)::createFunc(%p)", heap);
        //svcOutputDebugString(buf, strlen(buf));

        auto ret = new (heap) T;
        //return new (heap) T;
        // FIXME crash somewhere after this 0x710074e2a4 ai ctx/node init stuff

        //nn::util::SNPrintf(buf, sizeof(buf), "createFunc(%p) -> %p", heap, ret);
        //svcOutputDebugString(buf, strlen(buf));
        return ret;
    }
};

#define REGISTER_AI_NODE(NAME) \
    static ::ai::RegisterNode<NAME> reg(#NAME);

} // namespace ai
