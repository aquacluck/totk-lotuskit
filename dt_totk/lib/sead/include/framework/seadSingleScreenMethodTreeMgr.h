#pragma once

#include "framework/seadMethodTreeMgr.h"
#include "framework/seadMethodTree.h"

namespace sead {
class SingleScreenMethodTreeMgr : public MethodTreeMgr {
    SEAD_RTTI_OVERRIDE(SingleScreenMethodTreeMgr, MethodTreeMgr)
public:
    ~SingleScreenMethodTreeMgr() override;

    void attachMethod(s32, MethodTreeNode*) override;
    MethodTreeNode* getRootMethodTreeNode(s32) override;
    void pauseAll(bool) override;
    void pauseAppCalc(bool) override;

private:
    MethodTreeNode mMethodTreeNodeRootCalc;
    MethodTreeNode mMethodTreeNodeSysCalc;
    MethodTreeNode mMethodTreeNodeAppCalc;
    MethodTreeNode mMethodTreeNodeRootDraw;
    MethodTreeNode mMethodTreeNodeSysDraw;
    MethodTreeNode mMethodTreeNodeAppDraw;
    MethodTreeNode mMethodTreeNodeAppDrawFinal;
};

} // namespace sead