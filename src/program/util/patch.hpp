#pragma once
#include <string>

namespace lotuskit::util::patch {
    // implements reversable ips patching at runtime
    void PrepareRevertPatches();
    void PatchInstall(const std::string& patchsetName);
    void PatchRevert(const std::string& patchsetName);

} // ns

