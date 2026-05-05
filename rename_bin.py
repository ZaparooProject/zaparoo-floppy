import os
import subprocess
Import("env")

def post_build_action(source, target, env):
    build_file = str(target[0])  # firmware.bin
    dest_dir = os.path.join(env.get("PROJECT_DIR"), "bin")
    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)
    env_name = env.get("PIOENV")
    output_filename = f"firmware_{env_name}.uf2"
    output_path = os.path.join(dest_dir, output_filename)
    uf2_script = os.path.join(env.get("PROJECT_DIR"), "uf2conv.py")
    print(f"Converting {build_file} -> {output_path}")
    try:
        subprocess.check_call([
            "python",
            uf2_script,
            build_file,
            "--base", "0x4000",
            "--output", output_path
        ])
    except subprocess.CalledProcessError as e:
        print(f"UF2 conversion failed: {e}")
        raise

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", post_build_action)