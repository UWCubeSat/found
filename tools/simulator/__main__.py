import runpy

# Alias to run actual source code
runpy.run_module("simulator.src", run_name="__main__", alter_sys=True)