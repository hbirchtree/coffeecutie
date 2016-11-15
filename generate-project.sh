#!/bin/bash

COFFEE_DIR=$(dirname $(readlink -f "${0}"))
PROJECT_DIR="$(pwd)"
export PJNAME="CoffeeGame"

function create_base_directories()
{
    mkdir -p "${PROJECT_DIR}/rsrc/${PJNAME}"
    mkdir -p "${PROJECT_DIR}/src"
}

function copy_config_files()
{
    cp -ur "${COFFEE_DIR}/cmake" "${PROJECT_DIR}"
    cp -ur "${COFFEE_DIR}/desktop" "${PROJECT_DIR}"
    cp -ur "${COFFEE_DIR}/internal" "${PROJECT_DIR}"
}

#
# Removes unwanted signs from PJNAME, could be a lot?
#
function filter_pjname()
{
    echo "-- Before filter: ${PJNAME}"
    PJNAME=`echo ${PJNAME} | sed -e 's/[&^|\/\  -+%#"!~*?=(){}\\<>.,;:]//g'`
    PJNAME=`echo ${PJNAME} | sed -e "s/[']//g"`
    PJNAME=`echo ${PJNAME} | sed -e "s/[0-9]//g"`
    echo "-- After filter: ${PJNAME}"
}

#
# 1: Project name
#
function set_pjname()
{
    PJNAME="$1"
    if [[ -z "$1" ]]; then
        PJNAME=`basename "$(pwd)"`
    fi

    filter_pjname
}

#
# 1: project title, dirname if not found
# 2: project name, used internally for targets and etc.
#
function configure_project()
{

    local TITLE="$1"
    local COMPANY="Coffee"
    local DESC="Coffee Application"
    local COPYRIGHT="Coffee"
    local SRCS="src/main.cpp"
    local LIBS="\${COFFEE_CORE_LIBRARY};\${COFFEE_GLEAM_LIBRARY};\${COFFEE_OPENAL_LIBRARY}"

    if [[ -z "$1" ]]; then
        local TITLE=`basename "$(pwd)"`
    fi

    echo "-- Generating project with name '${PJNAME}'"
    echo "-- Properties: "
    local

    cat "${COFFEE_DIR}/cmake/Templates/TemplateProject.txt" | \
        sed -e "s|@PJNAME@|${PJNAME}|g" | \
        sed -e "s|@SRCS@|${SRCS}|g" | \
        sed -e "s|@LIBS@|${LIBS}|g" | \
        sed -e "s|@COPYRIGHT@|${COPYRIGHT}|g" | \
        sed -e "s|@COMPANY@|${COMPANY}|g" | \
        sed -e "s|@DESC@|${DESC}|g" | \
        sed -e "s|@TITLE@|${TITLE}|g" \
        > "${PROJECT_DIR}/CMakeLists.txt"

    cat "${COFFEE_DIR}/cmake/Templates/main.cpp" | \
        sed -e "s|@PJNAME@|${PJNAME}|g" \
        > "${PROJECT_DIR}/src/main.cpp"

    touch "${PROJECT_DIR}/LICENSE"
}

#
# 1: Title
# 2: Project name
#
function main()
{
    set_pjname "$2"
    create_base_directories
    configure_project "$1"

    copy_config_files
}

main "$1" "$2"
