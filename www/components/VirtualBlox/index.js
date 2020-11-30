import * as React from "react";
import * as THREE from "three";
import { useFrame } from "react-three-fiber";

const CANVAS_SIZE = 2048;

export default function VirtualBlox(props) {
  const { modelSize, boxSize } = props;

  // console.debug({ modelSize, boxSize });

  const materialRef = React.useRef();

  //create canvas for image texture
  const canvas = document.createElement("canvas");
  const g = canvas.getContext("2d");

  canvas.width = canvas.height = CANVAS_SIZE;

  // intialize screen as black
  g.fillStyle = "black";
  g.fillRect(0, 0, canvas.width, canvas.height);
  // g.fillStyle = "white";
  // g.strokeRect(0, 0, canvas.width, canvas.height);

  // // draw text on canvas
  // const text = "VirtualBlox";
  // g.font = "Bold 72px Arial";
  // g.fillStyle = "white";
  // g.strokeStyle = "black";
  // const {
  //   actualBoundingBoxAscent: textHeight,
  //   width: textWidth,
  //   ...textMetrics
  // } = g.measureText(text);
  // const x = CANVAS_SIZE / 2 - textWidth / 2;
  // const y = CANVAS_SIZE / 2 - textHeight / 2;
  // g.fillText(text, x, y);
  // g.strokeText(text, x, y);

  // canvas contents will be used for a texture
  useFrame(() => {
    const texture = new THREE.Texture(canvas);
    texture.needsUpdate = true;
    materialRef.current.map = texture;
  });

  if (typeof window.Module === "undefined") {
    window.Module = {};
  }
  window.Module["VirtualBloxCanvas"] = canvas;
  require("./curvy-snake.js");

  return (
    <mesh
      {...props}
      scale={[1, 1, 1]}
      position={[-1.25, -1.25, -1 * (modelSize / 4 + 1.8)]}
      rotation={[0, Math.PI, 0]}
    >
      <planeBufferGeometry args={[boxSize, boxSize]} attach="geometry" />
      <meshStandardMaterial ref={materialRef} attach="material" />
    </mesh>
  );
}
