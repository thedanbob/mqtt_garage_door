Import('env')

# Only compress OTA updates
if env["UPLOAD_PROTOCOL"] != "espota": exit

import os
import shutil
import pathlib
import gzip

# Compress .bin file to .bin.gz
def gzip_firmware(source, target, env):
    bin_file = pathlib.Path(target[0].get_abspath())
    gzip_file = bin_file.with_suffix(".bin.gz")

    # Remove old gzip file
    if os.path.isfile(gzip_file):
        os.remove(gzip_file)

    # Write gzip firmware file
    with open(bin_file, "rb") as fp:
        with gzip.open(gzip_file, "wb") as f:
            shutil.copyfileobj(fp, f)

    ORG_FIRMWARE_SIZE = bin_file.stat().st_size
    GZ_FIRMWARE_SIZE = gzip_file.stat().st_size

    print(
        "Compressed firmware to {:.0f}% of original (was {} B, now {} B)".format(
            (GZ_FIRMWARE_SIZE / ORG_FIRMWARE_SIZE) * 100,
            ORG_FIRMWARE_SIZE,
            GZ_FIRMWARE_SIZE,
        )
    )

# Upload .bin.gz file
def set_gzip_source(source, target, env):
    env.Replace(UPLOADCMD=env["UPLOADCMD"].replace("$SOURCE", "$SOURCE\.gz"))

# Modify build environment
env.Append(CPPDEFINES=["ATOMIC_FS_UPDATE"])
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", [gzip_firmware])
env.AddPreAction("upload", set_gzip_source)
