Import("env")

if env["PIOPLATFORM"] == "wasm":
    env.Append(
        LINKFLAGS=["-sUSE_SDL=2"],
    )
    # Override the "upload" target to start a web server hosting the html/js/wasm
    env.AddCustomTarget(
        "upload",
        "$BUILD_DIR/${PROGNAME}${PROGSUFFIX}",
        ">&2 echo 'Starting web server on http://localhost:8080 CTRL-C twice to exit...' && python3 -m http.server -d ${PROJECT_BUILD_DIR}/${PIOENV}/ 8080 & open http://localhost:8080/${PROGNAME}${PROGSUFFIX}",
    )
else:
    # Override the "upload" target to run the local blockware
    env.AddCustomTarget(
        "upload", "$BUILD_DIR/${PROGNAME}", "${PROJECT_BUILD_DIR}/${PIOENV}/${PROGNAME}"
    )
