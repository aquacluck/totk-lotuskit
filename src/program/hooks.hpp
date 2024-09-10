#pragma once
#include "lib.hpp"
#include "nn/nn.h"
#include "nn/hid.h"
#include "nn/util.h"
#include "lib/util/ptr_path.hpp"
#include "lib/util/sys/mem_layout.hpp"
#include "structs.hpp"
#include "types.h"
#include "logger/Logger.hpp"
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

// agl include
#include "common/aglDrawContext.h"
#include "lyr/aglLayer.h"
#include "lyr/aglRenderInfo.h"

// agl sym
#include "sym/agl_hacks.h"
#include "sym/lyr/aglLayer.h"

// sead include
#include "gfx/seadColor.h"
#include "gfx/seadDrawContext.h"
#include "gfx/seadTextWriter.h"
#include "gfx/seadViewport.h"
#include "math/seadVector.hpp"
#include "random/seadGlobalRandom.h"

// sead sym
#include "sym/gfx/seadTextWriter.h"
#include "sym/random/seadGlobalRandom.h"
#include "sym/random/seadRandom.h"

// engine sym
#include "sym/engine/actor/ActorInstanceMgr.h"
#include "sym/engine/actor/ActorMgr.h"
#include "sym/engine/actor/BaseProcCreateAndDeleteThread.h"
#include "sym/engine/hacks.h"

// game sym
#include "sym/game/ai/execute/ExecutePlayerWhistle.h"

// havok sym
#include "sym/hk/hacks.h"


// feature flags to minimize log noise and similar clutter/risk/etc
// TODO runtime toggles for everything, try to only define/constexpr features if its gonna break the build or crash.
//      Unstable hooks dont need to be installed right away, noisy hooks can be silenced/disabled, etc. try not to let things rot in ifdef purgatory.
//      Consider omitting default nullptrs from sym:: so missing syms break that version's build, and then that's when you ifdef on version,
//      which makes version parity gaps obvious *at the source level* keeping syms shit farther out of mind when its not relevant.
constexpr bool DO_RELATION_LOG = 1;
constexpr bool DO_PHYSICS_SPAM_LOG = 0;
#define DO_XXX_ACTOR_CREATION_LOG 1

extern ModCommand_ActorWatcher g_ModCommand_ActorWatcher[4]; // XXX trash glue

const float PLAYER_PHYSICS_HEIGHT_OFFSET = 0.9; // constant offset between pos32/pos64

// exclusively for unimportant actor relation add/remove hacks, dont use, keep local
// TODO delete these and do relation tracking+mgmt panel
ActorBase* PlayerCamera = nullptr;
ActorBase* EventCamera = nullptr;
ActorBase* Parasail;
ActorBase* Dm_Npc_RevivalFairy;
ActorBase* CarryBox;
ActorBase* ThrowBox;


HOOK_DEFINE_TRAMPOLINE(LoggerConnectOnWhistleHook) {
    static const ptrdiff_t s_offset = sym::game::ai::execute::ExecutePlayerWhistle::enterImpl_;

    static void Callback(void* param) {
        Logger::main->log(NS_DEFAULT_TEXT, R"("trying frontend connect() ")");
        Logger::main->connect();
        Logger::main->logf(NS_DEFAULT_TEXT, R"("main_offset %p")", exl::util::GetMainModuleInfo().m_Total.m_Start);

        sead::GlobalRandom* grand = *exl::util::pointer_path::FollowSafe<sead::GlobalRandom*, sym::sead::GlobalRandom::sInstance>();
        Logger::main->logf(NS_DEFAULT_TEXT, R"("great %p %d")", grand, grand->getU32());

        Orig(param);
    }
};

HOOK_DEFINE_TRAMPOLINE(ActorRelationAddHook) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorMgr::registerActorRelation;

    static u32 Callback(ActorMgr &actor_mgr, ActorBase &parentBase, ActorBase &childBase) {
        IActor *parent = &(parentBase.mIActor);
        IActor *child = &(childBase.mIActor);
        u8 is_skip = 0;
        //ActorMgr* ActorMgr_instance = &actor_mgr; // XXX removed global: do sead singleton stuff if we want this

        // resolve "via next relation" actor selection
        for (u8 i=1; i < sizeof(g_ModCommand_ActorWatcher); i++) {
            auto &cmd = g_ModCommand_ActorWatcher[i];
            if (!cmd.is_pending_selection || cmd.selection_type != 2) { continue; }
            if (cmd.actor_selector_parent_relation == &parentBase) {
                cmd.actor = &childBase;
                cmd.is_pending_selection = false;
                cmd.is_publishing_selection = true;
            } else if (cmd.actor_selector_parent_relation == &childBase) {
                // not sure we really care about parent/child here? use either
                cmd.actor = &parentBase;
                cmd.is_pending_selection = false;
                cmd.is_publishing_selection = true;
            }
        }

        if(!strcmp(parent->name, "PlayerCamera")) { PlayerCamera = &parentBase; }
        if(!strcmp(parent->name, "EventCamera")) { EventCamera = &parentBase; }

        if(!strcmp(parent->name, "Player")) {
            ActorBase* prev_player = g_ModCommand_ActorWatcher[0].actor;
            if(prev_player != &parentBase) {
                Logger::main->logf(NS_DEFAULT_TEXT, R"("Reassigning Player %p -> %p")", prev_player, &parentBase);
                g_ModCommand_ActorWatcher[0].actor = &parentBase;
                g_ModCommand_ActorWatcher[0].is_pending_selection = false;
                g_ModCommand_ActorWatcher[0].is_publishing_selection = true;
            }

            // Required relations (will retry loop at boot)
            if(!strcmp(child->name, "Parasail")) { Parasail = &childBase; } // Removable (glider remains at location Player opened it)
            if(!strcmp(child->name, "Dm_Npc_RevivalFairy")) { Dm_Npc_RevivalFairy = &childBase; } // Removable (fairy remains at location Player triggered it)
            if(!strcmp(child->name, "CarryBox")) { CarryBox = &childBase; }
            if(!strcmp(child->name, "ThrowBox")) { ThrowBox = &childBase; }

            // Optional relations applied at boot
            /*
            if(DO_RELATION_SKIP) {
                // No crash but haven't seen their effects
                if(!strcmp(child->name, "Obj_OneTouch_Connection")) { is_skip = 1; } // idk what it means
                if(!strcmp(child->name, "WakeBoardRope")) { is_skip = 1; } // TODO go ride a seal? prob mild visual

                // Can't observe these in title scene, but skipping them in-game leaves model behind like overload
                if(!strcmp(child->name, "Armor_Default_Upper")) { is_skip = 1; }
                if(!strcmp(child->name, "Armor_Default_Lower")) { is_skip = 1; }
                if(!strcmp(child->name, "Armor_999_Head")) { is_skip = 1; }
                if(!strcmp(child->name, "Armor_Default_Extra_01")) { is_skip = 1; }
                if(!strcmp(child->name, "Armor_Default_Extra_00")) { is_skip = 1; }
                if(!strcmp(child->name, "Accessory_Battery")) { is_skip = 1; }
                if(!strcmp(child->name, "Accessory_SupportDevice")) { is_skip = 1; }
                if(!strcmp(child->name, "PlayerRaulHand")) { is_skip = 1; } // Rare missing arm
                if(!strcmp(child->name, "Weapon_Quiver_001")) { is_skip = 1; }
            }
            */
        }
        if (is_skip) {
            if (DO_RELATION_LOG) {
                Logger::main->logf(NS_ACTOR_PLAYER_RELATIONS, R"({"op_skipped": 1, "op": "RelationAdd", "op_pair": ["%s", "%s"], "len_pair": [%d, %d] })", parent->name, child->name, parent->count, child->count);
            }
            // XXX what is 2724636224? what exactly is skipped here -vs- still handled at callsites?
            return 2724636224; // Report success but do not attach (idk where this func is in the process, but bailing seems to do overload things)
        }
        u32 result = Orig(actor_mgr, parentBase, childBase);

        if (DO_RELATION_LOG) {
            if (result) {
                Logger::main->logf(NS_ACTOR_PLAYER_RELATIONS, R"({"op": "RelationAdd", "op_pair": ["%s", "%s"], "len_pair": [%d, %d] })", parent->name, child->name, parent->count, child->count);
            } else {
                // XXX can this really fail?
                Logger::main->logf(NS_ACTOR_PLAYER_RELATIONS, R"({"op_failed": 1, "op": "RelationAdd", "op_pair": ["%s", "%s"], "len_pair": [%d, %d] })", parent->name, child->name, parent->count, child->count);
            }
        }

        return result;

        /*
        // eg iterate over children
        if (paren->.count > 0) {
            std::string children;
            children.reserve(1024);
            for (u32 i = 0; i < paren->.count; ++i) {
                ActorBase * child = GetChild(actor_mgr, (u32)parent->array[i], *parent);
                if (child != nullptr) {
                    children.append(child->name, strlen(child->name));
                    children.append(" ");
                }
            }
        }
        */
    }
};

// I think the parent and child can sometimes be flipped in this function?
HOOK_DEFINE_TRAMPOLINE(ActorRelationRemoveHook) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorMgr::removeActorRelation;

    static u32 Callback(ActorMgr &actor_mgr, ActorBase &parentBase, ActorBase &childBase) {
        IActor *parent = &(parentBase.mIActor);
        IActor *child = &(childBase.mIActor);


        if(!strcmp(parent->name, "Player") && !strcmp(child->name, "Weapon_Sword_044")) { // Tree Branch
        //if(!strcmp(parent->name, "Player") && !strcmp(child->name, "Weapon_Lsword_020")) { // Rusty Claymore
        //if(!strcmp(parent->name, "Player") && !strcmp(child->name, "Weapon_Sword_021")) { // Rusty Broadsword
            // Forcibly remove required relations
            //Orig(actor_mgr, parentBase, *Parasail);
            //Orig(actor_mgr, parentBase, *Dm_Npc_RevivalFairy);
            //Orig(actor_mgr, parentBase, *CarryBox); // softlock next time opening inventory after creating the next bundle
            //Orig(actor_mgr, parentBase, *ThrowBox); // crash
        }


        u32 result = Orig(actor_mgr, parentBase, childBase);

        if (DO_RELATION_LOG) {
            if (result) {
                Logger::main->logf(NS_ACTOR_PLAYER_RELATIONS, R"({"op": "RelationRemove", "op_pair": ["%s", "%s"], "len_pair": [%d, %d] })", parent->name, child->name, parent->count, child->count);
            } else {
                Logger::main->logf(NS_ACTOR_PLAYER_RELATIONS, R"({"op_failed": 1, "op": "RelationRemove", "op_pair": ["%s", "%s"], "len_pair": [%d, %d] })", parent->name, child->name, parent->count, child->count);
            }
        }

        return result;
    }
};


// Thanks to MaxLastBreath for figuring this out in Ultracam -- similar approach here
HOOK_DEFINE_TRAMPOLINE(TryGetPlayerPhysicsPosPtrHook) {
    static const ptrdiff_t s_offset = sym::hkUNKNOWN::UNKNOWN_applyMotion;

    static void Callback(void* self, void* param_2, hknpMotion* hkmotion, hknpMotion* motion_pool) {
        if (DO_PHYSICS_SPAM_LOG) {
            // frontend proxy(?) can't keep up with this
            //     - TODO ez runtime work+log toggles/subscriptions instead of defines
            //     - TODO e2e blocking: optionally stop the world until the frontend ui reports render, slow+accurate mode aside from concurrency+timing+etc issues
            // motion_pool[0] is something else probably? then usually a few which rarely/never update, then Player/etc usually starts from there
            Logger::main->logf(NS_PHYSICS_SPAM, R"("param_2: %p, hkmotion: %p, motion_pool: %p")", param_2, hkmotion, motion_pool);
        }

        // This seems related to object movement while touching terrain? Is this for collision or movement or ?
        // Early return here interrupts a lot of movement, but it's inconsistent+choppy similar to freezing coords.
        // Eg air movement, standing on wings, even inside shrines has free movement. Safe ground handler?
        //return Orig(self, param_2, hkmotion, motion_pool);

        //Logger::main->log("TryGetPlayerPhysicsPosPtrHook: %p %p %p %p", self, param_2, hkmotion, motion_pool);
        Vector3d* pos64 = &(hkmotion->m_centerOfMass);

        // Player + default relations are initialized before this
        ActorBase* Player = g_ModCommand_ActorWatcher[0].actor;
        Vector3f pos32 = Player->mPosition;

        hknpMotion* prev_motion = g_ModCommand_ActorWatcher[0].tracked_motion;
        if (prev_motion != nullptr) {
            // Nothing to do, we have it
            return Orig(self, param_2, hkmotion, motion_pool);

            // TODO find the new instance if needed, but not like this
            if(abs(pos32.X - prev_motion->m_centerOfMass.X) > 0.1 && abs(pos32.Y + PLAYER_PHYSICS_HEIGHT_OFFSET - prev_motion->m_centerOfMass.Y) > 0.1 && abs(pos32.Z - prev_motion->m_centerOfMass.Z) > 0.1) {
                prev_motion = nullptr;
            }
        }

        //Logger::main->log("Player pos32: %f %f %f", pos32.X, pos32.Y, pos32.Z);
        //Logger::main->log("candid pos64: %f %f %f", pos64->X, pos64->Y, pos64->Z);
        if(abs(pos32.X - pos64->X) < 0.01 && abs(pos32.Y + PLAYER_PHYSICS_HEIGHT_OFFSET - pos64->Y) < 0.01 && abs(pos32.Z - pos64->Z) < 0.01) {
            // This is the first and only very-close physics object (if that's what this is) during bootup. It's always 0.9m off.
            // FIXME reallocated/??? sometimes... after several trapped-in-water voids at (0 0 0) I got this
            Logger::main->logf(NS_DEFAULT_TEXT, R"("Reassigning Player_physics %p -> %p")", prev_motion, hkmotion);
            g_ModCommand_ActorWatcher[0].tracked_motion = hkmotion;
        }

        Orig(self, param_2, hkmotion, motion_pool);
    }
};

HOOK_DEFINE_TRAMPOLINE(HookRNG_sead_u32) {
    static const ptrdiff_t s_offset = sym::sead::Random::getU32;

    inline static bool do_log = false;
    inline static bool do_stub = false;
    inline static u32 stub_value = 420;

    static u32 Callback(void* param) { // sead::Random*
        u32 ret;
        if (do_stub) {
            ret = stub_value;
            if (do_log) {
                Logger::main->logf(NS_DEFAULT_TEXT, R"("sead::Random::u32(%p) ~~ %d")", param, ret);
            }
        } else {
            ret = Orig(param);
            if (do_log) {
                Logger::main->logf(NS_DEFAULT_TEXT, R"("sead::Random::u32(%p) => %d")", param, ret);
            }
        }
        return ret;
    }
};


#if DO_XXX_ACTOR_CREATION_LOG
HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook1) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorInstanceMgr::createActorAndConnectSync;

    static long Callback
        (long **param_1,void *param_2,ulong param_3,char **param_4,long param_5,
        undefined8 param_6,undefined4 param_7,undefined **param_8,undefined *param_9,
        undefined8 param_10,void *param_11,u8 param_12,undefined4 param_13_00,int *param_13,
        undefined8 *param_14,undefined4 param_16,undefined4 param_17,undefined8 param_15, undefined param_19) {

        Logger::main->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create_sync1"})", *param_4); // only Obj_OneTouch_Connection, no overlap

        return Orig(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10, param_11, param_12, param_13_00, param_13, param_14, param_16, param_17, param_15, param_19);
    }
};

HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook2) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorInstanceMgr::createActorAndConnectSync__2;

    static long Callback
        (long **param_1,void *param_2,ulong param_3,char **param_4,long param_5,
        undefined8 param_6,undefined4 param_7,undefined **param_8,undefined *param_9,
        undefined8 param_10,void *param_11,u8 param_12,undefined4 param_13_00,int *param_13,
        undefined8 *param_14,undefined4 param_16,undefined4 param_17,undefined8 param_15, undefined param_19) {

        Logger::main->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create_idk2"})", *param_4); // XXX never hit?

        return Orig(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10, param_11, param_12, param_13_00, param_13, param_14, param_16, param_17, param_15, param_19);
    }
};

HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook3) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorInstanceMgr::createActorAndConnectSyncImpl;

    static long Callback
        (long **param_1,void *param_2,ulong param_3,char **param_4,long param_5,
        undefined8 param_6,undefined4 param_7,long param_8,long param_9,undefined8 param_10,
        void *param_11,u8 param_12,undefined4 param_13_00,int *param_13,long *param_14,
        undefined4 param_16,undefined4 param_17,undefined8 param_15,undefined param_19) {

        // only relation names (eg fused rocks, fused wings, but weird stuff too like quickmenu-icon-screenshotting and pause-actor instantiations). no overlap with other hooks
        Logger::main->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create_relation"})", *param_4);

        return Orig(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10, param_11, param_12, param_13_00, param_13, param_14, param_16, param_17, param_15, param_19);
    }
};

HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook4) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorInstanceMgr::requestCreateActorAsync;

    static long Callback
        (void* actorInstanceMgr, char** actorName, Vector3f* pos, void* param_4, undefined4 param_5, PreActor* preActor,
        void* actorObserver, void* param_8, u8 param_9, undefined4* param_10, PreActor** destPreActor) {

        // inject our own dest pointer if needed
        PreActor* pa = nullptr;
        if (destPreActor == nullptr) {
            destPreActor = &pa;
        }

        // async, most actors
        auto ret = Orig(actorInstanceMgr, actorName, pos, param_4, param_5, preActor, actorObserver, param_8, param_9, param_10, destPreActor);

        pa = *destPreActor;
        if (pa == nullptr) {
            // eg TerrainEditCamera returns without assigning a preactor (disabled? check ret value?)
            Logger::main->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create", "pos": [%f, %f, %f], "preactor": "%p" })",
                *actorName,
                pos->X, pos->Y, pos->Z,
                pa
            );
        } else {
            Logger::main->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create", "pos": [%f, %f, %f], "preactor": "%p", "preactor_pos": [%f, %f, %f], "actor": "%p" })",
                *actorName,
                pos->X, pos->Y, pos->Z, // rarely has placement
                pa,
                pa->mPosition.X, pa->mPosition.Y, pa->mPosition.Z, // usually has placement
                pa->mpActor // always nullptr
            );
        }

        if (!strcmp(*actorName, "Player")) {
            Logger::main->logf(NS_DEFAULT_TEXT, R"("Player preactor %p")", pa);
            g_ModCommand_ActorWatcher[0].preactor = pa;
        }

        // resolve "next spawn" actor selection
        for (u8 i=1; i < sizeof(g_ModCommand_ActorWatcher); i++) {
            auto &cmd = g_ModCommand_ActorWatcher[i];
            if (!cmd.is_pending_selection || cmd.selection_type != 1 || cmd.preactor != nullptr || pa == nullptr) { continue; }
            cmd.preactor = pa;
            // cmd.is_pending_selection = true; // still pending selection: now we check every frame for mpActor to be set
        }

        return ret;
    }
};

HOOK_DEFINE_TRAMPOLINE(TestCreateActorHook5) {
    static const ptrdiff_t s_offset = sym::engine::actor::ActorInstanceMgr::requestCreateActorSync;

    static long Callback(long **param_1,char **param_2,long param_3,long param_4,long param_5,undefined8 param_6, int *param_7) {
        // few actors at bootup: ReactionHit ReactionField Chemical EffectUI SpecialPower WakeBoardRope
        Logger::main->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "create_sync"})", *param_2);
        return Orig(param_1, param_2, param_3, param_4, param_5, param_6, param_7);
    }
};

HOOK_DEFINE_INLINE(TestCreateActorHookImplEnter) {
    static const ptrdiff_t s_offset = 0x0244a638; //121
    static void Callback(exl::hook::InlineCtx* ctx) {
        Logger::main->logf(NS_ACTOR, R"({"name": "top ", "actor_op": "calc_create", "_": "%p %p %p"})", ctx->X[0], ctx->X[1], ctx->X[2]);
    }
};
HOOK_DEFINE_INLINE(TestCreateActorHookImpl) {
    static const ptrdiff_t s_offset = sym::engine::actor::BaseProcCreateAndDeleteThread::calc_create; // 0x0244a80c 121
    static void Callback(exl::hook::InlineCtx* ctx) {
        //XXX how stable are inline register accesses like this across versions? (eg originally x0 param but moved to x19 before our hook)
        //    annotate SymbolType.INSTRUCTION with named registers if this becomes a problem.
        //void* self = ctx->X[19]; // param_1
        //lVar16 = (BaseProcCreateAndDeleter *)param_1[0x1b];

        // (param_1[0x1b] + 0x10)   ( (param_1[0x1b] + 0x10),  lVar9 )
        // BaseProcCreaterAndDeleter* thing; // idk type? but this holds a mCreateProcFunction. TODO also idk how to traverse IFunction, is it really just IFunction.invoke at +0? print %p at runtime or find the vtable or name or something idk how to follow this
        // thing->mCreateProcFunction(thing->mCreateProcFunction, BaseProcCreateRequest *)

        char* neatlog =*(char**)(ctx->X[19]+(0x21*8)); // vanilla snprintf: f"{BaseProcCreateRequest->mName}({BaseProcCreateRequest*})"
        Logger::main->logf(NS_ACTOR, R"({"name": "nope", "actor_op": "calc_create", "_": "%p %s"})", ctx->X[19], neatlog);
    }
};
HOOK_DEFINE_INLINE(TestDeleteActorHookImpl) {
    static const ptrdiff_t s_offset = sym::engine::actor::BaseProcCreateAndDeleteThread::calc_delete;

    static void Callback(exl::hook::InlineCtx* ctx) {
        //void* self = (void*)(ctx->X[0]);
        //void* idk = (void*)(ctx->X[1]);
        //char* name = (char*)(self + 0x21);
        //Logger::main->logf(NS_ACTOR, R"({"name": "%s", "actor_op": "calc_delete"})", name);
    }
};
#endif


// this function actually exists in 1.0.0 but is inlined in later versions so I just recreated it for simplicity
// 0x71006fdc84 on 1.0.0
/*
ActorBase * GetChild(ActorMgr& mgr, u32 index, ActorBase &parent) {
    u32 idx = index & 0xFFFF;
    if (mgr.count <= idx) {
        return mgr.array[0].child;
    }
    if (mgr.array[idx].parent == &parent) {
        return mgr.array[idx].child;
    }

    return nullptr;
}
*/


struct GraphicsModuleCreateArg {
    char _whatever[0xb4c];
    int value0;
    char _whatever2[0x10];
    int value1;
};

// used for DebugDrawEnsureFont
HOOK_DEFINE_INLINE(StealHeap) {
    static const ptrdiff_t s_offset = sym::engine::steal_heap; // hacks
    inline static sead::Heap* stolen_heap = nullptr;
    static void Callback(exl::hook::InlineCtx* ctx) {
#ifdef TOTK_100
        // TODO register annotation/aliasing instead
        stolen_heap = reinterpret_cast<sead::Heap*>(ctx->X[19]);
#else
        stolen_heap = reinterpret_cast<sead::Heap*>(ctx->X[22]);
#endif
    }
};

// used for DebugDrawEnsureFont
HOOK_DEFINE_INLINE(GetCreateArg) {
    static const ptrdiff_t s_offset = sym::agl::create_arg; // hacks
    inline static GraphicsModuleCreateArg* create_arg = nullptr;
    static void Setup(void) {
        create_arg = new GraphicsModuleCreateArg();
    }
    static void Callback(exl::hook::InlineCtx* ctx) {
        create_arg->value0 = reinterpret_cast<GraphicsModuleCreateArg*>(ctx->X[1])->value0;
        create_arg->value1 = reinterpret_cast<GraphicsModuleCreateArg*>(ctx->X[1])->value1; // nvnBufferBuilderSetStorage?
    }
};

HOOK_DEFINE_INLINE(DebugDrawEnsureFont) {
    static const ptrdiff_t s_offset = sym::agl::lyr::RenderDisplay::drawLayer_::ensure_font; // hacks

    inline static nn::os::MutexType* fontMutex = nullptr;
    inline static void** sDefaultFont = nullptr;

    static void Setup(void) {
        fontMutex = new nn::os::MutexType();
        nn::os::InitializeMutex(fontMutex, true, 0);

        sDefaultFont = exl::util::pointer_path::FollowSafe<void*, sym::agl::default_font>(); // hacks
    }

    static void Callback(exl::hook::InlineCtx* ctx) {
        // ignore other layers
        auto* layer = (agl::lyr::Layer*)(ctx->X[21]);
        if (strncmp("Tool 2D", layer->mLayerName.cstr(), 0x20) != 0) {
            return;
        }

        // init font if needed
        if (*sDefaultFont == nullptr) {
            nn::os::LockMutex(fontMutex);
            if (*sDefaultFont == nullptr) {
                Logger::main->log(NS_DEFAULT_TEXT, R"("init default font ")");

                void(*InitDebugDrawers)(sead::Heap*, GraphicsModuleCreateArg&) = nullptr;
                void** tmp = (void**)(&InitDebugDrawers);
                *tmp = exl::util::pointer_path::FollowSafe<void*, sym::agl::init_debug_drawers>(); // hacks
                InitDebugDrawers(StealHeap::stolen_heap, *GetCreateArg::create_arg);

                if (*sDefaultFont == nullptr) {
                    Logger::main->log(NS_DEFAULT_TEXT, R"("init default font fail")");
                    return;
                }
            }
            nn::os::UnlockMutex(fontMutex);
        }

        // allow layer to draw
        ctx->W[8] = 0x28; // satisfy check to call agl::lyr::Layer::drawDebugInfo_
        layer->mRenderFlags |= 1 << 13; // satisfy check inside vanilla drawDebugInfo_
    }
};
