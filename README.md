# Global Floorplanning

Run global floorplanning (`<punishment>` is an optional argument, the default value is 0.05):

```sh
./global_floorplan -i <input_file> -o <output_file> -p <punishment>
```



Run global floorplanning with `<case>` as input (a directory `outputs` is needed, `<punishment>` is an optional argument):

```sh
./runscript.sh <case> <punishment>
```



Draw the result of `<input_file>` to `<output_image>`:

``` sh
python3 utils/draw_rect_layout.py <input_file> <output_image> <line/noline>
```

