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
    # last base: https://github.com/aquacluck/nnheaders/commit/ccfa71dbafaf97b225805361f6eaced3fd315cb7
    #            dt initial fixes for totk
    # last upstream: https://github.com/open-ead/nnheaders/commit/86d94df95e8cd540817a698016e7b5a0575d8b5d
    #                g3d/ModelObj: Add GetNumShapes()
    git clone --depth 1 --single-branch --branch totk https://github.com/aquacluck/nnheaders tmpgit
    echo
    echo
fi



### Copy just what we need
echo "cp include"
cp -r tmpgit/include include



### Apply patches
# gen: git diff --binary lib > patches/0001-nn-init.patch
echo
echo "patching..."
for patchfile in patches/*.patch; do
    echo "git apply $patchfile"
    git apply $patchfile
done
