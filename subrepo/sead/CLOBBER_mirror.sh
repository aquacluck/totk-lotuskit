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
    # last base: https://github.com/aquacluck/sead/commit/e3dc8db76ba0ba9f779f3985a99f5fa4b30cef9a
    #            revert clang-format to upstream to reduce diff noise
    # last upstream: https://github.com/open-ead/sead/commit/e6dd5237035bb4e54b23765162a9e88213fde04c
    #                math/Matrix: Fix makeSRT
    git clone --depth 1 --single-branch --branch totk https://github.com/aquacluck/sead tmpgit
    echo
    echo
fi



echo "cp include"
cp -r tmpgit/include include



### Apply patches
# gen: git diff --cached --binary include > patches/0001-sead-init.patch
echo
echo "patching..."
for patchfile in patches/*.patch; do
    echo "git apply $patchfile"
    git apply $patchfile
done
