#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# ========================================
# Original Tunepal Library (Bryan's code)
# ========================================
env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "TunepalGodot/addons/tunepal/bin/libtunepal.{}.{}.framework/libtunepal.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "TunepalGodot/addons/tunepal/bin/libtunepal{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)

# ========================================
# Experimental Library (pYIN + DTW)
# ========================================
# Build with: scons experimental
# This creates a SEPARATE library that can be loaded alongside the original

# Check if 'experimental' target is specified
build_experimental = 'experimental' in COMMAND_LINE_TARGETS or os.environ.get('BUILD_EXPERIMENTAL', '0') == '1'

if build_experimental:
    # Clone environment for experimental library
    env_exp = env.Clone()

    # Add experimental source paths
    env_exp.Append(CPPPATH=["src_experimental/", "src_experimental/algorithms/"])

    # Collect experimental sources
    exp_sources = Glob("src_experimental/*.cpp")

    if env_exp["platform"] == "macos":
        exp_library = env_exp.SharedLibrary(
            "TunepalGodot/addons/tunepal_experimental/bin/libtunepal_experimental.{}.{}.framework/libtunepal_experimental.{}.{}".format(
                env_exp["platform"], env_exp["target"], env_exp["platform"], env_exp["target"]
            ),
            source=exp_sources,
        )
    else:
        exp_library = env_exp.SharedLibrary(
            "TunepalGodot/addons/tunepal_experimental/bin/libtunepal_experimental{}{}".format(
                env_exp["suffix"], env_exp["SHLIBSUFFIX"]
            ),
            source=exp_sources,
        )

    # Create an alias for easy building
    Alias('experimental', exp_library)
