add_library(nes-emu-lib
            Byte.cpp
            Cartridge.cpp
            CPU6502.cpp
            CpuBus.cpp
            Mapper.cpp
            NESConsole.cpp
            PPU.cpp
            UserInterface.cpp
        )

target_include_directories(nes-emu-lib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(nes-emu-lib PUBLIC imgui-lib)