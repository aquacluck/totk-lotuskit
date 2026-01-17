#pragma once

#include "aiController.h"

namespace ai {

class Context {
public:
    virtual ~Context();

    ControllerBase* getController() const {
        return mController;
    }
    
private:
    ControllerBase* mController;
};

} // namespace ai
