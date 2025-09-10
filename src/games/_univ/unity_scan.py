# Changelog:
# - 0.02


import os
import shutil
import re
import subprocess


# Current directory (where this script is run)
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

# Check if parent folder is called "renodx"
parent_folder = os.path.basename(SCRIPT_DIR)

print(f"Parent folder: {parent_folder}")
if parent_folder == "renodx" or parent_folder == "renodx-dev" or parent_folder == "dump" or parent_folder == "renodx-dump":
    # If parent folder is "renodx", scan the current directory and output to output subfolder
    SCAN_DIR = os.path.join(SCRIPT_DIR, "dump")
    DEST_DIR = os.path.join(SCRIPT_DIR, "output")
else:
    # Otherwise use the default scan directory and current directory as destination
    SCAN_DIR = r"D:\SteamLibrary\steamapps\common\Darkest Dungeon® II\renodx"
    DEST_DIR = SCRIPT_DIR
# Path to the decompiler executable
DECOMPILER_PATH = os.path.join(SCAN_DIR, "cmd_Decompiler.exe")


# Define what makes a file 'uber' or other types by substring patterns
SUBSTRING_TYPES = {
    'uber': [
	    ["float3(0.0734997839,0.0734997839,0.0734997839)"],
    ],
    'lutbuilder': [
        ["0.00390625", "0.549941003"],  # Example: both 'LUT' and 'builder' must be present
	    ["float3(0.262677222,0.262677222,0.262677222) + float3(0.0695999935,0.0695999935,0.0695999935)"],
    ],
}

def convert_cso_to_hlsl(cso_file_path):
    """Convert a CSO file to HLSL using the decompiler."""
    try:
        # Run the decompiler command
        cmd = [DECOMPILER_PATH, "-D", cso_file_path]
        print(f"Running command: {cmd}")
        result = subprocess.run(cmd,capture_output=True, text=True, cwd=SCAN_DIR)
        
        if result.returncode == 0:
            print(f"Successfully converted {os.path.basename(cso_file_path)} to HLSL")
            return True
        else:
            print(f"Failed to convert {os.path.basename(cso_file_path)}: {result.stderr}")
            return False
    except Exception as e:
        print(f"Error converting {os.path.basename(cso_file_path)}: {e}")
        return False

def ensure_hlsl_exists():
    """Check for CSO files and convert them to HLSL if corresponding HLSL doesn't exist."""
    if not os.path.exists(DECOMPILER_PATH):
        print(f"Decompiler not found at {DECOMPILER_PATH}")
        return
    
    converted_count = 0
    for fname in os.listdir(SCAN_DIR):
        if fname.lower().endswith('.cso'):
            cso_path = os.path.join(SCAN_DIR, fname)
            hlsl_name = fname.replace('.cso', '.hlsl')
            hlsl_path = os.path.join(SCAN_DIR, hlsl_name)
            
            # Check if corresponding HLSL file exists
            if not os.path.exists(hlsl_path):
                print(f"Converting {fname} to HLSL...")
                if convert_cso_to_hlsl(fname):
                    converted_count += 1
            else:
                print(f"HLSL file {hlsl_name} already exists, skipping conversion")
    
    if converted_count > 0:
        print(f"Converted {converted_count} CSO files to HLSL")
    else:
        print("No CSO files needed conversion")

# Generalized function to check if a file matches any pattern set for a type
def file_matches_type(filepath, type_key):
    patterns = SUBSTRING_TYPES.get(type_key, [])
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
            for substrs in patterns:
                if all(x in content for x in substrs):
                    print(f"Found {type_key} in {filepath}")
                    return True
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
    return False

# Generalized function to process files for a given type
def process_files_for_type(type_key):
    os.makedirs(DEST_DIR, exist_ok=True)
    cand_dir = os.path.join(DEST_DIR, f"{type_key}_cands")
    patterns = SUBSTRING_TYPES.get(type_key, [])
    count = 0  # Counter for files found and copied
    hlsl_found = 0



    for fname in os.listdir(SCAN_DIR):
        if not fname.lower().endswith('.hlsl'):
            continue
        hlsl_found += 1
        src_path = os.path.join(SCAN_DIR, fname)
        if not os.path.isfile(src_path):
            continue
        if file_matches_type(src_path, type_key):
            match = re.search(r'0x([A-Fa-f0-9]+)', fname)
            if match:
                hex_part = match.group(1).upper()
                # Check if any file in DEST_DIR, cand_dir, or subfolders contains this hex pattern
                hex_pattern = f"0x{hex_part}"
                dest_files = os.listdir(DEST_DIR)
                cand_files = os.listdir(cand_dir) if os.path.exists(cand_dir) else []
                
                # Check subfolders
                ubers_dir = os.path.join(DEST_DIR, "ubers")
                lutbuilders_dir = os.path.join(DEST_DIR, "lutbuilders")
                ubers_files = os.listdir(ubers_dir) if os.path.exists(ubers_dir) else []
                lutbuilders_files = os.listdir(lutbuilders_dir) if os.path.exists(lutbuilders_dir) else []
                
                found = False
                for f in dest_files + cand_files + ubers_files + lutbuilders_files:
                    if hex_pattern in f and f.lower().endswith('.hlsl'):
                        found = True
                        break
                if found:
                    print(f"A file with hex pattern {hex_pattern} already exists in destination or candidate directory, skipping {fname}. Full path: {src_path}")
                    continue
                new_name = f"{type_key}_{hex_part}.{fname}"
                dest_path = os.path.join(DEST_DIR, new_name)
                cand_path = os.path.join(cand_dir, new_name)
                # Create candidate directory only if needed
                os.makedirs(cand_dir, exist_ok=True)
                print(f"Moving {fname} -> {os.path.relpath(cand_path, DEST_DIR)}")
                shutil.copy(src_path, cand_path)
                
                # Also move corresponding .cso file if it exists
                cso_name = fname.replace('.hlsl', '.cso')
                cso_src_path = os.path.join(SCAN_DIR, cso_name)
                if os.path.exists(cso_src_path):
                    cso_cand_path = os.path.join(cand_dir, cso_name.replace('.cso', f'_{hex_part}.cso'))
                    print(f"Moving {cso_name} -> {os.path.relpath(cso_cand_path, DEST_DIR)}")
                    # shutil.move(cso_src_path, cso_cand_path)
                
                count += 1
            else:
                print(f"No hex pattern found in {fname}, skipping.")
    
    # Remove candidate directory if it's empty
    if os.path.exists(cand_dir) and not os.listdir(cand_dir):
        # os.rmdir(cand_dir)
        print(f"Removed empty candidate directory: {cand_dir}")
    
    print(f"Total {type_key} files found and copied: {hlsl_found} {count}")

if __name__ == "__main__":
    files_to_copy = [
        "cmd_Decompiler.exe",
        "d3dcompiler_46.dll",
    ]
    # copyt to SCAN_DIR from SCRIPT_DIR
    for fname in files_to_copy:
        src_path = os.path.join(SCRIPT_DIR, fname)
        dest_path = os.path.join(SCAN_DIR, fname)
        print("COPY", src_path, dest_path)
        shutil.copy2(src_path, dest_path)


    # First, ensure all CSO files are converted to HLSL
    print("Checking for CSO files that need conversion...")
    ensure_hlsl_exists()
    print()
    
    # Call for each type you want to process
    process_files_for_type('uber')
    process_files_for_type('lutbuilder')

