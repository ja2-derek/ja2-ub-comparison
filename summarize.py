###
# Usage:
#    pip3 install gitpython
#    python3 summarize.py

import git 
import csv
import re

MODULES = ['Laptop', 'Strategic', 'Tactical', 'TacticalAI', 'TileEngine', 'Editor', 'Utils', 'sgp']
MAX_COUNT = 100000

def list_commits(repo):
    table = []    
    regex = re.compile(': ')
    for commit in repo.iter_commits("master", max_count=MAX_COUNT, grep='PATCH'):
        # basic info
        msg = commit.message.split("\n")[0]
        msg = regex.split(msg)
        row = {
            'patch': msg[0],
            'summary': " ".join(msg[1:]),
            'sha': str(commit.hexsha[0:8]),
        }

        # which module(s) this commit changes
        for m in MODULES:
            row["_" + m] = changes_modules(commit.hexsha, m)

        table.append(row)
        #print(row)
    return table

def changes_modules(commit_sha, module):
    """Check if a file in current commit exist."""
    files = repo.git.show("--pretty=", "--name-only", commit_sha).split("\n")
    check = "/" + module + "/"
    for f in files:
        
        if check in f:
            return 1
    return 0

def print_csv(table, filename):
    fields = ['patch', 'summary', 'sha']
    for m in MODULES:
        fields.append("_" + m)

    with open(filename, 'w', newline='') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=fields, quoting=csv.QUOTE_ALL)
        writer.writeheader()
        for row in table:
            writer.writerow(row)

repo = git.Repo(".")
table = list_commits(repo)
print_csv(table, 'out.csv')
