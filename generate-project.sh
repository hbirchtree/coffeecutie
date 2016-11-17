#!/bin/bash

COFFEE_DIR=$(dirname $(readlink -f "${0}"))
PROJECT_DIR="$(pwd)"
GIT_COMMIT=""
export PJNAME="CoffeeGame"

function fetch_version()
{
    pushd "${COFFEE_DIR}" 1>/dev/null
    GIT_COMMIT="$(git rev-parse HEAD)" || GIT_COMMIT="0000"
    popd 1>/dev/null

    echo "${GIT_COMMIT}" > "${PROJECT_DIR}/ENGINE_VERSION"
}

function create_base_directories()
{
    mkdir -p "${PROJECT_DIR}/cmake"
    mkdir -p "${PROJECT_DIR}/rsrc/${PJNAME}"
    mkdir -p "${PROJECT_DIR}/src"
    mkdir -p "${PROJECT_DIR}/internal"
}

function copy_config_files()
{
    cp -ur "${COFFEE_DIR}/cmake/Find" "${PROJECT_DIR}/cmake"
    cp -ur "${COFFEE_DIR}/cmake/Modules" "${PROJECT_DIR}/cmake"
    cp -ur "${COFFEE_DIR}/cmake/Packaging" "${PROJECT_DIR}/cmake"
    cp -ur "${COFFEE_DIR}/cmake/Preload" "${PROJECT_DIR}/cmake"
    cp -ur "${COFFEE_DIR}/cmake/Toolchains" "${PROJECT_DIR}/cmake"
    cp -ur "${COFFEE_DIR}/desktop" "${PROJECT_DIR}"
    cp -ur "${COFFEE_DIR}/internal/templates" "${PROJECT_DIR}/internal"
    touch "${PROJECT_DIR}/rsrc/${PJNAME}/RESOURCES"
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

    local MAIN_FILE="${PROJECT_DIR}/src/main.cpp"
    local CMAKE_FILE="${PROJECT_DIR}/CMakeLists.txt"

    if [[ ! -f "${CMAKE_FILE}" ]]; then
        cat "${COFFEE_DIR}/cmake/Templates/TemplateProject.txt" | \
        sed -e "s|@PJNAME@|${PJNAME}|g" | \
        sed -e "s|@SRCS@|${SRCS}|g" | \
        sed -e "s|@LIBS@|${LIBS}|g" | \
        sed -e "s|@COPYRIGHT@|${COPYRIGHT}|g" | \
        sed -e "s|@COMPANY@|${COMPANY}|g" | \
        sed -e "s|@DESC@|${DESC}|g" | \
        sed -e "s|@TITLE@|${TITLE}|g" \
        > "${CMAKE_FILE}"
    fi

    if [[ ! -f "${MAIN_FILE}" ]]; then
        cat "${COFFEE_DIR}/cmake/Templates/main.cpp" | \
            sed -e "s|@PJNAME@|${PJNAME}|g" \
            > "${MAIN_FILE}"
    fi

    touch "${PROJECT_DIR}/LICENSE"
}

#
# Creates an executable reconfiguration file
# 1: title
# 2: project name
#
function reconfig_project()
{
    local RECONFIG_FILE="${PROJECT_DIR}/reconfig.sh"
    echo "#!/bin/bash
${COFFEE_DIR}/generate-project.sh '${1}' '${PJNAME}'
" > "${RECONFIG_FILE}"
    chmod +x "${RECONFIG_FILE}"
}

#
# 1: Title
# 2: Project name
#
function main()
{
    fetch_version
    set_pjname "$2"
    create_base_directories
    configure_project "$1"

    copy_config_files

    reconfig_project "$1" "$2"
}

main "$1" "$2"
