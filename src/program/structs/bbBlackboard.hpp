#pragma once

#include <container/seadBuffer.h>
#include <gfx/seadColor.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <prim/seadSafeString.h>

#include <cstring>
#include <type_traits>

#include "structs/exkingEnums.hpp"

// TODO bb array base types?
namespace engine::actor { class BlackboardAccessor; } // XXX
namespace sead { class Heap; }


namespace bb {

    struct Param {
        // they might've not used a raw union but whatever
        union Value {
            sead::SafeString str;
            s32 s_int;
            f32 float_32;
            bool boolean;
            sead::Vector3f vec3f;
            void* void_ptr;
            s8 s_byte;
            s16 s_short;
            s64 s_long;
            u8 u_byte;
            u16 u_short;
            u32 u_int;
            u64 u_long;
            f64 float_64;
            sead::Vector3i vec3i;
            sead::Color4f clr;
            sead::Matrix33f mtx33f;
            sead::Matrix34f mtx34f;
            sead::Quatf quat;
            void* custom;
        };

        Param() : type{ParamType::Invalid}, value{0} {}

        // this is kinda made up but whatever
        template<typename T>
        bool setValue(const sead::SafeString& k, T v) {
            if constexpr (std::is_same<T, sead::SafeString>()) {
                type = ParamType::String;
                *reinterpret_cast<char**>(&value) = const_cast<char*>(v.cstr()); // whatever
            } else if constexpr (std::is_same<T, s32>()) {
                type = ParamType::Int;
                value.s_int = v;
            } else if constexpr (std::is_same<T, f32>()) {
                type = ParamType::F32;
                value.float_32 = v;
            } else if constexpr (std::is_same<T, bool>()) {
                type = ParamType::Bool;
                value.boolean = v;
            } else if constexpr (std::is_same<T, sead::Vector3f>()) {
                type = ParamType::Vec3f;
                value.vec3f = v;
            } else if constexpr (std::is_pointer<T>()) {
                type = ParamType::VoidPtr;
                value.void_ptr = v;
            } else if constexpr (std::is_same<T, s8>()) {
                type = ParamType::S8;
                value.s_byte = v;
            } else if constexpr (std::is_same<T, s16>()) {
                type = ParamType::S16;
                value.s_short = v;
            } else if constexpr (std::is_same<T, s16>()) {
                type = ParamType::S64;
                value.s_long = v;
            } else if constexpr (std::is_same<T, u8>()) {
                type = ParamType::U8;
                value.u_byte = v;
            } else if constexpr (std::is_same<T, u16>()) {
                type = ParamType::U16;
                value.u_short = v;
            } else if constexpr (std::is_same<T, u32>()) {
                type = ParamType::U32;
                value.u_int = v;
            } else if constexpr (std::is_same<T, u64>()) {
                type = ParamType::U64;
                value.u_long = v;
            } else if constexpr (std::is_same<T, f64>()) {
                type = ParamType::F64;
                value.float_64 = v;
            } else if constexpr (std::is_same<T, sead::Vector3i>()) {
                type = ParamType::Vec3i;
                value.vec3i = v;
            } else if constexpr (std::is_same<T, sead::Color4f>()) {
                type = ParamType::Clr4f;
                value.clr = v;
            } else if constexpr (std::is_same<T, sead::Matrix33f>()) {
                type = ParamType::Mtx33f;
                value.mtx33f = v;
            } else if constexpr (std::is_same<T, sead::Matrix34f>()) {
                type = ParamType::Mtx34f;
                value.mtx34f = v;
            } else if constexpr (std::is_same<T, sead::Quatf>()) {
                type = ParamType::Quatf;
                value.quat = v;
            } else {
                return false;
            }
            // don't feel like implementing operator= so we'll just use strncpy
            auto len = strnlen(k.cstr(), 128);
            strncpy(key.getBuffer(), k.cstr(), len + 1 > 128 ? 128 : len + 1);
            *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(&key) + 0x8) = len + 1 > 128 ? 128 : len + 1;

            return true;
        }

        ParamType type;
        sead::FixedSafeString<128> key;
        Value value;
    };
    static_assert(sizeof(Param) == 0xc8);

    template <typename T>
    class State {
        public:
        bool* mpState;
        T* mpValue;
    };

    template <int count>
    struct InitInfo {
        sead::Buffer<Param> params;
        s32 param_count;
        Param buffer[count];

        InitInfo() : params{count, buffer}, param_count{0} {}

        template <typename T>
        bool setParam(const sead::SafeString& key, T value) {
            if (param_count >= params.size()) {
                return false;
            }
            if (params[param_count].setValue(key, value)) {
                ++param_count;
                return true;
            }
            return false;
        }
    };
    static_assert(sizeof(InitInfo<32>) == 0x1918);


    /*// XXX now overwriteparam or something
    namespace engine::event {
    template <int count>
    class BBUpdateInfo {
        public:
        engine::bb::BBUpdateRequestArg mUpdateArg;
        engine::bb::BBUpdateRequest field1_0x18[count];
    };
    } // ns
    */

    class BlackboardBase {
        public:
        sead::SafeString mName;
        virtual ~BlackboardBase(); // XXX i dont need to explicitly declare 2 dtors right?
        virtual void uhhidk();
        virtual void uhhidk2();
        virtual void initializeDefaults();
        virtual void setStringByKey(const sead::SafeString& key, const sead::SafeString& value);
        virtual void setIntByKey(const sead::SafeString& key, s32 value);
        virtual void setFloatByKey(const sead::SafeString& key, float value);
        virtual void setBoolByKey(const sead::SafeString& key, bool value);
        virtual void setVec3fByKey(const sead::SafeString& key, const sead::Vector3f& value);
        virtual void setPtrByKey(); // unimplemented in AS?
        virtual void setPtrByKeyImpl(); // unimplemented in AS?
        virtual void setStringByIndex(s32 index, const sead::SafeString& value);
        virtual void setIntByIndex(s32 index, s32 value);
        virtual void setFloatByIndex(s32 index, float value);
        virtual void setBoolByIndex(s32 index, bool value);
        virtual void setVec3fByIndex(s32 index, const sead::Vector3f& value);
        virtual void setPtrByIndex(); // unimplemented in AS?
        virtual void setPtrByIndexImpl(); // unimplemented in AS?
        virtual s32 setStringByKeyNoExcept(const sead::SafeString& key, const sead::SafeString& value);
        virtual s32 setIntByKeyNoExcept(const sead::SafeString& key, s32 value);
        virtual s32 setFloatByKeyNoExcept(const sead::SafeString& key, float value);
        virtual s32 setBoolByKeyNoExcept(const sead::SafeString& key, bool value);
        virtual s32 setVec3fByKeyNoExcept(const sead::SafeString& key, const sead::Vector3f& value);
        virtual s32 setPtrByKeyNoExcept(); // unimplemented in AS?
        virtual s32 setPtrByKeyNoExceptImpl(); // unimplemented in AS?
        virtual void getStringByKeyImpl(); // XXX can thunk args just remain void?
        virtual void getIntByKeyImpl();
        virtual void getFloatByKeyImpl();
        virtual bool* getBoolByKeyImpl(); // XXX why is this one different?
        virtual void getVec3fByKeyImpl();
        virtual void* getPtrByKeyImpl(); // unimplemented in AS?
        virtual void getStringByIndexImpl(s32 index);
        virtual s32* getIntByIndexImpl(s32 index);
        virtual float* getFloatByIndexImpl(s32 index);
        virtual bool* getBoolByIndexImpl(s32 index);
        virtual sead::Vector3f* getVec3fByIndexImpl(s32 index);
        virtual void getPtrByIndexImpl(s32 index);
        virtual const char* getStringByIndexCheckChange(s32 index, u8* outChangelogCount); // XXX changelog?
        virtual s32* getIntByIndexCheckChange(s32 index, u8* outChangelogCount);
        virtual float* getFloatByIndexCheckChange(s32 index, u8* outChangelogCount);
        virtual bool* getBoolByIndexCheckChange(s32 index, u8* outChangelogCount);
        virtual sead::Vector3f* getVec3fByIndexCheckChange(s32 index, u8* outChangelogCount);
        virtual void getPtrByIndexCheckChange(); // unimplemented in AS?
        virtual const char* getStringByKey(const sead::SafeString& key);
        virtual s32 getIntByKey(const sead::SafeString& key);
        virtual float getFloatByKey(const sead::SafeString& key);
        virtual bool getBoolByKey(const sead::SafeString& key);
        virtual const sead::Vector3f& getVec3fByKey(const sead::SafeString& key);
        virtual void* getPtrByKey(); // unimplemented in AS?
        virtual const char* getStringByIndex(s32 index);
        virtual s32 getIntByIndex(s32 index);
        virtual float getFloatByIndex(s32 index);
        virtual bool getBoolByIndex(s32 index);
        virtual const sead::Vector3f& getVec3fByIndex(s32 index);
        virtual void* getPtrByIndex(s32 index);
        virtual const char* getStringKeyByIndex(s32 index);
        virtual const char* getIntKeyByIndex(s32 index);
        virtual const char* getFloatKeyByIndex(s32 index);
        virtual const char* getBoolKeyByIndex(s32 index);
        virtual const char* getVec3fKeyByIndex(s32 index);
        virtual const char* getPtrKeyByIndex(s32 index);
        virtual const char* getStringFilenameByIndex(s32 index);
        virtual const char* getIntFilenameByIndex(s32 index);
        virtual const char* getFloatFilenameByIndex(s32 index);
        virtual const char* getBoolFilenameByIndex(s32 index);
        virtual const char* getVec3fFilenameByIndex(s32 index);
        virtual const char* getPtrFilenameByIndex(s32 index);
        virtual const char* getStringDescriptionByIndex(s32 index);
        virtual const char* getIntDescriptionByIndex(s32 index);
        virtual const char* getFloatDescriptionByIndex(s32 index);
        virtual const char* getBoolDescriptionByIndex(s32 index);
        virtual const char* getVec3fDescriptionByIndex(s32 index);
        virtual const char* getPtrDescriptionByIndex(s32 index);
        virtual u16 getStringCount();
        virtual u16 getIntCount();
        virtual u16 getFloatCount();
        virtual u16 getBoolCount();
        virtual u16 getVec3fCount();
        virtual u16 getPtrCount();
        virtual s32 getStringIndexByKey(const sead::SafeString& key);
        virtual s32 getIntIndexByKey(const sead::SafeString& key);
        virtual s32 getFloatIndexByKey(const sead::SafeString& key);
        virtual s32 getBoolIndexByKey(const sead::SafeString& key);
        virtual s32 getVec3fIndexByKey(const sead::SafeString& key);
        virtual s32 getPtrIndexByKey(const sead::SafeString& key);
        virtual const char* getStringDefaultByKey(const sead::SafeString& key);
        virtual s32 getIntDefaultByKey(const sead::SafeString& key);
        virtual float getFloatDefaultByKey(const sead::SafeString& key);
        virtual bool getBoolDefaultByKey(const sead::SafeString& key);
        virtual const sead::Vector3f& getVec3fDefaultByKey(const sead::SafeString& key);
        virtual void* getPtrDefaultByKey(const sead::SafeString& key);
        virtual void setBlackboardAccessor(engine::actor::BlackboardAccessor* value);
        virtual void resetBlackboard();
        virtual void* field94_0x2f0();
        virtual void initialize(sead::Heap*); // ???
    };

} // namespace bb
