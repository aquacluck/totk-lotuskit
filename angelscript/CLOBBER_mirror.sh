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
    # last base: Version 2.38.0 WIP - 2024/11/22 - "Fixed crash when passing function pointer without @ prefix to argument expecting asOBJ_ASHANDLE"
    #            https://github.com/codecat/angelscript-mirror/commit/57ef77e4793ca14f448f485f52392ab0eefe968b
    #            https://sourceforge.net/p/angelscript/code/2988/log/
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
#TODO addons? scripthandle(ref), scriptmath, scriptmathcomplex
cp tmpgit/sdk/add_on/scriptarray/*.h include
cp tmpgit/sdk/add_on/scriptarray/*.cpp source
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
