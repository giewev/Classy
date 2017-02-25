#!/bin/bash

output_file=bench.txt
> $output_file
for depth in 4, 5, 6
do
	(time ./bin/Debug/Classy -fen 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1' -depth $depth) &>> $output_file
	echo '----------------------------' >> $output_file
done