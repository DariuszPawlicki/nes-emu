add_library(nes-emu-lib
        Cartridge.cpp
        CPU6502.cpp
        PPU.cpp
        MainBus.cpp
        mappers/Mapper.cpp
        NESConsole.cpp
        ui/NESConsoleMainWindow.cpp
)

target_include_directories(nes-emu-lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(nes-emu-lib PUBLIC imgui-lib)