import pandas as pd
import re
import os
import pathlib
import glob
import sys
import shutil

def extract_parts_list_from_bom(bom_file):
    # read BOM sheet of BOM tool into dataframe; header of BOM table is on
    # line 18, so skip first 17 rows
    bom_data = pd.read_excel(io=bom_file, sheet_name='BOM', skiprows=17, engine='pyxlsb')

    # get part # and rev columns and remove any NaN rows
    parts_df = bom_data[["Part #","Kit(s)","Rev"]].dropna()

    # remove rows with 'z' in the part # column
    parts_df = parts_df[parts_df["Part #"]!='z']

    # convert part # to string
    parts_df['Part #'] = parts_df['Part #'].apply(lambda x : str(x))

    # create composite name using part # and rev
    create_compsite_name = lambda x : x['Part #'] if x['Rev'] in ['na', 0] else f"{x['Part #'].split('-')[0]}.Rev{x['Rev']}"
    parts_df["CompositePartName"] = parts_df[["Part #","Rev"]].apply(create_compsite_name, axis=1)

    parts_list = []
    for part_number, kits in  set(parts_df[["CompositePartName","Kit(s)"]].apply(tuple, axis=1)):
        for kit in kits.split(','):
            parts_list.append((part_number, kit.strip()))

    return parts_list

def find_string_in_folder(string, folder):
    matching_files = []
    for dir_path, dirs, file_names in os.walk(folder):
        for file_name in file_names:
            if string.lower() in file_name.lower(): # case insensitive
                fullpath = os.path.join(dir_path, file_name)
                matching_files.append(fullpath)
    return matching_files

def search_for_part_drawings(part, folder):
    matching_files = find_string_in_folder(part, folder)
    if matching_files: # exact matches found
        return matching_files
    try:
        partno, revno = part.split('.', 1)
    except ValueError: # composite part number may lack the trailing .Rev... string
        partno = part
        revno = ''

    # search again only by part number
    matching_files = find_string_in_folder(partno, folder)
    if matching_files:
        if len(matching_files) == 1 or revno == '':
            return matching_files

        revno_pattern = rf'\s*[.-_]?\s*(rev)?(\.)?\s*0?{revno[-1]}'
        matching_files = [x for x in matching_files
                          if re.search(revno_pattern, x, re.IGNORECASE)]
    return matching_files

# establish all paths relative to the location of this script
if getattr(sys, 'frozen', False):
    this_path = os.path.dirname(sys.executable)
elif __file__:
    this_path = os.path.dirname(os.path.abspath(__file__))

trackers_path = pathlib.Path(this_path).parents[2] # trackers path should be 2 parents above where this script is located
trackers_path = os.path.join(this_path, "Data", "Trackers")
master_drawings_path = os.path.join(trackers_path,"7_Voyager","Drawings")
if os.path.exists(master_drawings_path)==False:
    print(f'Master drawings path does not exist:\n{master_drawings_path}')
    sys.exit()

project_drawings_path = os.path.join(this_path,"Drawings")
project_drawings_path = "/home/haileyesus/data/Drawings"
if os.path.exists(project_drawings_path)==False:
    print(f'Project drawings path does not exist:\n{project_drawings_path}')
    sys.exit()

project_bom_path = os.path.join(this_path,"BOM", "WIP")
if os.path.exists(project_bom_path)==False:
    print(f'Project BOM path does not exist:\n{project_bom_path}')
    sys.exit()

# find the latest xlsb file in the BOM directory...this should be the most recent BOM
possible_bom_files = glob.glob(project_bom_path + '/*.xlsb')
if not possible_bom_files:
    print(f'No BOM files in project BOM folder:\n{project_bom_path}')
    sys.exit()

bom_file = max(possible_bom_files, key=os.path.getmtime)
if os.path.exists(bom_file)==False:
    print(f'Project BOM file does not exist:\n{bom_file}')
    sys.exit()

print("BOM File:")
print(bom_file)
# generate parts list from latest BOM file
parts_list = extract_parts_list_from_bom(bom_file)
# get possible assembly destination folder names from the project drawings path
assemblies = [f.name for f in os.scandir(project_drawings_path) if f.is_dir()]

# search drawings folder (incl subdirectories, case insensitive) for each composite part name
results = []
for part_number, kit in parts_list:
    best_match = None
    result = {
        "Part": part_number,
        "Kit": kit,
        "Found": True,
        "Copied": True,
        "Source":"",
        "Destination":""
    }
    matching_files = search_for_part_drawings(part_number, master_drawings_path)
    if matching_files:
        best_match = matching_files[0]

    # filter for only those in directories that match the assemblies list
    matching_files = [f for f in matching_files if f in assemblies]
    if matching_files:
        best_match = matching_files[0]

    # filter further by Kit ID
    matching_files = [f for f in matching_files if f'{kit}_' in f]

    # if the filtering results in zero matches, try results from previous filters
    if not matching_files and best_match:
        '''THIS STILL DOESN'T WORK IF THE PARENT FOLDER OF THE SOURCE CAN'T FIND
           A MATCHING PARENT FOLDER AT THE DESTINATION IN 'if folder in selection_drawing' BELOW
        '''
        matching_files.append(best_match)

    if matching_files:
        selected_drawing = matching_files[0].replace("\\","/")
        result["Source"] = selected_drawing
        for folder in assemblies:
            result["Copied"] = False
            if folder in selected_drawing:
                copy_to_path = os.path.join(project_drawings_path,folder)
                result["Destination"] = copy_to_path
                print(f'Copying drawing for {part_number}')
                try:
                    shutil.copy2(selected_drawing,copy_to_path)
                    result["Copied"] = True
                except FileNotFoundError:
                    print('Copy failed: File not found')
                except:
                    print(sys.exc_info()[0])
                break
    else: # no match found for this part
        result["Found"] = False
        result["Copied"] = False
    results.append(result)

# save the log into an Excel file in the same directory
results_df = pd.DataFrame.from_dict(results)
writer = pd.ExcelWriter('drawings_copy_log.xlsx', engine='openpyxl')
results_df.to_excel(writer,sheet_name='Log',index=False)
writer.save()
