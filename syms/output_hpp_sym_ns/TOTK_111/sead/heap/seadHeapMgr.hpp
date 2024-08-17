#pragma once

namespace sym {


    static const u64 square = 0x00000069; // _Z6squarei


namespace one { namespace two { namespace four { namespace ClsName { 
    static const u64 staticmemberfunc = 0x00000069; // _ZN3one3two4four7ClsName16staticmemberfuncEv
    static const u64 memberfunc = 0x00000069; // _ZN3one3two4four7ClsName10memberfuncEv
} } } }

namespace one { namespace two { namespace three { 
    static const u64 argsfunc = 0x00000069; // _ZN3one3two5three8argsfuncEPiS2_
    static const u64 staticfunc = 0x00000069; // _ZN3one3two5threeL10staticfuncEv
    static const u64 normalfunc = 0x00000069; // _ZN3one3two5three10normalfuncEv
} } }

} // sym
