#pragma once
// thanks dt: https://github.com/dt-12345/ZuggleAI/blob/master/source/program/engine/AI.h

#include "../misc.h" // until I feel like making proper headers for these things
using namespace PracticeMod;
#include "../ai/aiNode.h"
#include "../ai/aiContext.h"
#include "../ai/aiController.h"

namespace engine::ai {

class AIControllerActor : public ::ai::ControllerBase {
public:
    ~AIControllerActor() override;

    GameActor* getActor() const {
        return mActor;
    }

private:
    GameActor* mActor; // should be ActorBase but whatever
};

class NodeBase : public ::ai::Node {
    SEAD_RTTI_OVERRIDE(NodeBase, ::ai::Node)

public:
    ~NodeBase() override = default;

protected:
    void initializeParametersImpl_() override;

    virtual void* f0x100();
    virtual void* f0x108();
};

class NodeAIBase : public NodeBase {
    SEAD_RTTI_OVERRIDE(NodeAIBase, NodeBase)

public:
    ~NodeAIBase() override = default;

    AIControllerActor* getController() const {
        return reinterpret_cast<AIControllerActor*>(mContext->getController());
    }

    GameActor* getActor() const {
        return getController()->getActor();
    }
};

class OneShotBase : public NodeAIBase {
    SEAD_RTTI_OVERRIDE(OneShotBase, NodeAIBase);

public:
    ~OneShotBase() override = default;

protected:
    void enterImpl_(const ::ai::NodeCalcArg&) override;
    void updateImpl_(const ::ai::NodeCalcArg&) override;
    void initializeParametersImpl_() override;
    
    virtual void oneshot_(const ::ai::NodeCalcArg&);
};

class QueryBase : public NodeAIBase {
    SEAD_RTTI_OVERRIDE(QueryBase, NodeAIBase);

public:
    ~QueryBase() override = default;

protected:
    void enterImpl_(const ::ai::NodeCalcArg&) override;
    void updateImpl_(const ::ai::NodeCalcArg&) override;
    void leaveImpl_(const ::ai::NodeCalcArg&) override;
    void initializeParametersImpl_() override;
    
    virtual void query_(const ::ai::NodeCalcArg&);
};

class ExecuteBase : public NodeAIBase {
    SEAD_RTTI_OVERRIDE(ExecuteBase, NodeAIBase);

public:
    ~ExecuteBase() override = default;
};

class SelectorBase : public NodeAIBase {
    SEAD_RTTI_OVERRIDE(SelectorBase, NodeAIBase);

public:
    ~SelectorBase() override = default;

    ::ai::Result getResult() override;
    void enterImpl_(const ::ai::NodeCalcArg&) override;
    void updateImpl_(const ::ai::NodeCalcArg&) override;
    void leaveImpl_(const ::ai::NodeCalcArg&) override;
    void initializeParametersImpl_() override;

    virtual void select_(const ::ai::NodeCalcArg&) {}
    virtual bool isIgnoreChildResult();
};

// TODO: non-AI nodes

} // namespace engine::ai
