import * as React from "react";
import { extend } from "react-three-fiber";
import { useFrame, useThree } from "react-three-fiber";

export default function WebGLOrbitControls(props) {
  const {
    OrbitControls,
  } = require("three/examples/jsm/controls/OrbitControls");

  extend({ OrbitControls });

  const ref = React.useRef();
  const { camera, gl, invalidate } = useThree();

  useFrame(() => {
    // console.debug("WebGLOrbitControls", ref.current.object);
    ref.current.update();
  });

  React.useLayoutEffect(() => {
    if (Array.isArray(props.initCameraPos)) {
      camera.position.set(...props.initCameraPos);
    }
  });

  return (
    <orbitControls
      {...props}
      ref={ref}
      args={[camera, gl.domElement]}
      enableDamping
      screenSpacePanning
      // minPolarAngle={Math.PI / 3}
      // maxPolarAngle={Math.PI / 2}
    />
  );
}
