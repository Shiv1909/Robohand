"""
Copy build artifacts into <project>/.wokwi/<env>/ after every firmware build.

Why this exists: platformio.ini redirects the build workspace out of this
OneDrive-synced folder, but the Wokwi VS Code extension refuses any firmware
path outside the workspace ("firmware binary ... not found in workspace"). So
the artifacts have to be mirrored back inside the project.

Doing it as a post-build step rather than a one-off copy is deliberate — a
stale binary in the simulator is silently wrong, and you'd be debugging
yesterday's firmware without any error to tell you.

.wokwi/ is gitignored.
"""

import os
import shutil

Import("env")  # noqa: F821 — injected by PlatformIO

ARTIFACTS = ("firmware.bin", "bootloader.bin", "partitions.bin", "firmware.elf")


def copy_for_wokwi(source, target, env):
    build_dir = env.subst("$BUILD_DIR")
    dest = os.path.join(env.subst("$PROJECT_DIR"), ".wokwi", env.subst("$PIOENV"))
    os.makedirs(dest, exist_ok=True)

    for name in ARTIFACTS:
        src = os.path.join(build_dir, name)
        if os.path.exists(src):
            shutil.copy2(src, os.path.join(dest, name))

    print("Wokwi: mirrored firmware to %s" % dest)


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", copy_for_wokwi)  # noqa: F821
