#pragma once

#include "heap/seadDisposer.h"

namespace sead
{
class ProcessMeterBarBase;

// FIXME
class ProcessMeter
{
    ProcessMeter() = default;

public:
    void attachProcessMeterBar(ProcessMeterBarBase* bar);
    void detachProcessMeterBar(ProcessMeterBarBase* bar);

private:
    SEAD_SINGLETON_DISPOSER(ProcessMeter)
};
}  // namespace sead
