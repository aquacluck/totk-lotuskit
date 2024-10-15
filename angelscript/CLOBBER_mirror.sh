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
rm -rf include source projects/cmake # lib is generated



### Fetch sources
if [[ "$KEEP_TMPGIT" != "1" ]]; then
    echo
    echo "clone upstream..."
    rm -rf tmpgit

    # temp repo, clone ~1.5s
    # last base: Version 2.38.0 WIP - 2024/10/13 - "Fixed the test_dump.cpp test case for string::regexFind"
    #            https://github.com/codecat/angelscript-mirror/commit/094e827ca022ae7c9cc75b8402081a55ee3c846c
    #            https://sourceforge.net/p/angelscript/code/2974/log/
    git clone --recursive --depth 1 https://github.com/codecat/angelscript-mirror tmpgit
    echo
    echo
fi



### Copy just what we need
mkdir -p include source projects/cmake
mkdir lib && touch lib/.gitkeep
cp tmpgit/sdk/angelscript/include/angelscript.h include
cp tmpgit/sdk/angelscript/source/* source
cp -r tmpgit/sdk/angelscript/projects/cmake/* projects/cmake
#TODO addons



### Apply patches
# gen: git diff --binary lib > patches/0001-nn-init.patch
#echo
#echo "patching..."
#for patchfile in patches/*.patch; do
#    echo "git apply $patchfile"
#    git apply $patchfile
#done
