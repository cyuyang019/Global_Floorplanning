import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import math
import time
import argparse
import random

used_color = ["#FFE", "#FFF", "#F00", "#FCC", "#BBB"]

def set_rand_color(used_color):
    # Generate a random color with full 8-bit RGB values
    R, G, B = random.randint(0, 15), random.randint(0, 15), random.randint(0, 15)
    
    # Convert RGB to hexadecimal format
    color = f"#{R:01x}{G:01x}{B:01x}"
    
    # Ensure the color is not in the used colors and has a diverse intensity
    while color in used_color or (R + G + B) < 10 or (R + G + B) > 35:  # Adjust the sum constraint for diversity
        R, G, B = random.randint(0, 15), random.randint(0, 15), random.randint(0, 15)
        color = f"#{R:01x}{G:01x}{B:01x}"

    return color

def draw_block(ax, x, y, width, height, color):
    # color = "#BBB"
    ax.add_patch(
        patches.Rectangle(
            (x, y),
            width,
            height,
            fill=True,
            edgecolor="#000",
            facecolor=color,
            alpha=1.0  # 0.3 original
        )
    )

# Create the parser and add arguments
parser = argparse.ArgumentParser(description="This program can draw the layout of the circuit")
parser.add_argument("txt_name", help="The name of circuit spec file", type=str)
parser.add_argument("png_name", help="The name of circuit spec file", type=str)
parser.add_argument("-l", "--line", help="Show connections between blocks", action="store_true")

# Parse the command line arguments
args = parser.parse_args()

txt_name = args.txt_name
png_name = args.png_name
with_line = args.line
fread = open(txt_name, 'r')
f = fread.read().split("\n")


total_block_number = int(f[0].split(" ")[1])
total_connection_number = int(f[0].split(" ")[3])
window_width = int(f[1].split(" ")[0])
window_height = int(f[1].split(" ")[1])
aspect_ratio = window_height / window_width

h = 15*aspect_ratio
if int(h*100) % 2:
    h += 0.01
png_size = (16, h)
fig = plt.figure(figsize=png_size)

ax = fig.add_subplot(111)
ax.set_xbound(0, window_width)
ax.set_ybound(0, window_height)

i = 2

ax.add_patch(
    patches.Rectangle(
        (0, 0),
        window_width,
        window_height,
        fill=False,
        edgecolor="#000",
        facecolor="#FFF",
        alpha=1.0  # 0.3 original
    )
)

name2pos = {}

for block in range(total_block_number):
    ss = f[i].split(" ")
    if ss[1] == "SOFT":
        x, y, w, h = float(ss[3]), float(ss[4]), float(ss[5]), float(ss[6])
        draw_block(ax, x, y, w, h, color="#FCC")
        plt.text(x + 1, y + 1, ss[0])
        name2pos[ss[0]] = (x + w / 2, y + h / 2)
    else:
        x, y, w, h = float(ss[3]), float(ss[4]), float(ss[5]), float(ss[6])
        draw_block(ax, x, y, w, h, color="#BBB")
        x_offset, y_offset = 0, 0
        if x == 0:
            x_offset -= 20
        elif x == window_width:
            x_offset += 3
        if y == 0:
            y_offset -= 13
        elif y == window_height:
            y_offset += 3
        plt.text(x + x_offset, y + y_offset, ss[0])
        name2pos[ss[0]] = (x + w / 2, y + h / 2)
    i += 1

if with_line == True:
    is_hyper = False
    j = i
    max_value = 1
    min_value = 1e10
    for connection in range(total_connection_number):
        ss = f[j].split(" ")
        value = int(ss[-1])
        if value > max_value:
            max_value = value
        if value < min_value:
            min_value = value
        if len(ss) > 3:
            is_hyper = True
        j += 1

    for connection in range(total_connection_number):
        ss = f[i].split(" ")
        value = float(ss[-1])
        if min_value == max_value:
            width = 2
        else:
            width = (value - min_value) / (max_value - min_value) * 14 + 1
        
        if is_hyper:
            current_color = set_rand_color(used_color)
            for mod_id in range(1, len(ss)-1):
                x_values = [name2pos[ss[0]][0], name2pos[ss[mod_id]][0]]
                y_values = [name2pos[ss[0]][1], name2pos[ss[mod_id]][1]]
                plt.plot(x_values, y_values, color=current_color,
                    linestyle="-", linewidth=width, alpha=0.5)
        else:
            x_values = [name2pos[ss[0]][0], name2pos[ss[1]][0]]
            y_values = [name2pos[ss[0]][1], name2pos[ss[1]][1]]
            
            plt.plot(x_values, y_values, color="blue",
                    linestyle="-", linewidth=width, alpha=0.5)
        i += 1

# plt.savefig(str(sys.argv[1])[:-4]+".png")

plt.savefig(png_name)
