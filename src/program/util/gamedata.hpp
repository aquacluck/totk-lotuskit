#pragma once
#include "util/alloc.hpp"
using String = lotuskit::String;

namespace lotuskit::util::gamedata {
    void* getGameDataMgr();

    u32 getEnum(u32 hash, u32 default_=0, bool direct=false);
    u32 getEnumKey(const String& key, u32 default_=0, bool direct=false);
    void setEnum(u32 hash, u32 value);
    void setEnumKey(const String& key, u32 value);

    // TODO EnumArray?
    // TODO Structs
    // TODO Binary, BinaryArray?

    bool getBool(u32 hash, bool default_=false, bool direct=false);
    bool getBoolKey(const String& key, bool default_=false, bool direct=false);
    void setBool(u32 hash, bool value);
    void setBoolKey(const String& key, bool value);

    // TODO BoolArray?

    // TODO
    //bool getBool64bitKey(u64 hash, bool default_=false, bool direct=false);
    //bool getBool64bitKeyKey(const String& key, bool default_=false, bool direct=false);
    //void setBool64bitKey(u64 hash, bool value);
    //void setBool64bitKeyKey(const String& key, bool value);

    // TODO boolexp

    // TODO SaveTypeHash?

    s32 getInt(u32 hash, s32 default_=0, bool direct=false);
    s32 getIntKey(const String& key, s32 default_=0, bool direct=false);
    void setInt(u32 hash, s32 value);
    void setIntKey(const String& key, s32 value);

    // TODO IntArray

    u32 getUInt(u32 hash, u32 default_=0, bool direct=false);
    u32 getUIntKey(const String& key, u32 default_=0, bool direct=false);
    void setUInt(u32 hash, u32 value);
    void setUIntKey(const String& key, u32 value);

    // TODO UIntArray

    // TODO Int64, Int64Array?

    float getFloat(u32 hash, float default_=0, bool direct=false);
    float getFloatKey(const String& key, float default_=0, bool direct=false);
    void setFloat(u32 hash, float value);
    void setFloatKey(const String& key, float value);

    // TODO FloatArray
    // TODO Vector2, Vector2Array, Vector3, Vector3Array?
    // TODO Vector3f
    // TODO StringArray, String16, String16Array, String32, String32Array?
    // TODO String64, String64Array
    // TODO WString16, WString16Array, WString32, WString32Array, WString64, WString64Array?

} // ns

