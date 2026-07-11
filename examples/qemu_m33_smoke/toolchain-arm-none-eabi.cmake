# Freestanding Cortex-M33 toolchain for QEMU mps2-an505
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Default: compilers from PATH. Override with ARM_NONE_EABI_TOOLCHAIN_ROOT.
if(DEFINED ENV{ARM_NONE_EABI_TOOLCHAIN_ROOT})
  set(_root "$ENV{ARM_NONE_EABI_TOOLCHAIN_ROOT}")
  set(CMAKE_C_COMPILER "${_root}/bin/arm-none-eabi-gcc")
  set(CMAKE_ASM_COMPILER "${_root}/bin/arm-none-eabi-gcc")
  set(CMAKE_OBJCOPY "${_root}/bin/arm-none-eabi-objcopy")
  set(CMAKE_SIZE "${_root}/bin/arm-none-eabi-size")
else()
  set(CMAKE_C_COMPILER arm-none-eabi-gcc)
  set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
  set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
  set(CMAKE_SIZE arm-none-eabi-size)
endif()

set(CPU_FLAGS "-mcpu=cortex-m33 -mthumb -mfloat-abi=soft")
set(CMAKE_C_FLAGS_INIT "${CPU_FLAGS} -ffunction-sections -fdata-sections -ffreestanding -fno-builtin")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CPU_FLAGS} -Wl,--gc-sections -nostdlib -nostartfiles")
