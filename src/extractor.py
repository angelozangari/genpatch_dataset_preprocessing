import os
import pandas as pd
import re

def extract_snippet_from_file(file, start_line, start_col, end_line, end_col):
    with open(file, 'r') as f:
        file_content = f.readlines()

    snippet = ""

    ## we assume cols are 0-indexed, for instance look at ek-123-libfind2Ffind_list.c

    # initial line is when we first start to extract the snippet
    # it will either be a set number of lines n or the beginning of the previous function
    initial_line = start_line - 5

    for i in range (max(0, initial_line), end_line):
        snippet += file_content[i]

    # get file name by removing the part of path before
    file_name = re.search(r'([^/]+)$', file).group(0)
    
    return pd.DataFrame([{'file': file_name, 'snippet': snippet}])
    
def extract_vuln_code(file):
    # read file
    with open(file, 'r') as f:
        file_content = f.readlines()

    # go to VULNERABLE LINE line
    vuln_line = -1
    for i, line in enumerate(file_content):
        if "↓↓↓VULNERABLE LINES↓↓↓" in line:
            vuln_line = i
            break

    # parse each line pointing to snippet
    vuln_pattern = re.compile(r'\s*//\s*(\d+),(\d+);(\d+),(\d+)')
    snippets = []

    # call extract snippet function
    for line in file_content[vuln_line+1 : ]:
        match = vuln_pattern.search(line)
        if match:
            start_line, start_col, end_line, end_col = map(int, match.groups())
            # each snippet is a dataframe
            snippet = extract_snippet_from_file(file, start_line, start_col, end_line, end_col)
            snippets.append(snippet)

    return snippets # list of dataframes


def main():
    dataset_path = f'{os.getcwd()}/data/Vulnerable'
    print(dataset_path)

    code_snippets = []

    for vuln_c_file in sorted(os.listdir(dataset_path)):
        vuln_c_file_path = f'{dataset_path}/{vuln_c_file}'
        if os.path.isfile(vuln_c_file_path) and vuln_c_file.endswith('.c'):
            vuln_snippets = extract_vuln_code(vuln_c_file_path)
            code_snippets.extend(vuln_snippets)

    code_dataframes = pd.concat(code_snippets, ignore_index=True)

    code_dataframes.to_csv('output.csv', index=False)

if __name__ == '__main__':
    main()