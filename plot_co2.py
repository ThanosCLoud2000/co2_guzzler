import pandas as pd
import matplotlib.pyplot as plt

# =========================
# 1) Load Excel (your new 4-day file)
# =========================
file_path = r"/mnt/data/SUTD KOH 12345 - Copy.xlsx"
df = pd.read_excel(file_path)

# Clean column names
df.columns = df.columns.astype(str).str.strip()

print("Columns found:", list(df.columns))

# =========================
# 2) Detect time column (seconds)
#    If not found, assume first column is time
# =========================
time_col = None
for c in df.columns:
    n = str(c).lower().strip()
    if n in ["time", "time (s)", "seconds", "sec", "x axis", "x_axis", "t", "t(s)"] or ("time" in n) or ("sec" in n):
        time_col = c
        break

if time_col is None:
    time_col = df.columns[0]

# =========================
# 3) Treat all other columns as day readings (4 days)
# =========================
day_cols = [c for c in df.columns if c != time_col]

# If your file contains extra columns, force only first 4 day columns:
day_cols = day_cols[:4]

if len(day_cols) < 2:
    raise ValueError(f"Expected multiple day columns (4 days), but got: {day_cols}")

# Keep only required columns
df = df[[time_col] + day_cols].copy()

# Numeric conversion
df[time_col] = pd.to_numeric(df[time_col], errors="coerce")
for c in day_cols:
    df[c] = pd.to_numeric(df[c], errors="coerce")

# Drop rows with missing time and sort
df = df.dropna(subset=[time_col]).sort_values(time_col)

# =========================
# 4) Aggregate to every 10 minutes (600 seconds)
# =========================
BIN_SECONDS = 600  # 10 minutes
df["time_bin_s"] = (df[time_col] // BIN_SECONDS) * BIN_SECONDS

agg = df.groupby("time_bin_s", as_index=False)[day_cols].mean()
agg["time_hours"] = agg["time_bin_s"] / 3600.0

# =========================
# 5) Plot all 4 days (NO markers)
# =========================
plt.figure(figsize=(12, 5))

for c in day_cols:
    plt.plot(agg["time_hours"], agg[c], linewidth=1.8, label=str(c))  # no markers

plt.xlabel("Time (Hours)")
plt.ylabel("CO2 (ppm)")
plt.title("CO2 vs Time (10-minute averages) — 4 Days")
plt.grid(True, alpha=0.3)
plt.legend(title="Day")
plt.tight_layout()
plt.show()
