workspace 'immersive'
    configurations { 'Debug', 'Release' }
    platforms { 'Win32', 'Win64' }

    symbols "On"
    characterset "Unicode"

    includedirs { 'common/include/' }

    location 'build/'

    defines { 'PUGIXML_WCHAR_MODE' }

    filter 'platforms:Win32'
        architecture 'x32'

    filter 'platforms:Win64'
        architecture 'x64'

    filter 'system:windows'
        includedirs { 'dependencies/include/wtl/' }
        defines { 'RW_D3D9' }

    filter 'configurations:Debug'
        defines { '_DEBUG' }
        targetdir 'bin/debug/%{cfg.architecture}/'

    filter 'configurations:Release'
        optimize 'On'
        defines { 'NDEBUG' }
        targetdir 'bin/release/%{cfg.architecture}/'

project 'immersivehost'
    targetname 'ImmersiveHost'
    language 'C++'
    kind 'ConsoleApp'
    windowstarget '10.0.14393.0'
    linkoptions '/stack:5000000,131072' -- for software such as Microsoft.SpartaUWP

    includedirs
    {
        'host/include',
        'vendor/minhook/include',
        'vendor/udis86',
        'vendor/pugixml/src/'
    }

    files
    {
        'host/**.h',
        'host/**.cpp',
        'host/**.c',
        'host/**.idl'
    }

    links { 'minhook', 'udis86', 'pugixml' }

    pchsource 'host/src/StdInc.cpp'
    pchheader 'StdInc.h'

project 'tlsdll'
    targetname 'tlsdll'
    language 'C++'
    kind 'SharedLib'

    files
    {
        'tlsdll/**.h',
        'tlsdll/**.cpp'
    }

project 'elevationservice'
    targetname 'ImmersiveElevationService'
    language 'C++'
    kind 'ConsoleApp'

    includedirs
    {
        'vendor/minhook/include'
    }

    links { 'minhook' }

    files
    {
        'service/**.h',
        'service/**.cpp',
        'service/**.c',
        'common/**.idl'
    }

group 'vendor'
project 'udis86'
    language 'C'
    kind 'StaticLib'

    includedirs
    {
        'vendor/udis86/'
    }

    files {
        "vendor/udis86/libudis86/*.c",
        "vendor/udis86/libudis86/*.h",
    }

project 'pugixml'
    language 'C++'
    kind 'StaticLib'

    includedirs
    {
        'vendor/pugixml/src/'
    }

    files
    {
        'vendor/pugixml/src/**.cpp'
    }

project 'minhook'
    language 'C'
    kind 'StaticLib'

    includedirs
    {
        'vendor/minhook/include'
    }

    files
    {
        'vendor/minhook/src/*.c'
    }

    filter 'platforms:Win32'
        files 'vendor/minhook/src/HDE/hde32.c'

    filter 'platforms:Win64'
        files 'vendor/minhook/src/HDE/hde64.c'
