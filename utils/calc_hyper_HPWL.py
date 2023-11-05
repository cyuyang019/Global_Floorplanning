import sys


pos_name = sys.argv[1]
net_name = sys.argv[2]
fread = open(pos_name, 'r')
fpos = fread.read().split("\n")
fread = open(net_name, 'r')
fnet = fread.read().split("\n")



HPWL = 0

block_num = int(fpos[0].split(" ")[1])

modname2pos = {}

for i in range(2, 2 + block_num):
    ss = fpos[i].split(" ")
    modname = ss[0]
    x, y = float(ss[2]), float(ss[3])
    w, h = float(ss[4]), float(ss[5])
    modname2pos[modname] = (x + w/2, y + h/2)


for line in fnet:
    ss = line.split(" ")
    if len(ss) < 1:
        fnet.remove(line)
    elif ss[0] == "#":
        fnet.remove(line)

i = 0
while i < len(fnet):
    ss = fnet[i].split(" ")

    if ss[0] == "NetDegree":
        pin_count = int(ss[2])
        pin_list = []
        i += 1
        while pin_count > 0:
            ss = fnet[i].split(" ")
            pin_list.append(ss[0])
            pin_count -= 1
            i += 1
        x_min, x_max = modname2pos[pin_list[0]][0], modname2pos[pin_list[0]][0]
        y_min, y_max = modname2pos[pin_list[0]][1], modname2pos[pin_list[0]][1]
        for name in pin_list:
            x_min = min(x_min, modname2pos[name][0])
            x_max = max(x_max, modname2pos[name][0])
            y_min = min(y_min, modname2pos[name][1])
            y_max = max(y_max, modname2pos[name][1])
        HPWL += x_max - x_min + y_max - y_min
    else:
        i += 1

print("HPWL: ", HPWL)