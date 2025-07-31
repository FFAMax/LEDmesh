#!/bin/bash

# Path to control_hsv binary (adjust if needed)
BIN="./control_hsv"

# Emulated time range: from 5:35 to 5:55 (in minutes since midnight)
start_min=$((5 * 60 + 35))  # 335
end_min=$((5 * 60 + 55))    # 355

# Total steps (simulate 1 min = 1 sec)
for (( emu_min = start_min; emu_min <= end_min; emu_min++ )); do
    # Simulate the logic from sunrise.sh
    if (( emu_min < start_min )); then
        continue
    fi

    if (( emu_min >= start_min && emu_min < start_min + 10 )); then
        # First phase: 5:35–5:45
        progress=$(( (emu_min - start_min) * 100 / 10 ))
        brightness=$(( progress * 70 / 100 ))
        $BIN --hue 210 --sat 100 --brightness $brightness
        echo "[Demo] $(printf '%02d:%02d' $((emu_min / 60)) $((emu_min % 60))) — hue=210 sat=100 bright=$brightness"
    elif (( emu_min >= start_min + 10 && emu_min <= end_min )); then
        # Second phase: 5:45–5:55
        progress=$(( (emu_min - (start_min + 10)) * 100 / 10 ))
        hue=$(( 210 - (progress * (210 - 60) / 100) ))
        sat=$(( 100 - (progress * 90 / 100) ))
        brightness=$(( 70 + (progress * 30 / 100) ))
        $BIN --hue $hue --sat $sat --brightness $brightness
        echo "[Demo] $(printf '%02d:%02d' $((emu_min / 60)) $((emu_min % 60))) — hue=$hue sat=$sat bright=$brightness"
    fi

    sleep 1
done
