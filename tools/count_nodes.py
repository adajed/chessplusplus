import sys
import subprocess
import time

if __name__ == "__main__":

    fw = open("tmpout", "w")
    fr = open("tmpout", "r")
    p = subprocess.Popen(["./build/chessplusplus"], stdin=subprocess.PIPE, stdout=fw, text=True)


    p.stdin.write("uci\n")
    p.stdin.flush()
    l = fr.readline()
    print(f"Line: \"{l}\"")
    while l != "uciok":
        l = fr.readline()
        print(f"Line: \"{l}\"")
    p.stdin.write("position startpos\n")
    p.stdin.flush()
    p.stdin.write("go movetime 10000\n")
    p.stdin.flush()
    time.sleep(1)


    print(fr.readline())

    fw.close()
    fr.close()

