#!/usr/bin/env python3

import sys, csv
import matplotlib.pyplot as plt

def load(path):
    rows = []
    with open(path) as f:
        for line in csv.reader(f):
            if not line:
                continue
            try:
                rows.append([float(v) for v in line])
            except ValueError:
                continue
    return rows

def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "comparison.csv"
    rows = load(path)
    if not rows:
        print(f"No numeric rows in {path}")
        return
    
    ncols = len(rows[0])
    rows = [r for r in rows if len(r) == ncols]
    t = range(len(rows))

    if ncols == 9:
        axes_names = ["x", "y", "z"]
        fig, panels = plt.subplots(3, 1, figsize=(13, 9), sharex=True)
        for i, (panel, name) in enumerate(zip(panels, axes_names)):
            raw = [r[3*i]     for r in rows]
            ma  = [r[3*i + 1] for r in rows]
            fir = [r[3*i + 2] for r in rows]
            panel.plot(t, raw, label="raw", linewidth=0.6, alpha=0.45)
            panel.plot(t, ma,  label="moving avg", linewidth=1.0)
            panel.plot(t, fir, label="FIR (windowed-sinc)", linewidth=1.2)
            panel.set_ylabel(f"{name} (g)")
            panel.legend(loc="upper right", fontsize=8)
            panel.grid(True, alpha=0.3)
        panels[0].set_title(f"Raw vs Moving Average vs FIR ({len(rows)} samples)")
        panels[-1].set_xlabel("sample index")

    elif ncols == 3:
        names = ["x", "y", "z"]
        fig, ax = plt.subplots(figsize=(12, 5))
        for i, name in enumerate(names):
            ax.plot(t, [r[i] for r in rows], label=name, linewidth=0.9)
        ax.set(title=f"Capture ({len(rows)} samples)",
               xlabel="sample index", ylabel="acceleration (g)")
        ax.legend(); ax.grid(True, alpha=0.3)
    else:
        print(f"Unexpected column count: {ncols}")
        return
    
    plt.tight_layout()
    out = path.rsplit(".", 1)[0] + ".png"
    plt.savefig(out, dpi=110)
    print(f"Saved {out}")

if __name__ == "__main__":
    main()