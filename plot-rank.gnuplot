set terminal pngcairo

set title "AOC 2020 ranking"
set key right bottom
set grid
set logscale y
set yrange [1.30e5:1000] reverse

plot \
    "rank.txt" using 1:3 title "Part 1" with lines, \
    "rank.txt" using 1:6 title "Part 2" with lines
