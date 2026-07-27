#!/bin/bash

set -e

DOLPHIN_DIR=$(realpath $(dirname $0)/../../../multi_build/runtime/dolphin)

DEFAULT_ISO_ARG=()
if [ -n "${DVD_DIR:-}" ]; then
    DEFAULT_ISO_ARG=(-C "Dolphin.Core.DefaultISO=${DVD_DIR}")
fi

${DOLPHIN_DIR}/bin/dolphin-emu \
    -u ${DOLPHIN_DIR}/config/User \
    -C Dolphin.Core.CPUThread=False \
    -C Dolphin.Core.GFXBackend="${DOLPHIN_RENDERER:-Vulkan}" \
    -C Dolphin.Core.MMU=True \
    -C Dolphin.Core.SlotA=8 \
    -C Dolphin.Core.SlotB=7 \
    -C Dolphin.Core.SerialPort1=0 \
    -C Logger.Logs.CORE=True \
    -C Logger.Logs.DVD=True \
    -C Logger.Logs.EXI=True \
    -C Logger.Logs.GDB_STUB=True \
    -C Logger.Logs.HLE=True \
    -C Logger.Logs.PowerPC=True \
    -C Logger.Logs.OSREPORT_HLE=True \
    -C Logger.Logs.OSREPORT=True \
    -C Logger.Logs.SI=True \
    -C Logger.Logs.SP1=True \
    -C Logger.Options.Verbosity=4 \
    -C Logger.Options.WriteToConsole=True \
    -C Logger.Options.WriteToWindow=True \
    "${DEFAULT_ISO_ARG[@]}" \
    "${@}"
echo "Return code: $?"
