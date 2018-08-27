#!/bin/bash

function die()
{
    echo " * " $@
    exit 1
}

function notify()
{
    echo " * " $@
}

function debug()
{
    echo $@ > /dev/stderr
}

function requires()
{
    for prog in $@; do
        local mute=$(which ${prog})
        if [[ ! "$?" = "0" ]]; then
            die "Could not find program: $prog"
        fi
    done
}
