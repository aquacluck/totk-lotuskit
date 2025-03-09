#pragma once

#ifdef __aarch64__

#include <arm_neon.h>

namespace nn::util::neon {

// absolutely no idea if these are correct, somebody pls fix these if you can lol

struct Vector3fType {
    union {
        struct {
            float32_t x;
            float32_t y;
            float32_t z;
        };
        float32_t v[3];
    };
};

struct Vector4fType {
    union {
        struct {
            float32_t x;
            float32_t y;
            float32_t z;
            float32_t w;
        };
        float32x4_t v;
    };
};

struct MatrixRowMajor4x3Type {
    union {
        struct {
            Vector4fType r0;
            Vector4fType r1;
            Vector4fType r2;
        };
        float32x4x3_t m;
    };
};

struct MatrixRowMajor4x4Type {
    union {
        struct {
            Vector4fType r0;
            Vector4fType r1;
            Vector4fType r2;
            Vector4fType r3;
        };
        float32x4x4_t m;
    };
};

struct MatrixColumnMajor4x3Type {
    union {
        struct {
            Vector3fType c0;
            Vector3fType c1;
            Vector3fType c2;
            Vector3fType c3;
        };
        float32x4x3_t m;
    };
};

struct MatrixColumnMajor4x4Type {
    union {
        struct {
            Vector4fType c0;
            Vector4fType c1;
            Vector4fType c2;
            Vector4fType c3;
        };
        float32x4x4_t m;
    };
};

}

#endif