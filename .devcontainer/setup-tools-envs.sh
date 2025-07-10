#!/usr/bin/env bash
set -e

TOOLS_DIR="/workspace/tools"
MAMBA_ROOT_PREFIX="/workspace/.mamba"
export MAMBA_ROOT_PREFIX

# Find all environment.yml files under tools/
find "$TOOLS_DIR" -mindepth 2 -maxdepth 2 -name environment.yml | while read -r envfile; do
  # Extract environment name from environment.yml
  envname=$(awk '/^name:/ {print $2}' "$envfile")
  if [ -z "$envname" ]; then
    envname=$(basename "$(dirname "$envfile")")
  fi
  echo "Creating micromamba env: $envname"
  micromamba env create -f "$envfile" -y -n "$envname" || true
done