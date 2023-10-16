set -v
make
./global_floorplan inputs/$1-input.txt outputs/$1-output.txt | tee log/$1.log
python3 utils/draw_rect_layout.py outputs/$1-output.txt outputs/global_$1.png