min=$2
interval=$3
max=$4
ar=$5
for ((i = 0; i <= $3; i++)); do
    punishment=$(python -c "print($min + ($max - $min) * $i / $3)")
    ./global_floorplan -i inputs/$1-input.txt -o outputs/$1-output.txt -p $punishment -a $ar | tee log/$1.log
    python3 utils/record_parameter.py $1
done