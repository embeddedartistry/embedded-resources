-- Copyright © 2017 Embedded Artistry LLC.
-- License: MIT. See LICENSE file for details.

-- For reference, please refer to the premake wiki:
-- https://github.com/premake/premake-core/wiki

local ROOT = "./"
local RESULTSROOT = ROOT .. "buildresults/%{cfg.platform}_%{cfg.buildcfg}/"

---------------------------------
-- [ WORKSPACE CONFIGURATION   --
---------------------------------
workspace "embedded-resources C Examples"
  configurations { "debug", "release" }
  platforms      { "x86_64", "x86" }

  -- _ACTION is the argument you passed into premake5 when you ran it.
  local project_action = "UNDEFINED"
  if _ACTION ~= nill then project_action = _ACTION end

  -- Where the project/make files are output
  location(ROOT .. "buildresults/build")

  -----------------------------------
  -- Global Compiler/Linker Config --
  -----------------------------------
  filter "configurations:Debug"    defines { "DEBUG" }  symbols  "On"
  filter "configurations:Release"  defines { "NDEBUG" } optimize "On"

  filter { "platforms:x86" } architecture "x86"
  filter { "platforms:x86_64" } architecture "x86_64"

  -- Global settings for building makefiles
  filter { "action:gmake" }
    flags { "C++11" }

  -- Global settings for building make files on mac specifically
  filter { "system:macosx", "action:gmake"}
    toolset "clang"

  filter {} -- clear filter when you know you no longer need it!

  -------------------------------
  -- [ PROJECT CONFIGURATION ] --
  -------------------------------

  project "circular_buffer"
    kind "ConsoleApp"
    language "C"
    targetdir (RESULTSROOT)
    targetname "circular_buffer.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "circular_buffer.c",
    }

    filter {} -- clear filter!

    buildoptions {"-DCOMPILE_AS_EXAMPLE"}

    includedirs
    {
      SourceDir,
    }

    -- Library Dependencies
    libdirs
    {

    }

    links
    {

    }

  project "malloc_aligned"
    kind "ConsoleApp"
    language "C"
    targetdir (RESULTSROOT)
    targetname "malloc_aligned.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "malloc_aligned.c",
    }

    filter {} -- clear filter!

    buildoptions {"-DCOMPILE_AS_EXAMPLE"}

    includedirs
    {
      SourceDir,
    }

    -- Library Dependencies
    libdirs
    {

    }

    links
    {

    }

  project "malloc_freelist"
    kind "ConsoleApp"
    language "C"
    targetdir (RESULTSROOT)
    targetname "malloc_freelist.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "malloc_freelist.c",
      SourceDir .. "malloc_test.c",
    }

    filter {} -- clear filter!

    buildoptions {"-DCOMPILE_AS_EXAMPLE"}

    includedirs
    {
      SourceDir,
    }

    -- Library Dependencies
    libdirs
    {

    }

    links
    {

    }

  project "malloc_threadx"
    kind "StaticLib"
    language "C"
    targetdir (RESULTSROOT)
    targetname "malloc_threadx"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "malloc_threadx.c"
    }

    buildoptions {"-fno-builtin"}

    filter {} -- clear filter!

    includedirs
    {
      SourceDir,
      SourceDir .. "rtos/",
    }

    -- Library Dependencies
    libdirs
    {

    }

    links
    {

    }
