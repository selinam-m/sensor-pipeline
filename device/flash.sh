#!/usr/bin/env bash
set -e

FQBN="arduino:mbed_nano:nano33ble"
SKETCH="phase1_imu"
PORT="${1:-/dev/ttyACM0}"

echo ">> Compiling $SKETCH ..."
arduino-cli compile --warnings all --fqbn "$FQBN" "$SKETCH"

echo ">> Uploading to $PORT ..."
arduino-cli upload -p "$PORT" --fqbn "$FQBN" "$SKETCH"

echo ">> Done."