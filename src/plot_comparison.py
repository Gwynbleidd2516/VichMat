#!/usr/bin/env python3
"""
Построение графиков сравнения численных методов.
Запуск: python3 plot_comparison.py
"""

import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path

# ============================================================
# Загрузка данных
# ============================================================

def load_trajectory(method_name, dir="./comparison/"):
    """Загружает траекторию для указанного метода."""
    filepath = Path(dir) / f"trajectory_{method_name}.csv"
    data = np.loadtxt(filepath, delimiter=',', skiprows=1)
    return data

def load_rmsd(dir="./comparison/"):
    """Загружает RMSD между методами."""
    filepath = Path(dir) / "rmsd_comparison.csv"
    data = np.loadtxt(filepath, delimiter=',', skiprows=1)
    # Читаем заголовок для имён колонок
    with open(filepath, 'r') as f:
        header = f.readline().strip().split(',')[1:]  # без 't'
    return data, header

# ============================================================
# График 1: Траектории всех методов (2D проекция)
# ============================================================

def plot_trajectories_2d(n_bodies=3, body_idx=0):
    """
    Рисует траектории одного тела для всех методов.
    body_idx: индекс тела (0, 1, 2, ...)
    """
    methods = ["Euler", "Leapfrog", "RK4", "DP8"]
    colors = ["red", "orange", "green", "blue"]
    
    plt.figure(figsize=(10, 8))
    
    for m, color in zip(methods, colors):
        data = load_trajectory(m)
        t = data[:, 0]
        x = data[:, 1 + body_idx * 3]
        y = data[:, 2 + body_idx * 3]
        plt.plot(x, y, color=color, label=m, linewidth=1, alpha=0.7)
    
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.title(f"Body {body_idx + 1} trajectory (XY projection)")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.axis('equal')
    plt.savefig("comparison/trajectories_2d.png", dpi=150)
    print("Saved: comparison/trajectories_2d.png")
    plt.show()

# ============================================================
# График 2: RMSD между методами (логарифмическая шкала)
# ============================================================

def plot_rmsd():
    """Рисует RMSD между всеми парами методов."""
    data, col_names = load_rmsd()
    t = data[:, 0]
    
    plt.figure(figsize=(12, 6))
    
    for i, name in enumerate(col_names):
        rmsd = data[:, i + 1]
        # Заменяем нули на 1e-20 для логарифма
        rmsd_plot = np.maximum(rmsd, 1e-20)
        plt.semilogy(t, rmsd_plot, label=name, linewidth=1.5)
    
    plt.xlabel("Time t")
    plt.ylabel("RMSD (log scale)")
    plt.title("Deviation between numerical methods")
    plt.legend(loc='upper left', fontsize=8)
    plt.grid(True, alpha=0.3, which='both')
    plt.savefig("comparison/rmsd_comparison.png", dpi=150)
    print("Saved: comparison/rmsd_comparison.png")
    plt.show()

# ============================================================
# График 3: Энергия (если сохранена)
# ============================================================

def plot_energy_drift():
    """
    Рисует отклонение энергии от начального значения.
    Требует модификации C++ кода для сохранения энергии.
    Пока заглушка.
    """
    print("Energy plot: requires energy data in CSV (not implemented yet)")

# ============================================================
# График 4: 3D траектория (для одного метода)
# ============================================================

def plot_trajectory_3d(method="DP8", n_bodies=3):
    """Рисует 3D траекторию для указанного метода."""
    from mpl_toolkits.mplot3d import Axes3D
    
    data = load_trajectory(method)
    t = data[:, 0]
    
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    
    colors = plt.cm.viridis(np.linspace(0, 1, n_bodies))
    
    for i in range(n_bodies):
        x = data[:, 1 + i * 3]
        y = data[:, 2 + i * 3]
        z = data[:, 3 + i * 3]
        ax.plot(x, y, z, color=colors[i], label=f"Body {i+1}", linewidth=1)
        ax.scatter(x[0], y[0], z[0], color=colors[i], s=50, marker='o')
    
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")
    ax.set_title(f"3D trajectory ({method} method)")
    ax.legend()
    plt.savefig(f"comparison/trajectory_3d_{method}.png", dpi=150)
    print(f"Saved: comparison/trajectory_3d_{method}.png")
    plt.show()

# ============================================================
# Главная функция
# ============================================================

if __name__ == "__main__":
    print("=== N-body method comparison plots ===\n")
    
    # Проверяем наличие файлов
    if not Path("./comparison/trajectory_DP8.csv").exists():
        print("ERROR: No data found. Run C++ program first!")
        exit(1)
    
    # Определяем число тел из файла
    sample_data = load_trajectory("DP8")
    n_cols = sample_data.shape[1]
    n_bodies = (n_cols - 1) // 3  # минус колонка времени
    print(f"Detected {n_bodies} bodies\n")
    
    # Строим графики
    plot_trajectories_2d(n_bodies=n_bodies, body_idx=0)
    plot_rmsd()
    plot_trajectory_3d(method="DP8", n_bodies=n_bodies)
    
    print("\n=== Done ===")