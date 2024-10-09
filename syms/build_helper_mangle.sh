#!/bin/bash
#$ ./syms/build_helper_mangle.sh "namespace engine { class Gearbox { public: void spinny(int); }; } auto _ = &engine::Gearbox::spinny; "
# .quad   _ZN6engine7Gearbox6spinnyEi
echo $* | g++ -S -x c++ - -o- | grep .quad
