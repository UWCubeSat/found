# FOUND Tools
This folder hosts tools that can be used with FOUND for analysis or artificial generation.

# Usage
## Python Environment Setup
If you are using the Dev Container the Python environments are already setup so you can skip this step.  

To setup a virutal environment for a tool your going to want an environment mananger (conda, mamba, etc.) to install the dependencies. You can install `micromamba` by running:
```bash
"${SHELL}" <(curl -L micro.mamba.pm/install.sh) && /
micromamba shell init -s <your_shell> -p ~/micromamba && /
source ~/.bashrc  # or ~/.zshrc, depending on your shell
```
Once you've installed the package manager of your choice, create the virtual environments by running `setup-tools-envs.sh` in the `found` directory.

## Using Tools
1. Activate the environment for that tool (`micromamba activate <environment-name>`)
2. Invoke the tool (`python -m tools.<tool-name> [arguments for tool]`)

For more information, visit each tool's folder for its `README.md`.

# Tools
## 1. Generator
   
Generates artificial images of Earth from a given position and orientation
- Tool Name: `generator`
- Enviornment Name: `generator_env`