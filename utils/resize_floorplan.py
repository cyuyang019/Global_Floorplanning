import sys

case_name = sys.argv[1]
deadspace_ratio = float(sys.argv[2])
fpn_path = "./inputs/" + sys.argv[1] + "-input.txt"
resized_path = "./inputs/" + sys.argv[1] + "_%d%%"%(int(deadspace_ratio*100)) + "-input.txt"

fpn_read = open(fpn_path, "r")
ffpn = fpn_read.read().split("\n")
fwrite = open(resized_path, "w")


die_width = int(ffpn[0].split(" ")[1])
die_height = int(ffpn[0].split(" ")[2])
soft_mod_area = 0
soft_mod_num = int(ffpn[1].split(" ")[1])

l = 2

for mod in range(soft_mod_num):
    area = float(ffpn[l].split(" ")[1])
    soft_mod_area += area
    l += 1

scaled_area = soft_mod_area / (1 - deadspace_ratio)
scaled_width = int(scaled_area**0.5) + 1
scaled_height = int(scaled_area**0.5) + 1
width_scale_ratio = scaled_width / die_width
height_scale_ratio = scaled_height / die_height

fwrite.write("CHIP %d %d\n"%(scaled_width, scaled_height))
l = 1
for mod in range(soft_mod_num + 1):
    fwrite.write(ffpn[l] + "\n")
    l += 1

fixed_mod_num = int(ffpn[l].split(" ")[1])
fwrite.write("FIXEDMODULE %d\n"%(fixed_mod_num))
l += 1
for mod in range(fixed_mod_num):
    name, x, y, w, h = ffpn[l].split(" ")
    scaled_x = float(x) * width_scale_ratio
    scaled_y = float(y) * height_scale_ratio
    fwrite.write("%s %d %d %s %s\n"%(name, int(scaled_x), int(scaled_y), w, h))
    l += 1

for mod in range(len(ffpn) - l):
    fwrite.write(ffpn[l] + "\n")
    l += 1


fpn_read.close()
fwrite.close()
