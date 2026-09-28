from pathlib import Path
import subprocess
import os
#create path objects for directory that have tests and directory that contains exe of rain (../build/rain)
test_dir = Path("cases")
bin_dir = Path(__file__).resolve().parent.parent / "build" / "rain"
#used by subprocess to as arguments to launc everything
launch_com = [bin_dir, "build"]
#copy all of environment variables and add package path then path to process
env = os.environ.copy()
env["RAIN_PACKAGE_PATH"] = str(Path(__file__).resolve().parent / "artifacts" / "packages")
env["RAIN_ARTIFACT_PATH"] = str(Path(__file__).resolve().parent / "artifacts" / "libraries")

def run_command(cwd):
    print(f"Running: {cwd.name}")

    res = subprocess.run(launch_com, cwd=cwd, capture_output=True, text=True, env=env)
    if res.returncode == 0:
        print(f"{res.stdout}")
    else:
        print(f"Error running test\n{res.stdout}\n{res.stderr}")

def main():
    for i in test_dir.iterdir():
        run_command(i)

if __name__ == "__main__":
    main()
