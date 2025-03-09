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
rm -rf include



### Fetch sources
if [[ "$KEEP_TMPGIT" != "1" ]]; then
    echo
    echo "clone upstream..."
    rm -rf tmpgit

    # temp repo
    # last base: https://github.com/aquacluck/agl/commit/48ad56f30e44959732ff8a682a6888b10cf159f3
    #            dt intial fixes for totk
    # last upstream: https://github.com/open-ead/agl/commit/e00c665fb8f5d54c90e9f8559fcfd3eec2e888fe
    #                common/DrawContext: Use ShaderEnum instead of stubbing own enum
    git clone --depth 1 --single-branch --branch totk https://github.com/aquacluck/agl tmpgit
    echo
    echo
fi



### Copy just what we need
echo "cp include"
cp -r tmpgit/include include



### Apply patches
# gen: git diff --cached --binary include > patches/0001-agl-init.patch
echo
echo "patching..."
for patchfile in patches/*.patch; do
    echo "git apply $patchfile"
    git apply $patchfile
done
