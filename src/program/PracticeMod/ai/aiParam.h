#pragma once

#include <math/seadVector.h>
#include <prim/seadSafeString.h>

namespace ai {

enum class ParameterType {
    S32, Bool, F32, Str, Vec3f, Ptr
};

namespace detail {

template <ParameterType Type>
struct ParamToType;

template <> struct ParamToType<ParameterType::S32> { using type_t = s32; };
template <> struct ParamToType<ParameterType::Bool> { using type_t = bool; };
template <> struct ParamToType<ParameterType::F32> { using type_t = f32; };
template <> struct ParamToType<ParameterType::Str> { using type_t = sead::SafeString; };
template <> struct ParamToType<ParameterType::Vec3f> { using type_t = sead::Vector3f; };
template <> struct ParamToType<ParameterType::Ptr> { using type_t = void*; };

} // namespace detail

// Properties
template <ParameterType T>
class Property;

template <>
class Property<ParameterType::Str> {
    using Type = typename detail::ParamToType<ParameterType::Str>::type_t;

public:
    Property() = default;
    ~Property() = default;

    Type* getPtr() const {
        return mPtr;
    }

    Type get() const {
        return *mPtr;
    }

    operator Type() const {
        return get();
    }

private:
    Type* mPtr;
    Type mValue; // for some reason they store this
};

template <ParameterType T>
class Property {
    using Type = typename detail::ParamToType<T>::type_t;

public:
    Property() = default;
    ~Property() = default;

    Type* getPtr() const {
        return mPtr;
    }

    Type get() const {
        return *mPtr;
    }

    operator Type() const {
        return get();
    }

private:
    Type* mPtr;
};

// Inputs
template <ParameterType T>
class Input;

template <>
class Input<ParameterType::Bool> {
protected:
    using Type = detail::ParamToType<ParameterType::Bool>::type_t;

public:
    // since bools have an alignment of 1, we can't store flags in the bottom two bits safely
    bool isPulseTLS() const {
        return false;
    }

    bool isOutput() const {
        return true;
    }

    ~Input() = default;

    Type* getPtr() const {
        return reinterpret_cast<Type*>(mPtr);
    }

    Type get() const {
        return *getPtr();
    }

    operator Type() const {
        return get();
    }

protected:
    uintptr_t mPtr = 0;
};

template <>
class Input<ParameterType::Str> {
protected:
    using Type = detail::ParamToType<ParameterType::Str>::type_t;

public:
    bool isPulseTLS() const {
        return mPtr >> 1 & 1;
    }
    
    bool isOutput() const {
        return mPtr & 1;
    }

    ~Input() = default;

    Type* getPtr() const {
        return reinterpret_cast<Type*>(mPtr & cMask);
    }

    Type get() const {
        return *getPtr();
    }

    operator Type() const {
        return get();
    }

private:
    uintptr_t mPtr = 0;
    sead::SafeString mValue; // for some reason they store this

    static constexpr u64 cMask = 0xfffffffffffffffc;
};

template <>
class Input<ParameterType::Ptr> {
protected:
    using Type = detail::ParamToType<ParameterType::Ptr>::type_t;

public:
    bool isPulseTLS() const {
        return mPtr >> 1 & 1;
    }

    bool isOutput() const {
        return mPtr & 1;
    }

    ~Input() = default;

protected:
    uintptr_t mPtr = 0;

    static constexpr u64 cMask = 0xfffffffffffffffc;
};

template <ParameterType T>
class Input {
protected:
    using Type = detail::ParamToType<T>::type_t;

public:
    bool isPulseTLS() const {
        return mPtr >> 1 & 1;
    }

    bool isOutput() const {
        return mPtr & 1;
    }

    Input() = default;
    ~Input() = default;

    Type* getPtr() const {
        return reinterpret_cast<Type*>(mPtr & cMask);
    }

    Type get() const {
        return *getPtr();
    }

    operator Type() const {
        return get();
    }

private:
    uintptr_t mPtr = 0;

    static constexpr u64 cMask = 0xfffffffffffffffc;
};

template <typename T>
class PtrInput : public Input<ParameterType::Ptr> {
    using Type = T*;

public:
    Type* getPtr() const {
        return reinterpret_cast<Type*>(mPtr & cMask);
    }

    Type get() const {
        return *getPtr();
    }

    operator Type() const {
        return get();
    }
};

// Outputs
template <ParameterType T>
class Output;

template <>
class Output<ParameterType::Bool> {
protected:
    using Type = detail::ParamToType<ParameterType::Bool>::type_t;

public:
    // since bools have an alignment of 1, we can't store flags in the bottom two bits safely
    bool isPulseTLS() const {
        return false;
    }

    bool isOutput() const {
        return true;
    }

    Output() = default;
    ~Output() = default;

    Type* getPtr() const {
        return reinterpret_cast<Type*>(mPtr);
    }

    Type get() const {
        return *getPtr();
    }

    operator Type() const {
        return get();
    }

    void set(Type value);

private:
    uintptr_t mPtr = 0;
};

template <>
class Output<ParameterType::Ptr> {
protected:
    using Type = detail::ParamToType<ParameterType::Ptr>::type_t;

public:
    bool isPulseTLS() const {
        return mPtr >> 1 & 1;
    }

    bool isOutput() const {
        return mPtr & 1;
    }

    Output() = default;
    ~Output() = default;

protected:
    uintptr_t mPtr = 0;

    static constexpr u64 cMask = 0xfffffffffffffffc;
};

template <>
class Output<ParameterType::Str> {
protected:
    using Type = detail::ParamToType<ParameterType::Str>::type_t;

public:
    bool isPulseTLS() const {
        return mPtr >> 1 & 1;
    }

    bool isOutput() const {
        return mPtr & 1;
    }

    Output() = default;
    ~Output() = default;

    Type* getPtr() const {
        return reinterpret_cast<Type*>(mPtr & cMask);
    }

    Type get() const {
        return *getPtr();
    }

    operator Type() const {
        return get();
    }

    void set(const char* value);

private:
    uintptr_t mPtr = 0;

    static constexpr u64 cMask = 0xfffffffffffffffc;
};

template <>
class Output<ParameterType::Vec3f> {
protected:
    using Type = detail::ParamToType<ParameterType::Vec3f>::type_t;

public:
    bool isPulseTLS() const {
        return mPtr >> 1 & 1;
    }

    bool isOutput() const {
        return mPtr & 1;
    }

    Output() = default;
    ~Output() = default;

    Type* getPtr() const {
        return reinterpret_cast<Type*>(mPtr & cMask);
    }

    Type get() const {
        return *getPtr();
    }

    operator Type() const {
        return get();
    }

    void set(const sead::Vector3f& value);

private:
    uintptr_t mPtr = 0;

    static constexpr u64 cMask = 0xfffffffffffffffc;
};

template <ParameterType T>
class Output {
protected:
    using Type = detail::ParamToType<T>::type_t;

public:
    bool isPulseTLS() const {
        return mPtr >> 1 & 1;
    }

    bool isOutput() const {
        return mPtr & 1;
    }

    Output() = default;
    ~Output() = default;

    Type* getPtr() const {
        return reinterpret_cast<Type*>(mPtr & cMask);
    }

    Type get() const {
        return *getPtr();
    }

    operator Type() const {
        return get();
    }

    void set(Type value);

private:
    uintptr_t mPtr = 0;

    static constexpr u64 cMask = 0xfffffffffffffffc;
};

template <typename T>
class PtrOutput : public Output<ParameterType::Ptr> {
    using Type = T*;

public:
    void set(Type value);
};

using S32Property = Property<ParameterType::S32>;
using BoolProperty = Property<ParameterType::Bool>;
using F32Property = Property<ParameterType::F32>;
using Vec3fProperty = Property<ParameterType::Vec3f>;
using StrProperty = Property<ParameterType::Str>;

using S32Input = Input<ParameterType::S32>;
using BoolInput = Input<ParameterType::Bool>;
using F32Input = Input<ParameterType::F32>;
using Vec3fInput = Input<ParameterType::Vec3f>;
using StrInput = Input<ParameterType::Str>;
using VoidPtrInput = Input<ParameterType::Ptr>;

using S32Output = Output<ParameterType::S32>;
using BoolOutput = Output<ParameterType::Bool>;
using F32Output = Output<ParameterType::F32>;
using Vec3fOutput = Output<ParameterType::Vec3f>;
using StrOutput = Output<ParameterType::Str>;
using VoidPtrOutput = Output<ParameterType::Ptr>;

// TODO: multiparams

} // namespace ai
