make

if [ "$2" == "" ]; then
    ./global_floorplan -i inputs/$1-input.txt -o outputs/$1-output.txt | tee log/$1.log
else
    ./global_floorplan -i inputs/$1-input.txt -o outputs/$1-output.txt -p $2| tee log/$1.log
fi

python3 utils/draw_rect_layout.py outputs/$1-output.txt outputs/global_$1.png
open outputs/global_$1.png