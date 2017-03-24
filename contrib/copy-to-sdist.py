#!/usr/bin/python3
"""This script creates an unofficial sdist directory.
   It is useful when cmake can not be used (eg. on Windows)."""

from os import makedirs, walk, chdir
from os.path import dirname, join
from shutil import copy2
from sys import argv

def copyfile(src, dst ,reverse):
    if reverse:
        src, dst = dst, src
    makedirs(dirname(dst), exist_ok=True)
    copy2(src, dst)

def copydir(src, dst, fileend, reverse):
    for dirpath, dirnames, filenames in walk(src):
        for afile in filenames:
            if afile.endswith(fileend):
                fsrc = join(dirpath, afile)
                fdst = join(dirpath, afile).replace(src, dst)
                copyfile(fsrc, fdst ,reverse)

def main():
    chdir(dirname(__file__))
    reverse = "-r" in argv

    copydir("../cpp/cpp_odbc/Library/cpp_odbc", "sdist/include/cpp_odbc", ".h", reverse)
    copydir("../cpp/turbodbc/Library/turbodbc", "sdist/include/turbodbc", ".h", reverse)
    copydir("../cpp/turbodbc_numpy/Library/turbodbc_numpy", "sdist/include/turbodbc_numpy", ".h", reverse)
    copydir("../cpp/turbodbc_python/Library/turbodbc_python", "sdist/include/turbodbc_python", ".h", reverse)

    copydir("../cpp/cpp_odbc/Library/src", "sdist/src/cpp_odbc", ".cpp", reverse)
    copydir("../cpp/turbodbc/Library/src", "sdist/src/turbodbc", ".cpp", reverse)
    copydir("../cpp/turbodbc_numpy/Library/src", "sdist/src/turbodbc_numpy", ".cpp", reverse)
    copydir("../cpp/turbodbc_python/Library/src", "sdist/src/turbodbc_python", ".cpp", reverse)

    copydir("../python/turbodbc", "sdist/turbodbc", ".py", reverse)

    for afile in ["setup.cfg",
                  "setup.py",
                  "README.md",
                  "CHANGELOG.md",
                  "MANIFEST.in",
                  "LICENSE",
                  ]:
        fsrc = join("..", afile)
        fdst = join("sdist", afile)
        copyfile(fsrc, fdst ,reverse)

if __name__ == "__main__":
    main()
