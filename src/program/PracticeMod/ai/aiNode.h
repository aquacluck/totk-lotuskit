#pragma once

#include <prim/seadRuntimeTypeInfo.h>

#include "aiParam.h"

namespace ai {

class Context;

enum Result {
    None, Busy, Success, Failure
};

struct NodeInitArg;
struct NodeCalcArg;

class Element {
    SEAD_RTTI_BASE(Element)

public:
    virtual ~Element();
    virtual void initialize(const NodeInitArg&);
    virtual void initializeParameters(bool*);
    virtual void enter(const NodeCalcArg&);
    virtual void update(const NodeCalcArg&);
    virtual void leave(const NodeCalcArg&);
    virtual void postUpdateChildren(const NodeCalcArg&);
    virtual bool hasOutputs();
    virtual bool isDisableEnterInRootUpdate();
    virtual void f0x60();
    virtual bool isNoCalcChildren();
    virtual bool isStateEndForModuleCaller();
    virtual Result getResult();
    virtual void updateActiveNode();
    virtual const char* getName();
    virtual const char* getCategory();
    virtual void* getLocalParameters();
    virtual void updateUpdateStatus();
    virtual bool isNoCalcChildrenImpl_();
    virtual bool isChildNotRequired();
    virtual void evaluateExpression(bool, bool);

protected:
    enum PauseType {
        Update, Query, UpdateCalc, Attachment, RootUpdate, ChildNode, Enter, Leave
    };

    void pause(PauseType type) {
        mPauseFlags |= 1 << type;
    }

    void unpause(PauseType type) {
        mPauseFlags &= ~(1 << type);
    }

    void setResult(Result result) {
        mFlags.result = result;
    }

    Context* mContext = nullptr;
    void* mParameterHandler = nullptr;
    union {
        struct {
            u32 has_entered                 : 0x1; // 0
            u32 has_updated                 : 0x1; // 1 set on first update
            u32 initialized                 : 0x1; // 2
            u32 is_failure                  : 0x1; // 3
            u32 continue_update_loop        : 0x1; // 4
            u32 is_active_calc              : 0x1; // 5 set on enter regardless of if paused or not
            u32 has_attachment              : 0x1; // 6
            u32 has_queries                 : 0x1; // 7
            u32 has_root_update             : 0x1; // 8
            u32 has_action                  : 0x1; // 9
            u32 use_staging_buffer          : 0x1; // 0xa
            u32 auto_commit_outputs         : 0x1; // 0xb
            u32 update_result               : 0x1; // 0xc
            u32 disable_set_child_node      : 0x1; // 0xd - prevents child node from being set/changed
            u32 query_only_on_enter         : 0x1; // 0xe - only runs on enter (and leave but leave is clean up)
            u32 _0f                         : 0x1; // 0xf
            u32 _10                         : 0x1; // 0x10
            u32 _11                         : 0x1; // 0x11
            u32 _12                         : 0x1; // 0x12
            u32 _13                         : 0x1; // 0x13
            u32 _14                         : 0x1; // 0x14
            // these might be defined as user flags or something
            u32 disable_update              : 0x1; // 0x15 - from SelectChildOnEnterOnly or IsNoSelectChildWhenBusy - for Element_Simultaneous, this is ResultPolicy (0 = check all children for success, 1 = check only first)
            u32 calc_timing                 : 0x1; // 0x16 - when CalculateTiming == 1 (1 appears to be select only once? pauses child on enter)
            u32 play_policy                 : 0x1; // 0x17
            u32 result_policy               : 0x1; // 0x18 - ignores update_failed flag if set
            u32 _19                         : 0x1; // 0x19
            u32 _1a                         : 0x1; // 0x1a
            u32 update_next_in_frame        : 0x1; // 0x1b - IsUpdateNextInFrame
            u32 _1c                         : 0x1; // 0x1c
            u32 _1d                         : 0x1; // 0x1d - no update if no child set? (for if there is a replacement)
            Result result                   : 0x2; // 0x1e-0x1f
        };
        u32 raw;
    } mFlags{};
    u16 mNodeIndex;
    s8 mChildIndex = -1;
    u8 mPauseFlags = 0;
};
static_assert(sizeof(Element) == 0x20);

class Node : public Element {
    SEAD_RTTI_OVERRIDE(Node, Element)

public:
    ~Node() override;
    void initialize(const NodeInitArg&) override;
    void initializeParameters(bool*) override;
    void enter(const NodeCalcArg&) override;
    void update(const NodeCalcArg&) override;
    void leave(const NodeCalcArg&) override;
    bool hasOutputs() override;
    const char* getName() override;
    const char* getCategory() override;

    // given the pointer variants have different function signatures, they may not be templated? or maybe they are and the pointer ones are handled separately
    void parseS32Property(const sead::SafeString& name, S32Property& property, bool no_debug = true, bool use_existing = false);
    void parseBoolProperty(const sead::SafeString& name, BoolProperty& property, bool no_debug = true, bool use_existing = false);
    void parseF32Property(const sead::SafeString& name, F32Property& property, bool no_debug = true, bool use_existing = false);
    void parseStrProperty(const sead::SafeString& name, StrProperty& property, bool no_debug = true, bool use_existing = false);
    void parseVec3fProperty(const sead::SafeString& name, Vec3fProperty& property, bool no_debug = true, bool use_existing = false);
    // no pointer properties

    void parseS32Input(const sead::SafeString& name, S32Input& input, bool no_debug = true, bool use_existing = false);
    void parseBoolInput(const sead::SafeString& name, BoolInput& input, bool no_debug = true, bool use_existing = false);
    void parseF32Input(const sead::SafeString& name, F32Input& input, bool no_debug = true, bool use_existing = false);
    void parseStrInput(const sead::SafeString& name, StrInput& input, bool no_debug = true, bool use_existing = false);
    void parseVec3fInput(const sead::SafeString& name, Vec3fInput& input, bool no_debug = true, bool use_existing = false);
    void parsePtrInput(const sead::SafeString& name, VoidPtrInput& input, bool no_debug = true, const sead::SafeString* classname = nullptr);

    void parseS32Output(const sead::SafeString& name, S32Output& output, bool no_debug = true);
    void parseBoolOutput(const sead::SafeString& name, BoolOutput& output, bool no_debug = true);
    void parseF32Output(const sead::SafeString& name, F32Output& output, bool no_debug = true);
    void parseStrOutput(const sead::SafeString& name, StrOutput& output, bool no_debug = true);
    void parseVec3fOutput(const sead::SafeString& name, Vec3fOutput& output, bool no_debug = true);
    void parsePtrOutput(const sead::SafeString& name, VoidPtrOutput& output, bool no_debug = true, const sead::SafeString* classname = nullptr);

protected:
    virtual void parseParameters_();
    virtual void enterImpl_(const NodeCalcArg&);
    virtual void updateImpl_(const NodeCalcArg&);
    virtual void leaveImpl_(const NodeCalcArg&);
    virtual void initializeImpl_();
    virtual void initializeParametersImpl_();
    virtual void f0xf0();
    virtual void f0xf8();
};

#define AI_NODE_NAME_CATEGORY(NAME, CATEGORY)             \
    const char* getName() override { return NAME; } \
    const char* getCategory() override { return CATEGORY; }

} // namespace ai
