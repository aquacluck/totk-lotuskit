#pragma once

#include <nn/types.h>

namespace nn {
namespace g3d2 {
struct DDLDeclarations {
    u64 _0;
    u32 _8;
    u32 DDLDeclarations_xC;
    u64 _10;
    u64 _18;
};

class BindFuncTable {
    struct StringLength {
        size_t length;
        const char* content;
    };
    struct EntryPointer {
        void* something_0;
        nn::g3d2::BindFuncTable::StringLength* string;
    };

private:
    int lengths[4];
    nn::g3d2::BindFuncTable::EntryPointer strings[4];
};

}  // namespace g3d2
}  // namespace nn
