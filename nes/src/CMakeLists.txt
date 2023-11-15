add_library(nes-emu-lib
        Byte.cpp
        Cartridge.cpp
        CPU6502.cpp
        PPU.cpp
        MainBus.cpp
        Mapper.cpp
        NESConsole.cpp
        UI/NESConsoleMainWindow.cpp
)

target_include_directories(nes-emu-lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(nes-emu-lib PUBLIC imgui-lib)