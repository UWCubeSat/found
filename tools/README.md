# FOUND Tools

This folder hosts tools that can be used with FOUND for analysis or artificial generation.

# Usage

Tool Dependencies:
- Python
- Highly Recommended: Conda/Miniconda

Each tool uses a different set of dependencies, so its in your best interest to use `conda`. To use each tool:
1. `cd` into the respective folder
2. Create a virtual environment based on the `environment.yml` listed (`conda env create -f environment.yml`)
3. `cd` back into the `found` directory
4. Activate the environment for that tool (`conda activate <environment-name>`)
5. Invoke the tool (`python -m tools.<tool-name> [arguments for tool]`)

Each tool uses a different set of dependencies, so it's best to use micromamba. In the dev container, environments are created automatically for each tool (but not activated). To use a tool:

1. `cd` into the respective folder
2. (Environments are already created by the container)
3. Activate the environment for that tool:<br>
   `micromamba activate <environment-name>`
4. Invoke the tool:<br>
   `python -m tools.<tool-name> [arguments for tool]`

# Current Capabilities

For more information, visit each tool's folder for its `README.md`

## 1. Generator

1. Tool Name: `generator`
2. Enviornment Name: `generator_env`

Generates artificial images of Earth from a given position and orientation