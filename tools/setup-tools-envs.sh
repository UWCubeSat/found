#!/usr/bin/env bash
set -euo pipefail

TOOLS_DIR="/workspace/tools"


# Detect available environment manager
detect_env_mgr() {
  if command -v conda &> /dev/null; then
    echo "conda"
  elif command -v miniconda &> /dev/null; then
    echo "miniconda"
  elif command -v mamba &> /dev/null; then
    echo "mamba"
  elif command -v micromamba &> /dev/null; then
    echo "micromamba"
  elif [[ -x "$MICROMAMBA_BIN" ]]; then
    echo "$MICROMAMBA_BIN"
  else
    echo ""
  fi
}

# Ensure we have a working environment manager
ENV_MGR=$(detect_env_mgr)
if [[ -z "$ENV_MGR" ]]; then
  echo >&2 "No compatible environment manager found on your system."
  echo >&2 "Please install one of the following before running this script:"
  echo >&2 "  - conda      → https://docs.conda.io/en/latest/miniconda.html"
  echo >&2 "  - mamba      → https://mamba.readthedocs.io/en/latest/"
  echo >&2 "  - micromamba → https://mamba.readthedocs.io/en/latest/micromamba.html"
  echo >&2
  echo >&2 "After installing, re-run this script."
  exit 1
fi

# Setup micromamba shell if it's being used
if [[ "$ENV_MGR" == *micromamba ]]; then
  eval "$("$ENV_MGR" shell hook --shell bash)"
fi

# Process all environment.yml files under tools
find "$TOOLS_DIR" -mindepth 2 -maxdepth 2 -name environment.yml | while read -r envfile; do
  envname=$(awk '/^name:/ {print $2}' "$envfile")
  [[ -z "$envname" ]] && envname=$(basename "$(dirname "$envfile")")

  echo "Processing environment: $envname"

  if "$ENV_MGR" env list | grep -qw "$envname"; then
    echo "Updating existing environment: $envname"
    "$ENV_MGR" env update -n "$envname" -f "$envfile" -y
  else
    echo "Creating new environment: $envname"
    "$ENV_MGR" env create -n "$envname" -f "$envfile" -y
  fi
done
