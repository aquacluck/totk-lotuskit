from build_types import *
begin_sym_file(__file__)


"""
struct hknpMotion {};
namespace hkUNKNOWN { void UNKNOWN_applyMotion(void* self, void* param_2, hknpMotion* hkmotion, hknpMotion* motion_pool) {} }
"""

EXKING("_ZN9hkUNKNOWN19UNKNOWN_applyMotionEPvS0_P10hknpMotionS2_", {
    TOTK_100: 0x0022ed04,
    MagicWords.ALL: 0x0020d98c,
})
