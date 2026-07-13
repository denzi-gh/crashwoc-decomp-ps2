# Reproducible build/analysis environment for crashwoc-decomp-ps2.
#
# Pins the Python runtime (3.12, per the plan) and the host packages needed to
# run the EE GCC cross-compiler and PS2 binutils. It deliberately does NOT bake
# in the matching toolchain or any game files: those are user-supplied and are
# never committed or published. Install them at runtime with:
#
#   python tools/setup_toolchain.py --download        # fills URLs when locked
#   python tools/setup_toolchain.py --record          # trust local archives
#
# Build:  docker build -f Containerfile -t crashwoc-decomp .
# Run:    docker run --rm -it -v "$PWD:/work" crashwoc-decomp
#
# Podman works identically (podman build/run).

# Trixie (glibc 2.38+) is required by the decompals PS2 binutils; the EE GCC
# 2.9 binaries are 32-bit x86, so i386 multilib is added for them. One image
# thus runs both the matching compiler and the reconstruction binutils.
#
# Pinned by immutable digest so the base is reproducible (the tag is kept for
# readability only; the digest is authoritative). Tag: python:3.12-slim-trixie.
# To update, see "Container image pinning" in docs/pipeline.md.
FROM python:3.12-slim-trixie@sha256:423ed6ab25b1921a477529254bfeeabf5855151dc2c3141699a1bfc852199fbf

# The EE GCC 2.9-ee-991111-01 binaries are 32-bit x86 ELF executables, so the
# host needs i386 multilib support to run them.
RUN dpkg --add-architecture i386 \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates \
        curl \
        git \
        make \
        ninja-build \
        xz-utils \
        libc6:i386 \
        libstdc++6:i386 \
        zlib1g:i386 \
    && rm -rf /var/lib/apt/lists/*

# uv manages the pinned Python tooling (splat, etc.) reproducibly. Pinned to a
# concrete version by immutable digest (tag kept for readability). To update,
# see "Container image pinning" in docs/pipeline.md.
COPY --from=ghcr.io/astral-sh/uv:0.11.27@sha256:4d01caf3b22dfd11003455e2e68153da08c4ee1fa54fdbd166c6282d22693419 /uv /usr/local/bin/uv

# Bake the pinned disassembler into the image. Only requirements.txt is copied
# in -- no project sources or game files -- so `configure.py` finds splat at the
# locked version without a network fetch at run time.
COPY requirements.txt /tmp/requirements.txt
RUN uv pip install --system -r /tmp/requirements.txt && rm /tmp/requirements.txt

WORKDIR /work

# The repository is expected to be bind-mounted at /work at run time. The image
# stays free of project sources and target binaries by design.
CMD ["bash"]
