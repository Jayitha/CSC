#!/bin/bash 

echo -n "Are you sure you want to delete all stats (y/n): "
read answer

if [ "$answer" = "n" ]; then
    echo "Aborted."
    exit 1
fi

if [ "$answer" = "y" ]; then
    echo -n "Enter delete code (****) : "
    read passcode
    if [ "$passcode" != "1029384756" ]; then
        echo "Wrong code: Aborted."
        exit 1
    fi

    path="/Users/jayitha/Documents/CSAlgorithms"
    sqlite3 $path/data/comp_sky_stats.db "DELETE FROM CS_sizes"
    sqlite3 $path/data/comp_sky_stats.db "DELETE FROM competitive_set"
    sqlite3 $path/data/comp_sky_stats.db "DELETE FROM competitive_set_properties"
    sqlite3 $path/data/comp_sky_stats.db "DELETE FROM datasets"
    sqlite3 $path/data/comp_sky_stats.db "DELETE FROM time_stats"
    echo "Done! Deleted all stats!"
fi