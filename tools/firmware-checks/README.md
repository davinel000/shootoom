# Uno checks

`uno_0_1_0.cpp` includes the actual pure production decoder and motion arithmetic. No Arduino or hardware test doubles. Build with a C++14 host compiler with assertions enabled (do not define NDEBUG), then pass the preserved Cuneiform.h path as the single argument. Run from the repository root:

```text
cl /EHsc /std:c++14 /W4 /O2 /Fe:build/uno-check.exe /Fo:build/uno-check.obj tools/firmware-checks/uno_0_1_0.cpp
build/uno-check.exe firmware/legacy/rp2040-candidate/HammurapireadsEncoded_fulltexts/Cuneiform.h
```

Create build/ first and use a Visual Studio developer shell for cl. MSVC 2022 BuildTools was used locally. These checks validate packet recovery, payload bounds and arithmetic, not AVR timing, physical output, button electronics or software serial interrupt interactions. AVR compilation is a separate required check, documented with the firmware version.
