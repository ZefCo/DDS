"""
Driven Diffusion System (DDS) - Simulation Core
Kawasaki dynamics on a 2D lattice with external E-field along y-axis.

Spins are +1 and -1 (NOT 0/1).
Hamiltonian: H = -J * sum(s_i * s_j) - E * sum(y_i * s_i)
J = 1 (dimensionless, set to 1 throughout)

Acceptance probability for a proposed swap:
  P = exp((-2*J*dE_ising + E*q) / T)
where:
  dE_ising = local_energy + prime_energy  (3-neighbor sum × spin, for each site)
  q = +1 if a +1 charge moves downward (with field)
    = -1 if a +1 charge moves upward (against field)
    =  0 for lateral moves
"""

import numpy as np
from numba import njit

# ---------------------------------------------------------------------------
# Lattice init
# ---------------------------------------------------------------------------

@njit
def init_lattice(Ny, Nx, seed=42):
    """
    Populate lattice with equal numbers of +1s and -1s, randomly placed.
    Uses ±1 spin convention matching the original C++ code.
    """
    np.random.seed(seed)
    N = Ny * Nx
    flat = np.full(N, -1, dtype=np.int8)
    flat[: N // 2] = 1
    # Fisher-Yates shuffle
    for i in range(N - 1, 0, -1):
        j = np.random.randint(0, i + 1)
        flat[i], flat[j] = flat[j], flat[i]
    return flat.reshape(Ny, Nx)


# ---------------------------------------------------------------------------
# Core sweep — faithful port of DDS_v4.cpp sweep() and delta_energy()
# ---------------------------------------------------------------------------

@njit
def _delta_energy(lattice, row, col, rop, cop, direction, Ny, Nx):
    """
    Compute the Ising exchange energy change for swapping (row,col) with (rop,cop).

    direction: 1=right, 2=down, 3=left, 4=up  (matching C++ convention)

    For each site, sum 3 neighbors (excluding the swap partner) × site spin.
    The excluded neighbor differs by direction to avoid double-counting the
    bond between the two swapping sites.

    Returns integer energy (range roughly -6 to +6 for J=1 with ±1 spins).
    """
    # Periodic neighbors of (row, col)
    rowu = (row - 1 + Ny) % Ny
    rowd = (row + 1) % Ny
    coll = (col - 1 + Nx) % Nx
    colr = (col + 1) % Nx

    # Periodic neighbors of (rop, cop)
    ropu = (rop - 1 + Ny) % Ny
    ropd = (rop + 1) % Ny
    copl = (cop - 1 + Nx) % Nx
    copr = (cop + 1) % Nx

    s  = lattice[row][col]
    sp = lattice[rop][cop]

    if direction == 1:   # moving right: exclude right neighbor of origin, left neighbor of dest
        local_energy = (lattice[rowd][col] + lattice[rowu][col] + lattice[row][coll]) * s
        prime_energy = (lattice[ropd][cop] + lattice[ropu][cop] + lattice[rop][copr]) * sp
    elif direction == 3: # moving left: exclude left neighbor of origin, right neighbor of dest
        local_energy = (lattice[rowd][col] + lattice[rowu][col] + lattice[row][colr]) * s
        prime_energy = (lattice[ropd][cop] + lattice[ropu][cop] + lattice[rop][copl]) * sp
    elif direction == 2: # moving down
        if s == 1:
            local_energy  = (lattice[row][coll] + lattice[rowu][col] + lattice[row][colr]) * s
            prime_energy  = (lattice[rop][copl] + lattice[ropd][cop] + lattice[rop][copr]) * sp
        else:  # s == -1, hole moves down = charge moves up
            local_energy  = (lattice[row][coll] + lattice[row][colr] + lattice[rowd][col]) * s
            prime_energy  = (lattice[rop][copl] + lattice[rop][copr] + lattice[ropu][cop]) * sp
    else:                # direction == 4, moving up
        if s == 1:
            local_energy  = (lattice[row][coll] + lattice[row][colr] + lattice[rowd][col]) * s
            prime_energy  = (lattice[rop][copl] + lattice[rop][copr] + lattice[ropu][cop]) * sp
        else:
            local_energy  = (lattice[row][coll] + lattice[row][colr] + lattice[rowu][col]) * s
            prime_energy  = (lattice[rop][copl] + lattice[rop][copr] + lattice[ropd][cop]) * sp

    return local_energy + prime_energy


@njit
def _nearest_neighbor(row, col, direction, spin, Ny, Nx):
    """
    Return the neighbor position for a given direction, accounting for
    the asymmetric field convention in the C++ code:
      - A +1 spin moving 'down' (dir=2) goes to row+1
      - A -1 spin moving 'down' (dir=2) goes to row-1 (opposite, like a hole)
    This is the physics of opposite charges drifting opposite ways in a field.
    """
    if spin == 1:
        if direction == 1:   ni, nj = row,              (col + 1) % Nx
        elif direction == 2: ni, nj = (row + 1) % Ny,   col
        elif direction == 3: ni, nj = row,              (col - 1 + Nx) % Nx
        else:                ni, nj = (row - 1 + Ny) % Ny, col
    else:  # spin == -1
        if direction == 1:   ni, nj = row,              (col + 1) % Nx
        elif direction == 2: ni, nj = (row - 1 + Ny) % Ny, col   # up for hole
        elif direction == 3: ni, nj = row,              (col - 1 + Nx) % Nx
        else:                ni, nj = (row + 1) % Ny,   col       # down for hole
    return ni, nj


@njit
def sweep(lattice, Ny, Nx, T, E):
    """
    One Monte Carlo sweep = Ny*Nx attempted Kawasaki moves.

    Acceptance probability:
      P = exp((-2 * dE_ising + E * q) / T)
    where q = +1 (with field), -1 (against field), 0 (lateral).
    Accept if P >= uniform random in [0,1).
    """
    N = Ny * Nx

    for _ in range(N):
        # Random site and direction
        row = np.random.randint(0, Ny)
        col = np.random.randint(0, Nx)
        direction = np.random.randint(1, 5)   # 1,2,3,4

        spin = lattice[row][col]

        ni, nj = _nearest_neighbor(row, col, direction, spin, Ny, Nx)

        # Only swap unlike spins
        if lattice[ni][nj] == spin:
            continue

        # E-field bias: q tracks whether a +1 charge moves with or against field (y-axis)
        # direction 2 = down (+y), direction 4 = up (-y)
        if direction == 2:
            q = 1 if spin == 1 else -1
        elif direction == 4:
            q = -1 if spin == 1 else 1
        else:
            q = 0

        energy = _delta_energy(lattice, row, col, ni, nj, direction, Ny, Nx)

        log_p = (-2.0 * energy + E * q) / T
        if log_p >= 0.0 or np.random.random() < np.exp(log_p):
            lattice[row][col], lattice[ni][nj] = lattice[ni][nj], lattice[row][col]

    return lattice


@njit
def run_sweeps(lattice, Ny, Nx, T, E, n_sweeps):
    """Run n_sweeps sweeps, return final lattice."""
    for _ in range(n_sweeps):
        lattice = sweep(lattice, Ny, Nx, T, E)
    return lattice


# ---------------------------------------------------------------------------
# Structure factor
# ---------------------------------------------------------------------------

def structure_factor(lattice):
    """
    2D power spectrum of the lattice spin configuration.
    DC component (k=0,0) zeroed — reflects only mean density.
    Returns |FFT|^2 with DC suppressed, fftshifted for display.
    """
    ft = np.fft.fft2(lattice.astype(np.float64))
    ft[0, 0] = 0.0
    sf = np.abs(np.fft.fftshift(ft)) ** 2
    return sf


# ---------------------------------------------------------------------------
# Smoke test
# ---------------------------------------------------------------------------

if __name__ == "__main__":
    Ny, Nx = 32, 20
    T = 2.0
    E = 20.0

    print(f"Initializing {Ny}x{Nx} lattice (±1 spins)...")
    lat = init_lattice(Ny, Nx)
    print(f"  +1s: {(lat==1).sum()}, -1s: {(lat==-1).sum()}")

    print("Compiling Numba (first call)...")
    run_sweeps(lat.copy(), Ny, Nx, T, 0.0, 1)
    print("  Done.")

    print("Running 100k equilibration sweeps (E=0)...")
    lat = run_sweeps(lat, Ny, Nx, T, 0.0, 100_000)
    print(f"  +1s after equil: {(lat==1).sum()} (should stay {Ny*Nx//2})")

    print("Running 500k production sweeps (E=20)...")
    lat = run_sweeps(lat, Ny, Nx, T, E, 500_000)
    print(f"  +1s after production: {(lat==1).sum()}")

    sf = structure_factor(lat)
    peak = np.unravel_index(sf.argmax(), sf.shape)
    print(f"  SF max: {sf.max():.1f} at k={peak}")

    # Quick ASCII render to visually check for stripe order
    print("\nLattice (@ = +1, . = -1):")
    for row in lat:
        print("".join("@" if s == 1 else "." for s in row))
