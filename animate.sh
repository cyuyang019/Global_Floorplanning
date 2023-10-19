for i in {1..51}; do
    python utils/draw_rect_layout.py outputs/$1-output.txt.$i animation/global.png
    open animation/global.png
done