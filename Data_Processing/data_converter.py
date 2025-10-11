import os
import csv

# === Configuration ===
INPUT_DIR = "raw_data"
INPUT_FILE = os.path.join(INPUT_DIR, "LOG04.TXT")  # Path to your input text file
OUTPUT_DIR = "processed_data"
OUTPUT_FILE = os.path.join(OUTPUT_DIR, "second_test.csv")

def convert_txt_to_csv(input_path, output_path):
    # Ensure output directory exists
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    with open(input_path, "r") as infile:
        lines = infile.readlines()

    # First line may contain headers or not — detect automatically
    header = None
    data = []

    for line in lines:
        # Skip empty lines
        if not line.strip():
            continue

        # Split on commas and strip spaces
        parts = [p.strip() for p in line.split(",")]

        # If first entry isn't numeric, treat as header
        if header is None and not parts[0].replace('.', '', 1).isdigit():
            header = parts
        else:
            data.append(parts)

    # If no header, define one generically
    if header is None:
        num_cols = len(data[0]) if data else 0
        header = [f"col_{i+1}" for i in range(num_cols)]

    # Write CSV
    with open(output_path, "w", newline="") as outfile:
        writer = csv.writer(outfile)
        writer.writerow(header)
        writer.writerows(data)

convert_txt_to_csv(INPUT_FILE, OUTPUT_FILE)