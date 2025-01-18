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
rm -rf source



### Fetch sources
if [[ "$KEEP_TMPGIT" != "1" ]]; then
    echo
    echo "clone upstream..."
    rm -rf tmpgit

    # temp repo
    # last base: "Forgot a few things :3"
    #            https://github.com/shadowninja108/exlaunch/commit/4778861a288f2719ae5b7d867e435b271334b747
    git clone --recursive --depth 1 https://github.com/shadowninja108/exlaunch tmpgit
    echo
    echo
fi



### Copy just what we need
cp -r tmpgit/source source
rm -r source/program



### Apply patches
# gen: git diff --binary --cached source > patches/0001-exl-init.patch # staged changes only, afaik required for patching in new files
echo
echo "patching..."
for patchfile in patches/*.patch; do
    echo "git apply $patchfile"
    git apply $patchfile
done
