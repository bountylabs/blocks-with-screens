import * as React from "react";
import * as THREE from "three";
import { useFrame } from "react-three-fiber";

const CANVAS_SIZE = 2048;

export default function VirtualBlox(props) {
  const { modelSize, boxSize } = props;

  // console.debug({ modelSize, boxSize });

  const materialRef = React.useRef();
  const instance = React.useRef({
    //create canvas for image texture
    canvas: document.createElement("canvas"),
  });

  function initializeCanvasScreen() {
    const { canvas } = instance.current;
    const g = canvas.getContext("2d");

    const text = "VirtualBlox";
    g.font = "Bold 256px Arial";
    g.fillStyle = "orange";
    g.strokeStyle = "blue";
    const {
      actualBoundingBoxAscent: textHeight,
      width: textWidth,
      ...textMetrics
    } = g.measureText(text);
    const x = CANVAS_SIZE / 2 - textWidth / 2;
    const y = CANVAS_SIZE / 2 + textHeight / 4;
    g.fillText(text, x, y);
    g.strokeText(text, x, y);
  }

  React.useLayoutEffect(() => {
    const { canvas } = instance.current;
    const g = canvas.getContext("2d");
    canvas.width = canvas.height = CANVAS_SIZE;

    // intialize screen as black
    g.fillStyle = "black";
    g.fillRect(0, 0, canvas.width, canvas.height);
    // g.fillStyle = "white";
    // g.strokeRect(0, 0, canvas.width, canvas.height);

    initializeCanvasScreen();

    // map canvas as texture onto material
    const texture = new THREE.Texture(canvas);
    materialRef.current.map = texture;

    // load wasm demo
    if (typeof window.Module === "undefined") {
      window.Module = {};
    }
    window.Module["VirtualBloxCanvas"] = instance.current.canvas;
    require("./curvy-snake.js");
  }, []);

  // canvas contents will be used for a texture
  useFrame(() => {
    initializeCanvasScreen();
    materialRef.current.map.needsUpdate = true;
  });

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
