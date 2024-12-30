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
    # last base: "Reorganize relocation table impl to be more flexible, if people want to implement their own mechanisms built on it"
    #            https://github.com/shadowninja108/exlaunch/commit/fb102d67b8da42f991a5cb02b960054ae4beaf14
    git clone --recursive --depth 1 https://github.com/shadowninja108/exlaunch tmpgit
    echo
    echo
fi



### Copy just what we need
cp -r tmpgit/source source
rm -r source/program



### Apply patches
# gen: git diff --binary --cached include source > patches/0001-exl-init.patch # staged changes only, afaik required for patching in new files
echo
echo "patching..."
for patchfile in patches/*.patch; do
    echo "git apply $patchfile"
    git apply $patchfile
done
