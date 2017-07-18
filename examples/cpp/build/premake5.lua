-- Copyright © 2017 Embedded Artistry LLC.
-- License: MIT. See LICENSE file for details.

-- For reference, please refer to the premake wiki:
-- https://github.com/premake/premake-core/wiki

local ROOT = "../"
local RESULTSROOT = "../buildresults/%{cfg.platform}_%{cfg.buildcfg}/"

---------------------------------
-- [ WORKSPACE CONFIGURATION   --
---------------------------------
workspace "embedded-resources C++ Examples"
  configurations { "debug", "release" }
  platforms      { "x86_64", "x86" }

  -- _ACTION is the argument you passed into premake5 when you ran it.
  local project_action = "UNDEFINED"
  if _ACTION ~= nill then project_action = _ACTION end

  -- Where the project/make files are output
  location(ROOT .. "build/gen")

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
    language "C++"
    targetdir (RESULTSROOT)
    targetname "circular_buffer.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "circular_buffer.cpp"
    }

    filter {} -- clear filter!

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

  project "dispatch"
    kind "ConsoleApp"
    language "C++"
    targetdir (RESULTSROOT)
    targetname "dispatch.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "dispatch.cpp"
    }

    filter {} -- clear filter!

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

  project "shared_ptr"
    kind "ConsoleApp"
    language "C++"
    targetdir (RESULTSROOT)
    targetname "shared_ptr.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "shared_ptr.cpp"
    }

    filter {} -- clear filter!

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

  project "aligned_ptr"
    kind "ConsoleApp"
    language "C++"
    targetdir (RESULTSROOT)
    targetname "aligned_ptr.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "smart_ptr_aligned.cpp",
      SourceDir .. "../libc/malloc_aligned.c",
    }

    filter {} -- clear filter!

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

  project "array"
    kind "ConsoleApp"
    language "C++"
    targetdir (RESULTSROOT)
    targetname "array.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "array.cpp"
    }

    filter {} -- clear filter!

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

  project "vector"
    kind "ConsoleApp"
    language "C++"
    targetdir (RESULTSROOT)
    targetname "vector.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "vector.cpp"
    }

    filter {} -- clear filter!

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

  project "string"
    kind "ConsoleApp"
    language "C++"
    targetdir (RESULTSROOT)
    targetname "string.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "string.cpp"
    }

    filter {} -- clear filter!

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

  project "callbacks"
    kind "ConsoleApp"
    language "C++"
    targetdir (RESULTSROOT)
    targetname "callbacks.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "callbacks.cpp"
    }

    filter {} -- clear filter!

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

  project "bounce"
    kind "ConsoleApp"
    language "C++"
    targetdir (RESULTSROOT)
    targetname "bounce.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "bounce.cpp"
    }

    filter {} -- clear filter!

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

  project "c_sbrm"
    kind "ConsoleApp"
    language "C++"
    targetdir (RESULTSROOT)
    targetname "c_sbrm.bin"

    local SourceDir = ROOT;
    files
    {
      SourceDir .. "c_sbrm.cpp"
    }

    filter {} -- clear filter!

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
