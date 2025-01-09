#pragma once

#include <common.hpp>
#include <tuple>

#include "version.hpp"

namespace exl::reloc {
    using VersionType = util::UserVersion;

    template<VersionType Version, impl::LookupEntry... Entries>
    using UserTableType = VersionedTable<Version, Entries...>;

    using UserTableSet = TableSet<VersionType,
        UserTableType<VersionType::DEFAULT,

// i want to keep the ability to trivially ifdef stuff across versions anyways, so i don't see much point in unifying mod builds.
// the frontend can afford to be more unified, reloading its data assets when a different game version appears on the socket, etc.
// all the hash+lookup plumbing is great and i wanna use it, i just don't need the versioning part. This file is version specific.

#include "syms_exl_reloc.inc"

        >
    >;
}
