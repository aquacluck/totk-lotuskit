#pragma once
#include <math/seadMatrix.h>
#include "structs/exkingEnums.hpp"
#include "structs/as.hpp"
#include "structs/aresResource.hpp"

namespace engine::actor { class ActorBase; }
namespace phive { class ControllerSet; }
namespace pp { class Document; }


namespace engine::component {

    class Component { // impls IActorComponent (but there is no actor?)
        public:
        virtual ~Component(); // vtable i guess
        sead::SafeString refPath;
    };

    class Model { // XXX ns?
        public:
        char _00[0x1f8];
        sead::Matrix34f modelMtx;
    };
    class ModelComponent: public Component {
        public:
        char _10[0x18];
        Model* model;
    };
    static_assert(offsetof(ModelComponent, model) == 0x28);

    class ASInfoComponent: public Component { public: pp::Document* mpASInfoDocument; };
    class ASOptimizeComponent: public Component { public: pp::Document* mpASOptimizeDocument; };

    class ASResourceBinder {
        public:
        void* mVtable;
        as::ControllerSet *mpASControllerSet;
        ares::resource::ResourceBinder mAresResourceBinder;
        ares::resource::ResourceBinder mAnimEventResourceBinder;
    };
    static_assert(offsetof(ASResourceBinder, mpASControllerSet) == 8);

    class ASComponent: public Component {
        public:
        engine::component::ASResourceBinder mASResourceBinder;
        as::ModelAccessor *mpModelAccessor;
        as::Handle *mpHandle;
        engine::component::ASInfoComponent *mpASInfoComponent;
        engine::component::ASOptimizeComponent *mpASOptimizeComponent;
        engine::actor::ActorBase *mpActor;
        float mSpStepRateScale;
        u8 mFlags; // | 1 is EnableUpdatePause, | 0x10 is ForceSimpleUpdate, | 0x8 is a calc flag
        u8 field9_0x9d;
        u8 field10_0x9e;
        u8 field11_0x9f;
        u8 mResAnimEventArray_STUB[0x10]; //aev::PtrArray<aev::ResData> mResAnimEventArray;
    };
    static_assert(offsetof(ASComponent, mASResourceBinder) == 0x10);
    static_assert(offsetof(ASComponent, mpActor) == 0x90);

    class PhysicsComponent: public Component {
        public:
        u8 idkman[0x10];
        phive::ControllerSet* controllerSet;
    };
    static_assert(offsetof(PhysicsComponent, controllerSet) == 0x20);

} // ns

