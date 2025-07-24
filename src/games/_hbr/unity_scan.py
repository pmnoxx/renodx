import os
import shutil
import re

# Directory to scan
SCAN_DIR = r"C:\Program Files (x86)\Steam\steamapps\common\HeavenBurnsRed\renodx-dev\dump"
# Current directory (where this script is run)
DEST_DIR = os.path.dirname(os.path.abspath(__file__))


# Define what makes a file 'uber' or other types by substring patterns
SUBSTRING_TYPES = {
    'uber': [
      # ["cb0[136].xy", "SampleLevel"],
       ["cb0[36]", "floor", "SampleLevel"],
        # Add more patterns for 'uber' as needed
    ],
    'lutbuilder': [
        ["0.00390625"],  # Example: both 'LUT' and 'builder' must be present
        # Add more patterns for 'lut_builder' as needed
    ],
}

# Generalized function to check if a file matches any pattern set for a type
def file_matches_type(filepath, type_key):
    patterns = SUBSTRING_TYPES.get(type_key, [])
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
            for substrs in patterns:
                if all(x in content for x in substrs):
                    return True
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
    return False

# Generalized function to process files for a given type
def process_files_for_type(type_key):
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
                # Check if any file in DEST_DIR or cand_dir contains this hex pattern
                hex_pattern = f"0x{hex_part}"
                dest_files = os.listdir(DEST_DIR)
                cand_files = os.listdir(cand_dir) if os.path.exists(cand_dir) else []
                found = False
                for f in dest_files + cand_files:
                    if hex_pattern in f and f.lower().endswith('.hlsl'):
                        found = True
                        break
                if found:
                    print(f"A file with hex pattern {hex_pattern} already exists in destination or candidate directory, skipping {fname}.")
                    continue
                new_name = f"{type_key}_{hex_part}.{fname}"
                dest_path = os.path.join(DEST_DIR, new_name)
                cand_path = os.path.join(cand_dir, new_name)
                # Create candidate directory only if needed
                os.makedirs(cand_dir, exist_ok=True)
                print(f"Copying {fname} -> {os.path.relpath(cand_path, DEST_DIR)}")
                shutil.copy2(src_path, cand_path)
                count += 1
            else:
                print(f"No hex pattern found in {fname}, skipping.")
    print(f"Total {type_key} files found and copied: {hlsl_found} {count}")

if __name__ == "__main__":
    # Call for each type you want to process
    process_files_for_type('uber')
    process_files_for_type('lutbuilder')
