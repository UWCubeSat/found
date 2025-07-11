# FOUND Tools

This folder hosts tools that can be used with FOUND for analysis or artificial generation.

# Python Environment
Each tool uses a different set of dependencies, so its in your best interest to use an environment manager (we use `micromamba`). The Dev Container automatically sets up each tools environment.

## Manual Setup
To setup a virutal environment for a tool your going to need an environment mananger (conda, mamba, etc.). Once you've installed the package manager of your choie, create a virtual environment based on the `environment.yml` listed:
<div align="center">

`micromamba env create -f tools/<tool-name>/environment.yml`

</div>

# Tools
### 1. Generator
   
Generates artificial images of Earth from a given position and orientation
- Tool Name: `generator`
- Enviornment Name: `generator_env`

## Usage
1. Activate the environment for that tool (`micromamba activate <environment-name>`)
2. Invoke the tool (`python -m tools.<tool-name> [arguments for tool]`)


For more information, visit each tool's folder for its `README.md`.
## 1. Generator

1. Tool Name: `generator`
2. Enviornment Name: `generator_env`

Generates artificial images of Earth from a given position and orientation