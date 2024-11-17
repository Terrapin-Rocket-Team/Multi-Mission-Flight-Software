#!/usr/bin/python
import sys
path = sys.argv[1]
numRows = int(sys.argv[2])
f = open(path, "w")

columns = []
col = ""

while True:
    try:
        col = input("add column>")
        if col == "exit":
            break
        else:
            columns.append(col)
    except EOFError:
       break

for col in columns:
    f.write(f"{col},")
f.write("\n")
for i in range(numRows):
    for (j, col) in enumerate(columns):
        f.write(f"{float(i+(j+1))},")
    f.write("\n")

f.close()