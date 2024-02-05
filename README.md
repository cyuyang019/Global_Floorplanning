# Global Floorplanning

Run global floorplanning (`<punishment>` and `<max_aspect_ratio>` are optional arguments, the default value are 0.05 and 2):

```sh
./global_floorplan -i <input_file> -o <output_file> -p <punishment> -a <max_aspect_ratio>
```



Run global floorplanning with `<case>` as input (a directory `outputs` is needed, `<punishment>` and `<max_aspect_ratio>` are optional arguments):

```sh
./runscript.sh <case> <punishment>
```



Draw the result of `<input_file>` to `<output_image>`:

``` sh
python3 utils/draw_rect_layout.py <input_file> <output_image> <line/noline>
```

