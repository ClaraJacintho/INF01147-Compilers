import os
import subprocess
import sys

source_dir = "E5/"
dest_dir = "E5_res/"
for filename in os.listdir(source_dir):
    res_s = filename + ".s"
    program = "./etapa6 < " + source_dir+filename + " > " + dest_dir + res_s
    print(program)
    subprocess.run(program)
    # result = subprocess.run(
    #     [sys.executable, "-c", ], capture_output=True, text=True
    # )
    # print("stdout:", result.stdout)
    # print("stderr:", result.stderr)