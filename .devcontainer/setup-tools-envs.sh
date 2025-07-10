#!/usr/bin/env bash
set -e

TOOLS_DIR="/workspace/tools"
export MAMBA_ROOT_PREFIX

# Find all environment.yml files under tools/
find "$TOOLS_DIR" -mindepth 2 -maxdepth 2 -name environment.yml | while read -r envfile; do
  # Extract environment name from environment.yml
  envname=$(awk '/^name:/ {print $2}' "$envfile")
  if [ -z "$envname" ]; then
    envname=$(basename "$(dirname "$envfile")")
  fi

  echo "Processing micromamba env: $envname"

  if micromamba env list | grep -qw "$envname"; then
    echo "Updating existing micromamba env: $envname"
    micromamba env update -n "$envname" -f "$envfile" -y
  else
    echo "Creating new micromamba env: $envname"
    micromamba env create -n "$envname" -f "$envfile" -y
  fi
done
