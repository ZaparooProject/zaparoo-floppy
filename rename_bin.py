import os
import shutil
Import("env")

def post_build_action(source, target, env):
    dest_dir = os.path.join(env.get("PROJECT_DIR"), "bin")
    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)
    env_name = env.get("PIOENV")
    new_filename = f"firmware_{env_name}.bin"
    dest_path = os.path.join(dest_dir, new_filename)
    print(f"Copying {target[0]} to {dest_path}")
    shutil.copyfile(str(target[0]), dest_path)

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", post_build_action)