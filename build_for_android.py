#!/bin/env python

import os
import subprocess
import shutil

from StringIO import StringIO
from zipfile import ZipFile
from urllib import urlopen

CWD = os.getcwd()
OUTPUT_DIRECTORY = os.path.join(CWD, ".android")
ANDROID_MK_PATH = os.path.join(OUTPUT_DIRECTORY, "Android.mk")
ANDROID_APP_MK_PATH = os.path.join(OUTPUT_DIRECTORY, "Application.mk")
ANDROID_MANIFEST_PATH = os.path.join(OUTPUT_DIRECTORY, "AndroidManifest.xml")

MAKE_FILE_DATA = """
include $(call all-subdir-makefiles)
""".strip()

APPLICATION_FILE_DATA = """
APP_BUILD_SCRIPT		:= Android.mk
APP_PROJECT_PATH		:= {}
NDK_TOOLCHAIN_VERSION 	:= clang
APP_GNUSTL_CPP_FEATURES := rtti exceptions
APP_C_INCLUDES          := {}/pcre
APP_STL                 := gnustl_static
APP_PLATFORM            := android-19
""".format(OUTPUT_DIRECTORY, OUTPUT_DIRECTORY)

LIBRARIES = [
    {
        "name": "pcre",
        "repo": "https://github.com/Kazade/pcre.git",
        "include": "pcre"
    },
    {
        "name": "kazmath",
        "repo": "https://github.com/Kazade/kazmath.git",
        "include": "kazmath/kazmath"
    },
    {
        "name": "kaztimer",
        "repo": "https://github.com/Kazade/kaztimer.git",
        "include": "kaztimer/kaztimer"
    },
    {
        "name": "kazbase",
        "repo": "https://github.com/Kazade/kazbase.git",
        "include": "kazbase/kazbase"
    },
    {
        "name": "sdl",
        "zip": "https://www.libsdl.org/release/SDL2-2.0.3.zip",
        "include": "sdl/include"
    },
    {
        "name": "soil",
        "repo": "https://github.com/Kazade/soil.git",
        "include": "soil/include"
    }
]

if __name__ == "__main__":

    includes = []
    for library in LIBRARIES:
        print "Downloading %s" % library["name"]

        includes.append(library["include"])

        library_output_dir = os.path.join(OUTPUT_DIRECTORY, library["name"])
        if os.path.exists(library_output_dir):
            if "precompile" in library:
                library["precompile"]()
            continue

        if "repo" in library:
            subprocess.check_call([
                "git", "clone", library["repo"], library_output_dir
            ])
        elif "zip" in library:
            url = urlopen(library["zip"])
            zipfile = ZipFile(StringIO(url.read()))

            folder_name = zipfile.namelist()[0].rstrip("/")
            zipfile.extractall(path=os.path.dirname(library_output_dir))
            shutil.move(
                os.path.join(os.path.dirname(library_output_dir), folder_name),
                library_output_dir
            )
        else:
            raise ValueError("Unable to handle this library")

        if "precompile" in library:
            library["precompile"]()

    ##Make an Android.mk file
    with open(ANDROID_MK_PATH, "w") as make_file:
        make_file.write(MAKE_FILE_DATA.format(include_dirs=" ".join(includes)))

    with open(ANDROID_APP_MK_PATH, "w") as make_file:
        make_file.write(APPLICATION_FILE_DATA)

    with open(ANDROID_MANIFEST_PATH, "w") as make_file:
        make_file.write("\n")

    os.chdir(OUTPUT_DIRECTORY)

    subprocess.check_call([
        "ndk-build", "NDK_APPLICATION_MK=%s" % ANDROID_APP_MK_PATH
    ])