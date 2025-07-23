#include "exlaunch.hpp"
#include "util/gamedata.hpp"
#include "util/hash.hpp"
#include "TextWriter.hpp"
constexpr auto murmur32 = lotuskit::util::hash::murmur32;

namespace lotuskit::util::gamedata {
    void* getGameDataMgr() {
        return *EXL_SYM_RESOLVE<void**>("gmd::GameDataMgr::sInstance");
    }

    u32 getEnum(u32 hash, u32 default_, bool direct) {
        u32 ret = default_;
        using impl_t = u64 (void*, u32*, u32);
        const String sym = direct ? "gmd::GameDataMgr::getEnumDirect" : "gmd::GameDataMgr::getEnum";
        auto impl = EXL_SYM_RESOLVE<impl_t*>(sym.c_str());
        bool isOK = impl(getGameDataMgr(), &ret, hash);
        if (!isOK) { ret = default_; }
        return ret;
    }
    u32 getEnumKey(const String& key, u32 default_, bool direct) {
        return getEnum(murmur32(key), default_, direct);
    }
    void setEnum(u32 hash, u32 value) {
        using impl_t = void (void*, u32, u32);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("gmd::GameDataMgr::setEnum");
        return impl(getGameDataMgr(), value, hash);
    }
    void setEnumKey(const String& key, u32 value) {
        setEnum(murmur32(key), value);
    }

    // TODO EnumArray?
    // TODO Structs
    // TODO Binary, BinaryArray?

    bool getBool(u32 hash, bool default_, bool direct) {
        bool ret = default_;
        using impl_t = u64 (void*, bool*, u32);
        const String sym = direct ? "gmd::GameDataMgr::getBoolDirect" : "gmd::GameDataMgr::getBool";
        auto impl = EXL_SYM_RESOLVE<impl_t*>(sym.c_str());
        bool isOK = impl(getGameDataMgr(), &ret, hash);
        if (!isOK) { ret = default_; }
        return ret;
    }
    bool getBoolKey(const String& key, bool default_, bool direct) {
        return getBool(murmur32(key), default_, direct);
    }
    void setBool(u32 hash, bool value) {
        using impl_t = void (void*, u64, u32);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("gmd::GameDataMgr::setBool");
        return impl(getGameDataMgr(), value, hash);
    }
    void setBoolKey(const String& key, bool value) {
        setBool(murmur32(key), value);
    }

    // TODO BoolArray?

    // TODO
    //bool getBool64bitKey(u64 hash, bool default_, bool direct) {}
    //bool getBool64bitKeyKey(const String& key, bool default_=false, bool direct=false);
    //void setBool64bitKey(u64 hash, bool value) {}
    //void setBool64bitKeyKey(const String& key, bool value);

    // TODO boolexp

    // TODO SaveTypeHash?

    s32 getInt(u32 hash, s32 default_, bool direct) {
        s32 ret = default_;
        using impl_t = u64 (void*, s32*, u32);
        const String sym = direct ? "gmd::GameDataMgr::getIntDirect" : "gmd::GameDataMgr::getInt";
        auto impl = EXL_SYM_RESOLVE<impl_t*>(sym.c_str());
        bool isOK = impl(getGameDataMgr(), &ret, hash);
        if (!isOK) { ret = default_; }
        return ret;
    }
    s32 getIntKey(const String& key, s32 default_, bool direct) {
        return getInt(murmur32(key), default_, direct);
    }
    void setInt(u32 hash, s32 value) {
        using impl_t = void (void*, s32, u32);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("gmd::GameDataMgr::setInt");
        return impl(getGameDataMgr(), value, hash);
    }
    void setIntKey(const String& key, s32 value) {
        setInt(murmur32(key), value);
    }

    // TODO IntArray

    u32 getUInt(u32 hash, u32 default_, bool direct) {
        u32 ret = default_;
        if (direct) { // FIXME
            lotuskit::TextWriter::toastf(30, "[script] getUIntDirect unsupported!\n");
            return ret;
        }
        using impl_t = u64 (void*, u32*, u32);
        const String sym = direct ? "gmd::GameDataMgr::getUIntDirect" : "gmd::GameDataMgr::getUInt";
        auto impl = EXL_SYM_RESOLVE<impl_t*>(sym.c_str());
        bool isOK = impl(getGameDataMgr(), &ret, hash);
        if (!isOK) { ret = default_; }
        return ret;
    }
    u32 getUIntKey(const String& key, u32 default_, bool direct) {
        return getUInt(murmur32(key), default_, direct);
    }
    void setUInt(u32 hash, u32 value) {
        using impl_t = void (void*, u32, u32);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("gmd::GameDataMgr::setUInt");
        return impl(getGameDataMgr(), value, hash);
    }
    void setUIntKey(const String& key, u32 value) {
        setUInt(murmur32(key), value);
    }

    // TODO UIntArray

    // TODO Int64, Int64Array?

    float getFloat(u32 hash, float default_, bool direct) {
        float ret = default_;
        using impl_t = u64 (void*, float*, u32);
        const String sym = direct ? "gmd::GameDataMgr::getFloatDirect" : "gmd::GameDataMgr::getFloat";
        auto impl = EXL_SYM_RESOLVE<impl_t*>(sym.c_str());
        bool isOK = impl(getGameDataMgr(), &ret, hash);
        if (!isOK) { ret = default_; }
        return ret;
    }
    float getFloatKey(const String& key, float default_, bool direct) {
        return getFloat(murmur32(key), default_, direct);
    }
    void setFloat(u32 hash, float value) {
        using impl_t = void (void*, float, u32);
        auto impl = EXL_SYM_RESOLVE<impl_t*>("gmd::GameDataMgr::setFloat");
        return impl(getGameDataMgr(), value, hash);
    }
    void setFloatKey(const String& key, float value) {
        setFloat(murmur32(key), value);
    }

    // TODO FloatArray
    // TODO Vector2, Vector2Array, Vector3, Vector3Array?
    // TODO Vector3f
    // TODO StringArray, String16, String16Array, String32, String32Array?
    // TODO String64, String64Array
    // TODO WString16, WString16Array, WString32, WString32Array, WString64, WString64Array?

} // ns

