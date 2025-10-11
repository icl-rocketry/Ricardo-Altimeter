import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

# === Configuration ===
# You can specify a file path as a command-line argument, or default to processed_data/data.csv
DEFAULT_FILE = os.path.join("processed_data", "second_test.csv")

def plot_time_differences(csv_path):
    # Read the CSV
    df = pd.read_csv(csv_path)

    if "t_ms" not in df.columns:
        raise ValueError("The CSV file must contain a 't_ms' column.")

    # Compute time differences (Δt = t[n] - t[n-1])
    df["Δt_ms"] = df["t_ms"].diff()

    # Drop the first NaN
    df = df.dropna(subset=["Δt_ms"])

    # Plot
    plt.figure(figsize=(10, 5))
    plt.plot(df["t_ms"], df["Δt_ms"]/1000, marker="o", linestyle="-", markersize=3)
    plt.title("Time Difference Between Consecutive Readings (Δt_ms)")
    plt.xlabel("Timestamp (t_ms)")
    plt.ylabel("Δt_ms (seconds)")
    plt.grid(True)
    plt.tight_layout()

    plt.show()

if __name__ == "__main__":
    # Allow user to specify a file via command line
    csv_path = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_FILE

    if not os.path.exists(csv_path):
        print(f"❌ File not found: {csv_path}")
    else:
        plot_time_differences(csv_path)
