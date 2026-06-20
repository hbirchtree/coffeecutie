#!/bin/bash

set -e

DOLPHIN_DIR=$(realpath $(dirname $0)/../../../multi_build/runtime/dolphin)

TIMEOUT=${TIMEOUT:-30}
DUMP_FILE=${DUMP_FILE:-dump_$RANDOM.mp4}
echo "::info::Virtual GameCube will run for ${TIMEOUT} seconds"
echo "::info::Recording will be saved to \"${DUMP_FILE}\""

echo "::group::Dolphin run"
# Enable dumping A/V
# Enable USB Gecko,
# Enable writing to console
# Enable bunch of log categories
${DOLPHIN_DIR}/bin/dolphin-emu-nogui \
    -p headless \
    -u ${DOLPHIN_DIR}/config/User \
    -C Dolphin.Movie.DumpFrames=True \
    -C Dolphin.DSP.DumpAudio=True \
    -C Dolphin.Core.SlotB=7 \
    -C GFX.Settings.DumpCodec=utvideo \
    -C Logger.Options.WriteToConsole=True \
    -C Logger.Options.WriteToFile=True \
    -C Logger.Logs.Core=True \
    -C Logger.Logs.EXI=True \
    -C Logger.Logs.PowerPC=True \
    -C Logger.Logs.OSREPORT=True \
    -C Logger.Logs.OSREPORT_HLE=True \
    "${@}" &

sleep ${TIMEOUT:-30}

kill %1
echo "::endgroup::"

AUDIO_DUMP=$(find ${DOLPHIN_DIR}/config/User/Dump/Audio -name *_dspdump.wav)
VIDEO_DUMP=$(find ${DOLPHIN_DIR}/config/User/Dump/Frames -name *.avi)
echo "::group::Encode dumped frames to file"
ffmpeg \
    -i ${VIDEO_DUMP} \
    -i ${AUDIO_DUMP} \
    -c:v libx264 -b:v 4M \
    -c:a aac -b:a 128k \
    ${DUMP_FILE}
rm -r ${DOLPHIN_DIR}/config/User/Dump/{Audio,Frames}
echo "::endgroup::"
