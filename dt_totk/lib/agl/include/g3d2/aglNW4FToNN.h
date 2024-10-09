#pragma once

namespace nn::g3d2 {
class ResFile;
}

namespace agl::g3d2 {

class ResFile {
public:
    static void BindTexture(nn::g3d2::ResFile*, nn::g3d2::ResFile*);
    static void Cleanup(nn::g3d2::ResFile*);
};

}  // namespace agl::g3d
