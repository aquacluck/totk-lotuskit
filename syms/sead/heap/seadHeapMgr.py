from build_types import *
begin_sym_file(__file__)

# namespace stuff
testsyms = ["_Z6squarei", "_ZN3one3two5threeL10staticfuncEv", "_ZN3one3two5three10normalfuncEv", "_ZN3one3two5three8argsfuncEPiS2_", "_ZN3one3two4four7ClsName16staticmemberfuncEv", "_ZN3one3two4four7ClsName10memberfuncEv"]
for mangled in testsyms:
    EXKING(mangled, { MagicWords.ALL: 0x69 })

