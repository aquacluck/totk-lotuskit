#pragma once

#include <basis/seadNew.h>
#include <nn/types.h>

#include <type_traits>
#include <utility>

// not sure if this is correct, I'm not good at this kinda stuff and it took a lot of messing around
// to even get something that kinda worked

namespace sead {

template <typename F>
class IFunction;

class AnyFunctionBase {};

template <typename R, typename... A>
class IFunction<R(A...)> {
public:
    IFunction() = default;

    // usage should mostly parallel sead::IDelegate
    virtual R invoke(A... args) const { return R(args...); };
    virtual bool isNoDummy() const { return false; }
    virtual void copyTo(AnyFunctionBase* other) const { new (other) IFunction(*this); };

    virtual ~IFunction();

    inline R operator()(A... args) const { return invoke(args...); }
};

template <typename F>
class Function;

template <typename R, typename... A>
class Function<R(A...)> : public IFunction<R(A...)> {
public:
    using FuncType = R (*)(A...);
    explicit Function(FuncType f) : mFunction(f) {}

    R invoke(A... args) const override { return (*mFunction)(args...); }
    bool isNoDummy() const override { return true; }
    void copyTo(AnyFunctionBase* other) const override { new (other) Function(*this); }

    ~Function() override;

    inline R operator()(A... args) const { return invoke(args...); }

private:
    FuncType mFunction;
};

template <typename L, typename F>
class LambdaFunction;

template <typename L, typename R, typename... A>
class LambdaFunction<L, R(A...)> : IFunction<R(A...)> {
public:
    using FuncType = R (*)(A...);
    explicit LambdaFunction(L l) : mLambda(std::move(l)) {}

    R invoke(A... args) const override { return mLambda(args...); }
    bool isNoDummy() const override { return true; }
    void copyTo(AnyFunctionBase* other) const override { new (other) LambdaFunction(*this); }

    ~LambdaFunction() override;

    inline R operator()(A... args) const { return invoke(args...); }

private:
    L mLambda;
};

template <class C, typename F>
class MemberFunction;

template <class C, typename R, typename... A>
class MemberFunction<C, R(A...)> : IFunction<R(A...)> {
public:
    using FuncType = R (C::*)(A...);
    explicit MemberFunction(C* instance, FuncType f) : mInstance(instance), mFunction(f) {}

    R invoke(A... args) const override { return (mInstance->*mFunction)(args...); }
    bool isNoDummy() const override { return true; }
    void copyTo(AnyFunctionBase* other) const override { new (other) MemberFunction(*this); }

    ~MemberFunction() override;

    inline R operator()(A... args) const { return invoke(args...); }

private:
    C* mInstance;
    FuncType mFunction;
};

template <typename FuncType, typename F>
auto makeFunction(F& f) {
    return Function<FuncType>(f);
}

template <typename FuncType, typename F>
auto makeFunction(F&& f) {
    return Function<FuncType>(std::forward<F>(f));
}

template <typename FuncType, typename L>
auto makeLambda(L& l) {
    return LambdaFunction<L, FuncType>(std::move(l));
}

template <typename FuncType, typename L>
auto makeLambda(L&& l) {
    return LambdaFunction<L, FuncType>(std::forward<L>(l));
}

template <typename FuncType, class C, typename F>
auto makeMemberFunction(C* instance, F& f) {
    return MemberFunction<C, FuncType>(instance, f);
}

template <typename FuncType, class C, typename F>
auto makeMemberFunction(C* instance, F&& f) {
    return MemberFunction<C, FuncType>(instance, std::forward<F>(f));
}

namespace detail {

template <size_t StorageSize, typename F>
class AnyFunctionImpl;

template <size_t StorageSize, typename R, typename... A>
class AnyFunctionImpl<StorageSize, R(A...)> : AnyFunctionBase {
public:
    class UnbindDummy final : public IFunction<R(A...)> {
    public:
        R invoke(A...) const override { return {}; }
        bool isNoDummy() const override { return false; }
        void copyTo(AnyFunctionBase* other) const override { new (other) UnbindDummy(*this); }

        ~UnbindDummy() override;
    };

    using FuncType = R (*)(A...);
    AnyFunctionImpl() { new (&mStorage) UnbindDummy(); }
    template <typename T>
    AnyFunctionImpl(T other) {
        *this = std::move(other);
    }

    template <typename T>
    AnyFunctionImpl& operator=(T& other) {
        copyFrom(&other);
        return *this;
    }

    inline R invoke(A... args) const {
        return reinterpret_cast<const IFunction<R(A...)>*>(&mStorage)->invoke(args...);
    }
    inline bool isNoDummy() const {
        return reinterpret_cast<const IFunction<R(A...)>*>(&mStorage)->isNoDummy();
    }
    inline void copyTo(AnyFunctionBase* other) const {
        reinterpret_cast<const IFunction<R(A...)>*>(&mStorage)->copyTo(other);
    }
    template <typename T>
    inline void copyFrom(T* other) {
        other->copyTo(this);
    }

    inline R operator()(A... args) { return invoke(args...); }
    inline R operator()(A... args) const { return invoke(args...); }

    explicit operator bool() const { return isNoDummy(); }

protected:
    std::aligned_storage_t<StorageSize, 8> mStorage;
};

}  // namespace detail

template <typename F>
class AnyFunction;

template <typename R, typename... A>
class AnyFunction<R(A...)> : public detail::AnyFunctionImpl<0x20, R(A...)> {
public:
    using Base = detail::AnyFunctionImpl<0x20, R(A...)>;
    using Base::Base;
    using Base::operator=;
};

}  // namespace sead