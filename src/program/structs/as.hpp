#pragma once
#include <container/seadPtrArray.h>
#include "structs/bbBlackboard.hpp"

namespace gsys { class Model; }
namespace sead { class CriticalSection; }
namespace engine::actor { class BlackboardAccessor; } // XXX


namespace as {
    class BlackboardChangelog;
    class BlackboardIndexArray;
    class Handle;
    class ModelAccessor;

    // TODO bb array base types? varying storage: ai uses sead::Buffer with extra capacity member?
    class BBStringArray {
        public:
        u32 mCount;
        u32 _align;
        char* mValueArray;
    };
    class BBIntArray {
        public:
        u32 mCount;
        u32 mValueArray;
    };
    class BBFloatArray {
        public:
        u32 mCount;
        u32 mValueArray;
    };
    class BBBoolArray {
        public:
        u32 mCount;
        bool mValueArray;
    };
    class BBVec3fArray {
        public:
        u32 mCount;
        sead::Vector3f mValueArray;
    };

    class Blackboard: public ::bb::BlackboardBase {
        public:
        as::BBStringArray *mpStringValueArray;
        as::BBIntArray *mpIntValueArray;
        as::BBFloatArray *mpFloatValueArray;
        as::BBBoolArray *mpBoolValueArray;
        as::BBVec3fArray *mpVec3fValueArray;
        as::BlackboardChangelog *mpStringChangelog; // these track how many times the parameter has been changed and is reset when the blackboard is reset
        as::BlackboardChangelog *mpIntChangelog;
        as::BlackboardChangelog *mpFloatChangelog;
        as::BlackboardChangelog *mpBoolChangelog;
        as::BlackboardChangelog *mpVec3fChangelog;
        as::BlackboardIndexArray *mpStringIndexArray;
        as::BlackboardIndexArray *mpIntIndexArray;
        as::BlackboardIndexArray *mpFloatIndexArray;
        as::BlackboardIndexArray *mpBoolIndexArray;
        as::BlackboardIndexArray *mpVec3fIndexArray;
        as::BBStringArray *mpStringDefaultArray;
        as::BBIntArray *mpIntDefaultArray;
        as::BBFloatArray *mpFloatDefaultArray;
        as::BBBoolArray *mpBoolDefaultArray;
        as::BBVec3fArray *mpVec3fDefaultArray;
        as::Blackboard *mBlackboardArray[4];
        as::Handle *mpHandle;
        void* field23_0xd8;
        engine::actor::BlackboardAccessor *mpBlackboardAccessor; // XXX
    };

    class Controller {
        public:
        virtual ~Controller(); // vtable
        u64 idk[2];
        u64 mSlotArray_STUB[2]; // Buffer<Slot>
        ModelAccessor* mpModelAccessor;
        void* mpModelMorph_STUB; // ModelMorph* mpModelMorph;
        void* idk2;
        as::Blackboard* mpBlackboard;
        // TODO xlink? bones? lots more members
    };
    static_assert(offsetof(Controller, mpBlackboard) == 0x40);

    class ControllerSet {
        public:
        virtual ~ControllerSet(); // vtable
        sead::PtrArray<Controller> mControllerArray;
        Controller* mpCurrentController;
        sead::CriticalSection* mpMutex;
    };
    static_assert(offsetof(ControllerSet, mpCurrentController) == 0x18);

    class Extractor; // TODO

    class Handle {
        public:
        virtual ~Handle(); // vtable
        u32 mFlags;
        u32 idk;
        as::Extractor* mpExtractor;
    };

    class ModelAccessor {
        public:
        virtual ~ModelAccessor(); // vtable
        gsys::Model* mpModel;
    };

} // as

