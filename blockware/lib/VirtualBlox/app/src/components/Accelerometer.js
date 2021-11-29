import * as React from "react";

import styles from "../../styles/Home.module.css";

const ACCELEROMETER_MAX_INT16 = 32767;

export function Accelerometer() {
  const [accelerometer, set_accelerometer] = React.useState({
    x: 0,
    y: 0,
    z: 0,
  });

  React.useEffect(() => {
    if (!window) {
      return;
    }

    if (!window.Module) {
      console.warn("⚠️ [WARNING] Module is not defined.");
      return;
    }

    if (typeof window.Module.updateAccelerometer !== "function") {
      console.warn("⚠️ [WARNING] Module.updateAccelerometer is not defined.");
      return;
    }

    // call c function to keep accelerometer up to date
    window.Module.updateAccelerometer(
      accelerometer.x,
      accelerometer.y,
      accelerometer.z
    );
  }, [accelerometer]);

  return (
    <div className={styles.slidersContainer}>
      <span>Accelerometer</span>
      <button onClick={() => set_accelerometer({ x: 0, y: 0, z: 0 })}>
        Zero
      </button>
      {["x", "y", "z"].map((axis) => {
        return (
          <div key={axis} className={styles.slider}>
            <span
              className="label"
              style={{ alignSelf: "flex-start", fontWeight: 600 }}
            >
              {axis}
            </span>
            <input
              type="range"
              min={-1 * ACCELEROMETER_MAX_INT16}
              max={ACCELEROMETER_MAX_INT16}
              value={accelerometer[axis]}
              onChange={(e) => {
                set_accelerometer({
                  ...accelerometer,
                  [axis]: parseInt(e.target.value, 10),
                });
              }}
            />
            <span style={{ alignSelf: "flex-end", fontFamily: "monospace" }}>
              {accelerometer[axis]}
            </span>
          </div>
        );
      })}
    </div>
  );
}
