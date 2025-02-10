#!/bin/bash



### This will CLOBBER ur working directory ok thanks
read -p "CLOBBER TIME? [y/N]? " -n 1
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo
    echo ok i sleep
    exit 1
fi
echo
echo OH BOY ITS CLOBBERIN TIME
rm -rf include source



### Fetch sources
if [[ "$KEEP_TMPGIT" != "1" ]]; then
    echo
    echo "clone upstream..."
    rm -rf tmpgit

    # temp repo, clone ~1.5s
    # last base: Version 2.38.0 WIP - 2025/2/7 - "Fixed invalid loop in FindNextFunctionCalled that could read out of bounds"
    #            https://github.com/codecat/angelscript-mirror/commit/b38423734656e3924dddf843b3f234d09d043a03
    #            https://sourceforge.net/p/angelscript/code/2994/log/
    git clone --recursive --depth 1 https://github.com/codecat/angelscript-mirror tmpgit
    echo
    echo
fi



### Copy just what we need
mkdir -p include source
cp tmpgit/sdk/angelscript/include/angelscript.h include
cp tmpgit/sdk/angelscript/source/*.h source
cp tmpgit/sdk/angelscript/source/as_callfunc_arm64_gcc.S source
cp tmpgit/sdk/angelscript/source/as_[abd-z]*.cpp source
cp tmpgit/sdk/angelscript/source/as_callfunc.cpp source
cp tmpgit/sdk/angelscript/source/as_callfunc_arm64.cpp source
cp tmpgit/sdk/angelscript/source/as_compiler.cpp source
cp tmpgit/sdk/angelscript/source/as_configgroup.cpp source
cp tmpgit/sdk/angelscript/source/as_context.cpp source
#TODO addons? scripthandle(ref)
cp tmpgit/sdk/add_on/scriptarray/*.h include
cp tmpgit/sdk/add_on/scriptarray/*.cpp source
cp tmpgit/sdk/add_on/scriptmath/*.h include
cp tmpgit/sdk/add_on/scriptmath/*.cpp source
cp tmpgit/sdk/add_on/scriptstdstring/*.h include
cp tmpgit/sdk/add_on/scriptstdstring/*.cpp source



### Apply patches
# gen: git diff --binary --cached include source > patches/0001-as-init.patch # staged changes only, afaik required for patching in new files
echo
echo "patching..."
for patchfile in patches/*.patch; do
    echo "git apply $patchfile"
    git apply $patchfile
done
