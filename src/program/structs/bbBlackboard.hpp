#pragma once

#include <container/seadBuffer.h>
#include <gfx/seadColor.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <prim/seadSafeString.h>

#include <cstring>
#include <type_traits>

namespace bb {

enum class ParamType : u32 {
    Invalid = 0,
    String = 1,
    Int = 2,
    F32 = 3,
    Bool = 4,
    Vec3f = 5,
    VoidPtr = 6,
    S8 = 7,
    S16 = 8,
    S64 = 9,
    U8 = 10,
    U16 = 11,
    U32 = 12,
    U64 = 13,
    F64 = 14,
    Vec3i = 15,
    Clr4f = 16,
    Mtx33f = 17,
    Mtx34f = 18,
    Quatf = 19,
    CustomType = 20,
};

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

} // namespace bb
