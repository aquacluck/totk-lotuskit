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
rm -rf lib



### Fetch sources
if [[ "$KEEP_TMPGIT" != "1" ]]; then
    echo
    echo "clone upstream..."
    rm -rf tmpgit

    # temp repo, yeah its slow ~1m for me
    # last base: https://github.com/dt-12345/totk/commit/67e7145c8698f45ce0be6969bdb1184e1349bc21
    git clone --recursive --depth 1 https://github.com/dt-12345/totk tmpgit
    echo
    echo
fi



### Copy just what we need
# XXX lib/EventFlow/

echo "cp lib/NintendoSDK"
mkdir -p lib/NintendoSDK
cp -r tmpgit/lib/NintendoSDK/include lib/NintendoSDK

echo "cp lib/agl"
mkdir -p lib/agl
cp -r tmpgit/lib/agl/include lib/agl

echo "cp lib/sead"
mkdir -p lib/sead
# headers
cp -r tmpgit/lib/sead/include lib/sead
# impl
mkdir -p lib/sead/modules/src/heap
cp {tmpgit/,}lib/sead/modules/src/heap/seadHeapMgr.cpp
mkdir -p lib/sead/modules/src/thread/nin
cp {tmpgit/,}lib/sead/modules/src/thread/nin/seadCriticalSectionNin.cpp

echo "cp lib/zstd"
mkdir -p lib/zstd/lib/{compress,decompress,common}
cp tmpgit/lib/zstd/lib/*.h lib/zstd/lib
#cp tmpgit/lib/zstd/lib/compress/*.h lib/zstd/lib/compress
#cp tmpgit/lib/zstd/lib/decompress/*.h lib/zstd/lib/decompress
#cp tmpgit/lib/zstd/lib/common/*.h lib/zstd/lib/common



### Apply patches
# gen: git diff --binary --cached lib > patches/0001-nn-init.patch # staged changes only, afaik required for patching in new files
echo
echo "patching..."
for patchfile in patches/*.patch; do
    echo "git apply $patchfile"
    git apply $patchfile
done
