"""
Driven Diffusion System (DDS) — Interactive Streamlit Demo
"""

import io
import time
import zipfile

import numpy as np
import streamlit as st
import plotly.graph_objects as go

from dds_sim import init_lattice, run_sweeps, structure_factor

# ── Page config ──────────────────────────────────────────────────────────────
st.set_page_config(
    page_title="Driven Diffusion System",
    layout="wide",
    initial_sidebar_state="expanded",
)

# ── Minimal custom CSS ────────────────────────────────────────────────────────
st.markdown("""
<style>
  /* Tighten header spacing */
  h1 { margin-bottom: 0.15rem; }
  h3 { margin-top: 0.5rem; margin-bottom: 0.2rem; }
  /* Subtle separator */
  hr { border-color: #334155; margin: 0.75rem 0; }
  /* Caption text */
  .caption { font-size: 0.78rem; color: #94a3b8; }
  /* Disclaimer box */
  .disclaimer {
    background: #1e293b;
    border-left: 3px solid #f59e0b;
    padding: 0.5rem 0.75rem;
    border-radius: 0 4px 4px 0;
    font-size: 0.82rem;
    color: #cbd5e1;
    margin-top: 0.5rem;
  }
</style>
""", unsafe_allow_html=True)

# ── Constants ─────────────────────────────────────────────────────────────────
LATTICE_SIZES = {
    "20 × 20": (20, 20),
    "32 × 32": (32, 32),
    "44 × 26": (44, 26),
    "82 × 32": (82, 32),
    "128 × 64": (128, 64),
}
T_ONSAGER = 2.269  # exact Onsager critical temp for 2D Ising
EQUIL_SWEEPS = 1
SNAPSHOT_INTERVAL = 100_000  # save one data point every 100k sweeps
DISPLAY_EVERY = 1_000        # redraw visuals every N sweeps

# Colorblind-friendly colorscales for Plotly
# Lattice: discrete two-color (blue=-1, gold=+1)
LATTICE_COLORSCALE = [[0.0, "#1d4ed8"], [1.0, "#f59e0b"]]
LATTICE_ZMIN, LATTICE_ZMAX = -1, 1
# Structure factor: inferno-like perceptual gradient
SF_COLORSCALE = "Inferno"

# ── Session state init ────────────────────────────────────────────────────────
def init_state():
    defaults = {
        "lattice": None,
        "phase": "idle",        # idle | equilibrating | running | paused | done
        "sweep_count": 0,       # sweeps completed in production phase
        "equil_done": False,
        "snapshots": [],        # list of dicts: {sweep, lattice, sf}
        "Ny": 20, "Nx": 20,
        "T": 2.0, "E": 20.0,
        "target_sweeps": 100_000,
    }
    for k, v in defaults.items():
        if k not in st.session_state:
            st.session_state[k] = v

init_state()
ss = st.session_state

# ── Helper: render lattice plot ───────────────────────────────────────────────
_DARK = "#0f172a"
_AXIS_COLOR = "#94a3b8"
_GRID_COLOR = "#1e293b"

def _base_layout(title, xlab, ylab, Nx, Ny):
    """Shared dark-theme layout for both plots."""
    return go.Layout(
        title=dict(text=title, font=dict(color="#e2e8f0", size=13), x=0.5),
        paper_bgcolor=_DARK,
        plot_bgcolor=_DARK,
        margin=dict(l=50, r=10, t=40, b=40),
        xaxis=dict(title=xlab, color=_AXIS_COLOR, tickfont=dict(color=_AXIS_COLOR),
                   showgrid=False, zeroline=False, scaleanchor="y", constrain="domain"),
        yaxis=dict(title=ylab, color=_AXIS_COLOR, tickfont=dict(color=_AXIS_COLOR),
                   showgrid=False, zeroline=False, autorange="reversed",
                   constrain="domain"),
        legend=dict(bgcolor="#1e293b", font=dict(color="#e2e8f0"), borderwidth=0),
        height=400,
    )


def render_lattice(lattice):
    Ny, Nx = lattice.shape
    fig = go.Figure(
        go.Heatmap(
            z=lattice,
            colorscale=LATTICE_COLORSCALE,
            zmin=LATTICE_ZMIN, zmax=LATTICE_ZMAX,
            showscale=False,
            hovertemplate="x=%{x}, y=%{y}<br>spin=%{z}<extra></extra>",
        ),
        layout=_base_layout("Charge Distribution", "x", "y (E-field axis)", Nx, Ny),
    )
    # Manual legend via invisible scatter traces
    fig.add_trace(go.Scatter(x=[None], y=[None], mode="markers",
        marker=dict(color="#1d4ed8", size=10, symbol="square"),
        name="Charge −1"))
    fig.add_trace(go.Scatter(x=[None], y=[None], mode="markers",
        marker=dict(color="#f59e0b", size=10, symbol="square"),
        name="Charge +1"))
    return fig


def render_sf(sf):
    Ny, Nx = sf.shape
    fig = go.Figure(
        go.Heatmap(
            z=sf,
            colorscale=SF_COLORSCALE,
            showscale=True,
            colorbar=dict(
                tickfont=dict(color=_AXIS_COLOR),
                outlinewidth=0,
            ),
            hovertemplate="kx=%{x}, ky=%{y}<br>|S(k)|²=%{z:.1f}<extra></extra>",
        ),
        layout=_base_layout("Structure Factor |S(k)|²", "kₓ", "ky (field direction)", Nx, Ny),
    )
    return fig

# ── Sidebar: controls ─────────────────────────────────────────────────────────
with st.sidebar:
    st.markdown("## ⚙️ Simulation Controls")
    st.markdown("---")

    size_label = st.selectbox(
        "Lattice size (rows × cols)",
        list(LATTICE_SIZES.keys()),
        help="Rows = y-axis (E-field direction). These sizes were used in the original study."
    )
    Ny, Nx = LATTICE_SIZES[size_label]

    T_val = st.slider(
        "Temperature T",
        min_value=1.0, max_value=5.0, value=2.0, step=0.05,
        help=f"T_Onsager ≈ {T_ONSAGER:.3f}. T_critical for DDS ≈ 1.4 × T_Onsager ≈ {1.4*T_ONSAGER:.2f}."
    )
    st.markdown(f"<p class='caption'>T / T_Onsager = {T_val/T_ONSAGER:.2f} &nbsp;|&nbsp; T_c(DDS) ≈ {1.4*T_ONSAGER:.2f}</p>", unsafe_allow_html=True)

    E_val = st.slider(
        "Electric field E",
        min_value=0.0, max_value=100.0, value=20.0, step=1.0,
        help="Field applied along the y-axis (vertical). E=0 gives equilibrium Ising behavior."
    )

    st.markdown("---")
    st.markdown("**Production sweeps** (after equilibration)")
    target = st.number_input(
        "Number of sweeps",
        min_value=100_000,
        max_value=10_000_000,
        value=500_000,
        step=100_000,
        help="Recommended: 500k–1M for visible ordering. Each sweep = Ny×Nx attempted moves."
    )
    st.markdown("""
<p class='caption'>
💡 Suggested:<br>
&nbsp;&nbsp;100k — quick look<br>
&nbsp;&nbsp;500k &nbsp;— typical length of analysis<br>
&nbsp;&nbsp;1M+ — long term analysis
</p>
""", unsafe_allow_html=True) 

    st.markdown("---")
    col1, col2 = st.columns(2)
    run_btn = col1.button("▶ Run", use_container_width=True, type="primary")
    pause_btn = col2.button("⏸ Pause", use_container_width=True)
    reset_btn = st.button("↺ Reset", use_container_width=True)

# ── Main layout ───────────────────────────────────────────────────────────────
st.markdown("# ⚡ Driven Diffusion System")
st.markdown(
    "A lattice of charges driven out of equilibrium by an external electric field. "
)

# Explainer expander
with st.expander("Introduction", expanded=False):
    st.markdown("""
The Ising Model is a simple lattice of discrete charges that interact locally. In it's simpliest form,
the spins of the lattice have a random chance to influence their neighbors [without an external electric 
field], causing them to try to align. Below a certian critical tempurature large regions and clusters of 
spins will align. The size and distirbution of these clustes can be used as a way to study the critically 
and universality of the system.

More complicated Ising Models allow for studying more interesting physics: for example, instead of flipping
spins in the standard Ising Model, Kawasaki Dynamics can be used to see how charges are exchanged on the
lattice, allowing them to move around. This is an Ising Lattice Gas. Additionally, an external electric 
field can be applied, which then gives a bias to the movements of the charges: if the field is oriented 
in the y direction, some charages will prefer to go against the field, while others prefer to go with it: 
both will have no problem traversing the field latteral to its orientation (the x axis).
                
As a result of the bias in the motion of the charges, the electric field creates a non-equillibrium, something
that is still an active area of research in physics. This system is refered to as a Driven Diffusive System (DDS),
and surprisingly, will see like charges cluster together into large bands across the lattice. They quickly align
themselves along the field. Due to the anisotropic nature of the motion, this also means the lattice size must be
carefully chosen for proper study: the lattice should be eloganted along the electric field to properly measure
the critical exponents.
                
To actually measure the system, the Structure Factor (SF) of the lattice is measured by a Fourier Transform of
position of all the charges. The lattice does not gain or loose any charge, the density of the system stays the
same. This means that something such as the magnitization of the lattice is constant and gives no insight into
what is happening. But by taking a 2-dimensional Fourier Transform of the system, the underlying strcuture can be
measured with |S(k)|².

**Temperature guide**
- **T < 1.5** — strong ordering, clear stripes (if E is non-zero)  
- **T ≈ 1.5–2.5** — near the DDS critical temperature (~3.2), fluctuating order  
- **T > 3.5** — disordered phase, no persistent stripes
                
Note: this code was written in python and is intended for simple simultions. It is part of a large project:
                https://github.com/ZefCo/DDS
""")

st.markdown("---")

# Status bar
status_col, sweep_col, snap_col = st.columns([3, 2, 2])
status_placeholder = status_col.empty()
sweep_placeholder = sweep_col.empty()
snap_placeholder = snap_col.empty()

# Visualization panels
viz_col1, viz_col2 = st.columns(2)
with viz_col1:
    st.markdown("### Charge Lattice")
    lattice_placeholder = st.empty()
with viz_col2:
    st.markdown("### Structure Factor")
    sf_placeholder = st.empty()

# Second explainer
with st.expander("How to read the plots", expanded=False):
    st.markdown("""
**Charge lattice (left)**  
Each pixel is one lattice site. **Gold** = charge present (+1), **Blue** = empty (-1). 
The y-axis is the electric field direction.

**Structure factor (right)**  
Brighter = stronger signal at that wavevector. The center (DC component, k=0) is removed 
since it just reflects the constant average density. 

A **bright spot away from center** means the charges have a preferred spatial periodicity 
at that length scale. In the DDS, a peak along the k_y axis signals stripe order perpendicular 
to the driving field. As temperature decreases or E increases, this peak sharpens.

Run this across a variety of tempuratures, electric field strengths, 
""")

with st.expander("Additional Reading/Citations", expanded=False):
    st.markdown("""
For additional reading on Driven Diffusion Systems and the Ising Model:
                
- Cipra, Barry A. "An introduction to the Ising model." The American Mathematical Monthly 94.10 (1987): 937-959.
- Domb, Cyril, et al. Statistical mechanics of driven diffusive systems. Vol. 17. Elsevier, 1995.
- Newman, Mark EJ, and Gerard T. Barkema. Monte Carlo methods in statistical physics. Clarendon Press, 1999.
- Schmittmann, B., and R. K. P. Zia. "Driven diffusive systems. An introduction and recent developments." Physics reports 301.1-3 (1998): 45-64.
- Schmittmann, Beate, and Royce KP Zia. "Statistical mechanics of driven diffusive systems." Phase transitions and critical phenomena 17 (1995): 3-214.
- Sklar, Lawrence. Physics and chance: Philosophical issues in the foundations of statistical mechanics. Cambridge University Press, 1993.
- Zia, R. K. P., and T. Blum. "An introduction to the statistical mechanics of driven diffusive systems." Scale Invariance, Interfaces, and Non-Equilibrium Dynamics. Boston, MA: Springer US, 1995. 111-131.
                
These are interesting applications of DDS
- Helbing, Dirk. "Traffic and related self-driven many-particle systems." Reviews of modern physics 73.4 (2001): 1067.
- Nagatani, Takashi. "The physics of traffic jams." Reports on progress in physics 65.9 (2002): 1331-1386.
- Schadschneider, Andreas, Debashish Chowdhury, and Katsuhiro Nishinari. Stochastic transport in complex systems: from molecules to vehicles. Elsevier, 2010.
                """)

# Download section
st.markdown("---")
st.markdown("### 📥 Download Data")
dl_col1, dl_col2 = st.columns([3, 1])
dl_col1.markdown("""
<div class='disclaimer'>
⚠️ <strong>Note on data collection:</strong> Snapshots are saved every 100,000 production sweeps. 
In a more rigorous study, you would use an autocorrelation analysis to find the correlation time and 
collect only statistically independent samples. For this demo, the fixed interval is a reasonable 
approximation — but treat these as illustrative, not publication-quality independent samples. Additionally
there is no warm-up period in this simulation, to show the inital set of sweeps before the charges align.
</div>
""", unsafe_allow_html=True)
download_placeholder = dl_col2.empty()

# ── State machine ─────────────────────────────────────────────────────────────
if reset_btn:
    for key in ["lattice", "phase", "sweep_count", "equil_done", "snapshots"]:
        del st.session_state[key]
    init_state()
    st.rerun()

if run_btn and ss.phase in ("idle", "paused", "done"):
    ss.Ny, ss.Nx = Ny, Nx
    ss.T, ss.E = T_val, E_val
    ss.target_sweeps = int(target)
    if ss.phase == "idle" or ss.lattice is None:
        ss.lattice = init_lattice(Ny, Nx)
        ss.sweep_count = 0
        ss.equil_done = False
        ss.snapshots = []
        ss.phase = "equilibrating"
    else:
        ss.phase = "running"
    st.rerun()

if pause_btn and ss.phase == "running":
    ss.phase = "paused"
    st.rerun()

# ── Render current state ───────────────────────────────────────────────────────
def display_current():
    if ss.lattice is not None:
        lattice_placeholder.plotly_chart(render_lattice(ss.lattice),
                                         use_container_width=True, key="lattice_chart")
        sf = structure_factor(ss.lattice)
        sf_placeholder.plotly_chart(render_sf(sf),
                                    use_container_width=True, key="sf_chart")
    else:
        lattice_placeholder.info("Configure parameters and press ▶ Run to start.")
        sf_placeholder.info("Structure factor will appear here.")

phase_labels = {
    "idle": "⬜ Ready",
    "equilibrating": "🔄 Equilibrating (E=0)…",
    "running": "▶ Running",
    "paused": "⏸ Paused",
    "done": "✅ Complete",
}
status_placeholder.markdown(f"**Status:** {phase_labels.get(ss.phase, ss.phase)}")
sweep_placeholder.markdown(f"**Sweeps:** {ss.sweep_count:,} / {ss.target_sweeps:,}" if ss.phase not in ("idle", "equilibrating") else "")
snap_placeholder.markdown(f"**Snapshots:** {len(ss.snapshots)}" if ss.snapshots else "")

# ── Simulation loop ────────────────────────────────────────────────────────────
if ss.phase == "equilibrating":
    status_placeholder.markdown("**Status:** 🔄 Equilibrating (E=0, 100k sweeps)…")
    ss.lattice = run_sweeps(ss.lattice, ss.Ny, ss.Nx, 1.0 / ss.T, 0.0, EQUIL_SWEEPS)
    ss.equil_done = True
    ss.phase = "running"
    display_current()
    st.rerun()

if ss.phase == "running":
    beta = 1.0 / ss.T
    remaining = ss.target_sweeps - ss.sweep_count

    if remaining <= 0:
        ss.phase = "done"
        display_current()
        st.rerun()

    chunk = min(DISPLAY_EVERY, remaining)
    ss.lattice = run_sweeps(ss.lattice, ss.Ny, ss.Nx, beta, ss.E, chunk)
    ss.sweep_count += chunk

    # Save snapshot every 100k sweeps
    prev = ss.sweep_count - chunk
    for snap_idx in range((prev // SNAPSHOT_INTERVAL) + 1, (ss.sweep_count // SNAPSHOT_INTERVAL) + 1):
        snap_sweep = snap_idx * SNAPSHOT_INTERVAL
        if snap_sweep <= ss.sweep_count:
            ss.snapshots.append({
                "sweep": snap_sweep,
                "lattice": ss.lattice.copy(),
                "sf": structure_factor(ss.lattice),
            })

    display_current()
    status_placeholder.markdown(f"**Status:** {phase_labels['running']}")
    sweep_placeholder.markdown(f"**Sweeps:** {ss.sweep_count:,} / {ss.target_sweeps:,}")
    snap_placeholder.markdown(f"**Snapshots:** {len(ss.snapshots)}")
    time.sleep(0.01)
    st.rerun()

else:
    display_current()

# ── Download button (always shown when snapshots exist) ───────────────────────
if ss.snapshots:
    buf = io.BytesIO()
    with zipfile.ZipFile(buf, "w", zipfile.ZIP_DEFLATED) as zf:
        for snap in ss.snapshots:
            s = snap["sweep"]
            # Lattice as CSV
            csv_bytes = io.BytesIO()
            np.savetxt(csv_bytes, snap["lattice"], fmt="%d", delimiter=",")
            zf.writestr(f"lattice_sweep{s:07d}.csv", csv_bytes.getvalue())
            # SF as npy
            sf_bytes = io.BytesIO()
            np.save(sf_bytes, snap["sf"])
            zf.writestr(f"sf_sweep{s:07d}.npy", sf_bytes.getvalue())
        # Metadata
        meta_lines = [
            f"Lattice: {ss.Ny} x {ss.Nx}",
            f"Temperature: {ss.T}",
            f"E-field: {ss.E}",
            f"Production sweeps: {ss.sweep_count}",
            f"Snapshots: {len(ss.snapshots)}",
            f"Snapshot interval: {SNAPSHOT_INTERVAL} sweeps",
            "",
            "Files:",
            "  lattice_sweep*.csv  — charge configuration (0/1 per site)",
            "  sf_sweep*.npy       — structure factor array (numpy float64)",
            "",
            "DISCLAIMER: Snapshots are collected at fixed 100k-sweep intervals.",
            "For rigorous analysis, use autocorrelation to find the correlation",
            "time and collect truly independent samples.",
        ]
        zf.writestr("README.txt", "\n".join(meta_lines))
    buf.seek(0)
    download_placeholder.download_button(
        "⬇ Download zip",
        data=buf,
        file_name=f"dds_{ss.Ny}x{ss.Nx}_T{ss.T}_E{ss.E}.zip",
        mime="application/zip",
        use_container_width=True,
    )
