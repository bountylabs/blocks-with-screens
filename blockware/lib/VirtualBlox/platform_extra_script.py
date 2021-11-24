Import("env")

# Override the "upload" target to run the local blockware
env.AddCustomTarget(
    "upload", "$BUILD_DIR/${PROGNAME}", "./.pio/build/VirtualBlox/program"
)
