# What this is:
This repo contains the vanilla Jagged Alliance 2 (`src_ja2`) and the Unfinished Business (`src_ub`) source code.

Files common to both source trees can be found in (`src_common`).

The files have been converted to unix line endings. The folder structure of both trees have been adjusted to roughly match the Stracciatella folder structure.

## Initial version:
The initial version can be found at https://github.com/ja2-stracciatella/ja2-ub-comparison/commit/83ebfe0ffa17ce18f558192d8cde62d997bd1d52

Note: The old Visual Studio project files and similar files have been removed.

## Continuation:
The basic idea is to generate a set of patches that transform the code in `src_ja2` to the Unfinised Business code.

### Strategy:
- Unimportant differences may be removed from either of the folders, i.e. `src_ja2` or `src_ub`.
- Important differences (features/bug fixes) may be removed by merging them into `src_ja2`. The commit title has to start with `PATCHXXX` where XXX is a three digit 
number. If the patch only modifies the editor, the commit title has to start with `EDITORYYY`.
- The python script `move_common.py` can be used to automatically move equal (modulo white space) files to `src_common`. These changes should be commited without any manual changes. The commit message should be `Use script to move shared files to src_common`.
