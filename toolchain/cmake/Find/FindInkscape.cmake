find_program ( INKSCAPE_PROGRAM inkscape
    HINTS
    ~/.local
    ~/
    /usr/local
    /usr

    PATH_SUFFIXES
    bin
    )

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Inkscape REQUIRED_VARS INKSCAPE_PROGRAM)
