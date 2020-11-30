import * as React from "react";
import * as THREE from "three";
import { Canvas, useFrame } from "react-three-fiber";
import { useFBX } from "@react-three/drei/useFBX";
import { useGLTF } from "@react-three/drei/useGLTF";
import { a, useSpring } from "@react-spring/three";
import { BlendFunction } from "postprocessing";

import {
  EffectComposer,
  DepthOfField,
  Bloom,
  Noise,
  Vignette,
} from "@react-three/postprocessing";

import WebGLStats from "./WebGLStats";
import WebGLOrbitControls from "./WebGLOrbitControls";

const MODEL_SIZE = 57.55298173283222;

export default function WebGLDemo() {
  return (
    <Canvas
      style={{ background: "white" }}
      gl={{ antialias: true }}
      invalidateFrameloop={false}
      pixelRatio={window.devicePixelRatio}
      onCreated={({ gl }) => {
        gl.shadowMap.enabled = true;
        gl.shadowMap.type = THREE.PCFSoftShadowMap;
      }}
    >
      <ambientLight intensity={0.75} penumbra={1} />
      <spotLight intensity={5} position={[0, 50, 0]} penumbra={1} castShadow />
      <fog attach="fog" args={["white", 100, 250]} />

      <GentleFloatEffect>
        <BlockWithScreen />
        <VirtualBlox />
      </GentleFloatEffect>

      <mesh
        position={[0, -25, 0]}
        rotation={[-Math.PI / 2, 0, 0]}
        receiveShadow
      >
        <planeBufferGeometry args={[1000, 1000]} attach="geometry" />

        <meshPhysicalMaterial attach="material" color={"white"} />
      </mesh>

      <WebGLOrbitControls autoRotate initCameraPos={[-32, 0, 69]} />

      <EffectComposer>
        <Noise opacity={0.03} />
      </EffectComposer>

      {/* debug */}
      {/* <WebGLStats /> */}
      <gridHelper args={[100, 100]} rotation={[Math.PI / 2, 0, 0]} />
    </Canvas>
  );
}

function GentleFloatEffect({ children, ...props }) {
  // subtle floaty effect
  // create a common spring that will be used later to interpolate other values
  const [direction, set_direction] = React.useState(false);
  const duration = 5;
  const { pos } = useSpring({
    pos: direction ? [0, -2, 0] : [0, +2, 0],
    config: { duration: duration * 1000 },
  });

  // This reference will give us direct access to the mesh
  const mesh = React.useRef();

  useFrame((frameMetadata) => {
    const clockMod = frameMetadata.clock.elapsedTime % duration;
    const new_direction = clockMod < duration / 2;
    if (direction !== new_direction) {
      set_direction(new_direction);
    }
  });

  return (
    <a.mesh scale={[1, 1, 1]} position={pos} {...props}>
      {children}
    </a.mesh>
  );
}

function BlockWithScreen(props) {
  const [active, set_active] = React.useState(false);

  // This reference will give us direct access to the mesh
  const mesh = React.useRef();

  React.useEffect(() => {
    traverseMaterials(mesh.current, (material) => {
      material.wireframe = active;
    });
  }, [active]);

  function handleLoadedModel(model) {
    const box = new THREE.Box3().setFromObject(model);
    const size = box.getSize(new THREE.Vector3()).length();

    console.debug(size);

    model.position.x = (-1 * size) / 4;
    model.position.y = (-1 * size) / 4;
    model.position.z = (-1 * size) / 4;

    // console.debug("handleLoadedModel", { model, box, size });
  }

  const boxSize = 40;

  return (
    <mesh
      scale={[1, 1, 1]}
      onPointerUp={(event) => {
        // setTimeout required to prevent racing
        // without setTimeout the wireframes may flicker and reset
        setTimeout(() => set_active(!active), 0);
      }}
      {...props}
    >
      <mesh>
        <boxBufferGeometry args={[boxSize, boxSize, boxSize]} />
        <meshStandardMaterial transparent opacity={0} color={"red"} />
      </mesh>

      <mesh ref={mesh} scale={[1, 1, 1]}>
        <React.Suspense fallback={<Box scale={[25, 25, 25]} />}>
          <BlockWithScreenFBX onLoad={handleLoadedModel} />
        </React.Suspense>
      </mesh>
    </mesh>
  );
}

function BlockWithScreenFBX(props) {
  let fbx = useFBX("/block_with_screen.fbx");

  if (typeof props.onLoad === "function") {
    props.onLoad(fbx);
  }

  // IMPORTANT: ensure all geometries have valid materials for raycasting
  fbx.traverse((node) => {
    if (!node.isMesh) return;

    node.castShadow = true;
    node.receiveShadow = true;

    node.geometry.groups.forEach((group) => {
      // some groups had invalid materialIndex of -1
      // this caused raycasting to fail because it would
      // lookup material at array index of -1 (always undefined)
      if (group.materialIndex < 0) {
        group.materialIndex = 1;
      }
    });
  });

  return <primitive object={fbx} dispose={null} />;
}

function BlockWithScreenGLTF() {
  let gltf = useGLTF("/block_with_screen_gltf/model.gltf");
  return <primitive object={gltf.scene} dispose={null} />;
}

function Box(props) {
  return (
    <mesh scale={[1, 1, 1]} castShadow {...props}>
      <boxBufferGeometry args={[1, 1, 1]} />
      <meshStandardMaterial color={"gray"} />
    </mesh>
  );
}

function traverseMaterials(object, callback) {
  object.traverse((node) => {
    if (!node.isMesh) return;
    const materials = Array.isArray(node.material)
      ? node.material
      : [node.material];
    materials.forEach(callback);
  });
}

function VirtualBlox(props) {
  //create canvas for image texture
  const canvas = document.createElement("canvas");
  const g = canvas.getContext("2d");
  const canvasSize = 1024;
  canvas.width = canvas.height = canvasSize;
  g.fillStyle = "rgba(255,0,0,1)";
  g.fillRect(0, 0, canvas.width, canvas.height);
  g.fillStyle = "black";
  g.strokeRect(0, 0, canvas.width, canvas.height);

  // draw text on canvas
  const text = "VirtualBlox";
  g.font = "Bold 72px Arial";
  g.fillStyle = "white";
  g.strokeStyle = "black";
  const {
    actualBoundingBoxAscent: textHeight,
    width: textWidth,
    ...textMetrics
  } = g.measureText(text);
  const x = canvasSize / 2 - textWidth / 2;
  const y = canvasSize / 2 - textHeight / 2;
  g.fillText(text, x, y);
  g.strokeText(text, x, y);

  const boxMult = 0.5 + 1 / Math.pow(2, 3 + 1 / Math.pow(2, 1));
  const boxSize = MODEL_SIZE * boxMult;

  // canvas contents will be used for a texture
  const texture = new THREE.Texture(canvas);
  texture.needsUpdate = true;

  // console.debug({ boxMult, boxSize });

  return (
    <mesh
      {...props}
      scale={[1, 1, 1]}
      position={[-1.25, -1.25, -1 * (MODEL_SIZE / 4 + 1.8)]}
      rotation={[0, Math.PI, 0]}
    >
      <planeBufferGeometry args={[boxSize, boxSize]} attach="geometry" />
      <meshStandardMaterial attach="material" map={texture} />
    </mesh>
  );
}
