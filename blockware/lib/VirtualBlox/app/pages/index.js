import * as React from "react";
import Head from "next/head";
import Image from "next/image";

import { Accelerometer } from "../src/components/Accelerometer";

import styles from "../styles/Home.module.css";

export default function Home() {
  const canvasRef = React.useRef();
  const [jsSource, set_jsSource] = React.useState(undefined);

  React.useEffect(() => {
    set_jsSource("pixel-dust/pixel-dust.js");
  }, []);

  React.useEffect(() => {
    if (!canvasRef.current) {
      return;
    }

    // As a default initial behavior, pop up an alert when webgl context is lost. To make your
    // application robust, you may want to override this behavior before shipping!
    // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
    canvasRef.current.addEventListener(
      "webglcontextlost",
      function (e) {
        alert("WebGL context lost. You will need to reload the page.");
        e.preventDefault();
      },
      false
    );
  }, []);

  React.useEffect(() => {
    if (!window) {
      return;
    }

    let isSetup = false;

    if (!window.Module) {
      isSetup = true;
      window.Module = {
        preRun: [],
        postRun: [],
        totalDependencies: 0,
      };
    }

    const statusElement = document.getElementById("status");
    const progressElement = document.getElementById("progress");
    const spinnerElement = document.getElementById("spinner");

    window.Module.canvas = canvasRef.current;
    window.Module.print = (function () {
      const outputElement = document.getElementById("output");
      if (outputElement) outputElement.value = ""; // clear browser cache
      return function (text) {
        if (arguments.length > 1)
          text = Array.prototype.slice.call(arguments).join(" ");
        // These replacements are necessary if you render to raw HTML
        //text = text.replace(/&/g, "&amp;");
        //text = text.replace(/</g, "&lt;");
        //text = text.replace(/>/g, "&gt;");
        //text = text.replace('\n', '<br>', 'g');
        console.log(text);
        if (outputElement) {
          outputElement.value += text + "\n";
          outputElement.scrollTop = outputElement.scrollHeight; // focus on bottom
        }
      };
    })();

    window.Module.printErr = function (...args) {
      console.error("[ERROR]", ...args);
      window.Module.print("[ERROR]", ...args);
    };

    window.Module.setStatus = function (text) {
      if (!Module.setStatus.last)
        Module.setStatus.last = { time: Date.now(), text: "" };
      if (text === Module.setStatus.last.text) return;
      var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
      var now = Date.now();
      if (m && now - Module.setStatus.last.time < 30) return; // if this is a progress update, skip it if too soon
      Module.setStatus.last.time = now;
      Module.setStatus.last.text = text;
      if (m) {
        text = m[1];
        progressElement.value = parseInt(m[2]) * 100;
        progressElement.max = parseInt(m[4]) * 100;
        progressElement.hidden = false;
        spinnerElement.hidden = false;
      } else {
        progressElement.value = null;
        progressElement.max = null;
        progressElement.hidden = true;
        if (!text) spinnerElement.style.display = "none";
      }
      statusElement.innerHTML = text;
    };

    window.Module.monitorRunDependencies = function (left) {
      this.totalDependencies = Math.max(this.totalDependencies, left);
      Module.setStatus(
        left
          ? "Preparing... (" +
              (this.totalDependencies - left) +
              "/" +
              this.totalDependencies +
              ")"
          : "All downloads complete."
      );
    };

    if (isSetup) {
      console.debug("One-time setup for Module.");

      Module.setStatus("Downloading...");

      window.onerror = function (event) {
        // TODO: do not warn on ok events like simulating an infinite loop or exitStatus
        Module.setStatus("Exception thrown, see JavaScript console");
        spinnerElement.style.display = "none";
        Module.setStatus = function (text) {
          if (text) Module.printErr("[post-exception status] " + text);
        };
      };
    } else {
      console.debug("Hot reload, skipping setup.");
    }
  });

  React.useEffect(() => {}, []);

  return (
    <React.Fragment>
      <Head>
        <meta charSet="utf-8" />
        <meta httpEquiv="Content-Type" content="text/html; charset=utf-8" />
        <title>Emscripten-Generated Code</title>
        <meta
          name="description"
          content="Generated with 💖 by BlocksWithScreens"
        />
        <link rel="icon" href="/favicon.ico" />
      </Head>

      <div className={styles.container}>
        <div className="spinner" id="spinner"></div>
        <div className="emscripten" id="status">
          Downloading...
        </div>

        <div className="emscripten">
          <progress value="0" max="100" id="progress" hidden={1}></progress>
        </div>

        <div className="emscripten_border">
          <canvas
            id="canvas"
            className="emscripten"
            ref={canvasRef}
            onContextMenu={(e) => e.preventDefault()}
            tabIndex={-1}
          ></canvas>
        </div>

        <div
          id="controls"
          style={{ display: "flex", flexDirection: "column", padding: 16 }}
        >
          <div
            id="row-controls"
            style={{ display: "flex", flexDirection: "row" }}
          >
            <span>
              <input type="checkbox" id="resize" />
              <span>&nbsp;</span>
              Resize canvas
            </span>
            <span>&nbsp;&nbsp;&nbsp;</span>
            <span>
              <input type="checkbox" id="pointerLock" />
              <span>&nbsp;</span>
              Lock/hide mouse pointer
            </span>
            <span>&nbsp;&nbsp;&nbsp;</span>
            <span>
              <input
                type="button"
                value="Fullscreen"
                onClick={() => {
                  Module.requestFullscreen(
                    document.getElementById("pointerLock").checked,
                    document.getElementById("resize").checked
                  );
                }}
              />
            </span>
          </div>

          <Spacer />

          <Accelerometer />
        </div>

        <textarea id="output" rows="8"></textarea>
      </div>
      <script async type="text/javascript" src={jsSource}></script>
    </React.Fragment>
  );
}

function Spacer() {
  return <div className={styles.spacer} />;
}
