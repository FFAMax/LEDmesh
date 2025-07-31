#!/bin/bash

# Path to control_hsv binary (adjust if necessary)
BIN="./control_hsv"
$BIN --hue 210 --sat 100 --brightness 0

# Time in minutes since midnight
now=$(date +%H)
now_min=$(date +%M)
time_now=$((10#$now * 60 + 10#$now_min))

# Schedule points
start1=$((5 * 60 + 35))   # 5:35
end1=$((5 * 60 + 45))     # 5:45
end2=$((5 * 60 + 55))     # 5:55

if (( time_now < start1 )); then
    exit 0
fi

if (( time_now >= start1 && time_now < end1 )); then
    # First phase: 5:35–5:45
    # Brightness: 0 → 70%
    progress=$(( (time_now - start1) * 100 / (end1 - start1) ))
    brightness=$(( progress * 70 / 100 ))
    $BIN --hue 210 --sat 100 --brightness $brightness
    exit 0
fi

if (( time_now >= end1 && time_now < end2 )); then
    # Second phase: 5:45–5:55
    progress=$(( (time_now - end1) * 100 / (end2 - end1) ))
    hue=$(( 210 - (progress * (210 - 60) / 100) ))
    sat=$(( 100 - (progress * 90 / 100) ))
    brightness=$(( 70 + (progress * 30 / 100) ))
    $BIN --hue $hue --sat $sat --brightness $brightness
    exit 0
fi

# After 5:55 — optionally turn off or keep white
# $BIN --hue 60 --sat 0 --brightness 100
exit 0
