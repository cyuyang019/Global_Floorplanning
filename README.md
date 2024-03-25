# Global Floorplanning

This tool is for global floorplanning stage in VLSI floorplanning. 

## Prerequisites

1. Make sure your `gcc/g++` supports C++17 (C++14 should work too).
2. Make sure `numpy` and `matplotlib` are installed.

## Installation

1. Clone the git repo:

   ```sh
   git clone https://github.com/cyuyang019/Global_Floorplanning.git
   ```

2. Compile:

   ```sh
   make
   ```

3. Create 4 directories:

`outputs` for storing output floorplan

`img` for storing floorplan's image

`log` for recording log file

`parameter_record` for recording different parameters' results



## Usage

#### Run global floorplanning

```shell
./global_floorplan -i <input_file> -o <output_file> -c <config_file> -p <punishment> -a <max_aspect_ratio>
```

where `<punishment>` and `<max_aspect_ratio>` and `<config_file>` are optional arguments, the default values are 0.05 and 2.

#### Visualization

```shell
python3 utils/draw_rect_layout.py <input_file> <output_image> <line/noline>
```

Use `line` to visualize the interconnections between modules and use `noline` to disable it.

#### Run global floorplanning with `<case>` as input

```shell
./runscript.sh <case> <punishment> <max_aspect_ratio>
```

where `<punishment>` and `<max_aspect_ratio>` are optional arguments. The output floorplan and its visualized image will be stored in `outputs`. The resulting HPWL and its corresponding parameters will be stored in `parameter_record`. The log file will be stored in `log`.

#### Run global floorplanning with configuration file

```shell
./runconfig.sh <case> <punishment> <max_aspect_ratio>
```

where `<punishment>` and `<max_aspect_ratio>` are optional arguments. It reads the configuration file stored in `config/<case>.txt`. The remaining functionalities are same as `runscript.sh`

#### Configuration file

There are 3 parameters that can be specified in configuration file:

1. Punishment. Usage:

   ```pseudocode
   punishment <punishment>
   ```

2. Maximum allowed aspect ratio. Usage:

   ``` pseudocode
   max_aspect_ratio <maximum_allowed_aspect_ratio>
   ```

3. Shape constraints. Usage:

   ``` pseudocode
   shape_constraint <num_of_constraint>
   <cons1_mod1> <cons1_mod2>
   <cons2_mod1> <cons2_mod2> <cons2_mod3> ...
   ...
   ```

Both configuration file and inline argument can specify punishment and maximum allowed aspect ratio. Inline argument is set to prevail configuration file.
