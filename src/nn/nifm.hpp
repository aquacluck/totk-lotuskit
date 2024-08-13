#pragma once

#include "types.h"
#include "nn/result.hpp"

namespace nn { namespace nifm {

Result Initialize();
void SubmitNetworkRequest();
void SubmitNetworkRequestAndWait();
void CancelNetworkRequest();
bool IsNetworkRequestOnHold();
bool IsNetworkAvailable();

} }
