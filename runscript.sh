make

if [[ "$2" == "" && "$3" == "" ]]; then
    ./global_floorplan -i inputs/$1-input.txt -o outputs/$1-output.txt | tee log/$1.log
elif [[ "$3" == "" ]]; then
    ./global_floorplan -i inputs/$1-input.txt -o outputs/$1-output.txt -p $2| tee log/$1.log
else
    ./global_floorplan -i inputs/$1-input.txt -o outputs/$1-output.txt -p $2 -a $3| tee log/$1.log
fi

Case=$(echo "$1" | cut -c 1)

if [ $Case == "c" ]; then
    python3 utils/draw_rect_layout.py outputs/$1-output.txt img/$1.png -l
else
    python3 utils/draw_rect_layout.py outputs/$1-output.txt img/$1.png
fi

python3 utils/record_parameter.py $1

# open outputs/$1.png