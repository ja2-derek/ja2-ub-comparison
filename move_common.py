#!/usr/bin/env python

import os
import subprocess

DIR_JA2 = "src_ja2"
DIR_UB = "src_ub"
DIR_COMMON = "src_common"


def run_ret(args):
    ret = subprocess.run(args, stdout=subprocess.PIPE)
    #print(ret)
    return ret.returncode


def diff(path1, path2):
    args = ["diff", "-wBq", path1, path2]
    return run_ret(args)
    #Note: 1 is different, 0 is same


# Similar to os.walk()
#   returns pairs of files in both folders
def scan_folder(sub_path, base1, base2):
    #print("scan_folder", sub_path, base1, base2)
    ret = []

    for item in sorted(os.listdir(os.path.join(base1, sub_path))):
        item_full_path = os.path.join(sub_path, item)
        path1 = os.path.join(base1, item_full_path)
        #print("path1", path1)
        # Handle directory:
        if os.path.isdir(path1):
            ret += scan_folder(item_full_path, base1, base2)
            continue

        path2 = os.path.join(base2, item_full_path)

        # Check if file is also in base2:
        if not os.path.isfile(path2):
            continue

        ret += [(path1, path2)]
    return ret


def move_common(sub_path):
    files = scan_folder(sub_path, DIR_JA2, DIR_UB)
    something_changed = False
    for no, (path1, path2) in enumerate(files):
        if no % 25 == 0:
            print("{: 3}% done".format(int(100*no/len(files))))

        # Check if file is also in DIR_UB:
        if not os.path.isfile(path2):
            continue

        ret = diff(path1, path2)
        if ret == 1: # files are different
            continue


        print("  ", path1)

        path3 = path1[len(DIR_JA2)+1:]

        path3 = os.path.join(DIR_COMMON, path3)

        if os.path.exists(path3):
            raise Exception("Common file should not exist.")

        item_common_dir = os.path.dirname(path3)

        if not os.path.isdir(item_common_dir):
            os.makedirs(item_common_dir)
        #print(item_common_dir)

        print("->", path3)

        #continue

        args = ["git", "mv", path1, path3]
        #print(args)
        ret = run_ret(args)

        ret = 0

        if ret == 0: # success
            args = ["git", "rm", path2]
            #print(args)
            run_ret(args)
            something_changed = True
    if something_changed:
        print("\n Some files have been moved.")
        rep = input("Commit changes (Y)?")
        if rep == "Y":
            args = ["git", "commit", "-m", "Use script to move shared files to src_common"]
            run_ret(args)


def main():
    if not os.path.exists(DIR_COMMON):
        os.mkdir(DIR_COMMON)

    if not os.path.isdir(DIR_COMMON):
        raise Exception(DIR_COMMON + " is not a directory.")

    if not os.path.isdir(DIR_JA2):
        raise Exception(DIR_JA2 + " is not a directory.")

    if not os.path.isdir(DIR_UB):
        raise Exception(DIR_UB + " is not a directory.")

    move_common("")

if __name__ == "__main__":
    main()
