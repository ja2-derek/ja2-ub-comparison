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
    args = ["diff", "-wq", path1, path2]
    return run_ret(args)
    #Note: 1 is different, 0 is same


def scan_folder(sub_path):
    print("scan_folder", sub_path)
    for item in sorted(os.listdir(os.path.join(DIR_JA2, sub_path))):
        item_full_path = os.path.join(sub_path, item)
        item_full_path_ja2 = os.path.join(DIR_JA2, item_full_path)
        #print("item_full_path_ja2", item_full_path_ja2)
        # Handle directory:
        if os.path.isdir(item_full_path_ja2):
            scan_folder(item_full_path)
            continue

        item_full_path_ub = os.path.join(DIR_UB, item_full_path)

        # Check if file is also in DIR_UB:
        if not os.path.isfile(item_full_path_ub):
            continue

        #print(item_full_path_ja2)

        ret = diff(item_full_path_ja2, item_full_path_ub)
        #print("  ", ret)
        if ret == 1: # files are different
            continue


        print("  ", item_full_path_ja2)
        
        item_full_path_common = os.path.join(DIR_COMMON, item_full_path)
        
        if os.path.exists(item_full_path_common):
            raise Exception("Common file should not exist.")
        
        item_common_dir = os.path.dirname(item_full_path_common)
        
        if not os.path.isdir(item_common_dir):
            os.makedirs(item_common_dir)
        #print(item_common_dir)
        
        print("->", item_full_path_common)
        
        args = ["git", "mv", item_full_path_ja2, item_full_path_common]
        ret = run_ret(args)
        
        if ret == 0: # success
            args = ["git", "rm", item_full_path_ub]
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

    scan_folder("")

if __name__ == "__main__":
    main()
