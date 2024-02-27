import sys
import csv
import os

case_name = sys.argv[1]
log_path = "./log/" + case_name + ".log"
csv_path = "./parameter_record/" + case_name + ".csv"

if not os.path.exists(csv_path):
    with open(csv_path, 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["punishment", "aspect_ratio", "overlap_ratio", "HPWL"])

fread = open(log_path, 'r')
f = fread.read().split("\n")

punishment_line = f[2]
punishment_list = punishment_line.split(" ")
try:
    punishment = "'" + punishment_list[-1] + "'"
except ValueError:
    punishment = "'0.05'"


aspect_ratio_line = f[3]
aspect_ratio_list = aspect_ratio_line.split(" ")
aspect_ratio = float(aspect_ratio_list[-1])

for line in f:
    if "Overlap Ratio" in line:
        overlap_ratio = line.split()[-2]
    if "Estimated HPWL" in line:
        HPWL = float(line.split()[-1])

with open(csv_path, 'a', newline='') as file:
    writer = csv.writer(file)
    writer.writerow([punishment, aspect_ratio, overlap_ratio, HPWL])