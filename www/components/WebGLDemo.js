import * as React from "react";
import * as THREE from "three";
import { Canvas, useFrame } from "react-three-fiber";
import { useFBX } from "@react-three/drei/useFBX";
import { useGLTF } from "@react-three/drei/useGLTF";

import WebGLStats from "./WebGLStats";
import WebGLOrbitControls from "./WebGLOrbitControls";

export default function WebGLDemo() {
  return (
    <Canvas
      style={{ background: "rgba(0,0,0,0.8)" }}
      gl={{ antialias: true }}
      invalidateFrameloop={false}
      pixelRatio={window.devicePixelRatio}
      onCreated={({ gl }) => {
        gl.shadowMap.enabled = true;
        gl.shadowMap.type = THREE.PCFSoftShadowMap;
      }}
    >
      <ambientLight intensity={0.5} penumbra={1} />

      <BlockWithScreen />

      <WebGLOrbitControls autoRotate initCameraPos={[-32, 0, 69]} />

      {/* debug */}
      {/* <WebGLStats /> */}
      {/* <gridHelper args={[100, 100]} rotation={[Math.PI / 2, 0, 0]} /> */}
    </Canvas>
  );
}

function BlockWithScreen(props) {
  const [hovered, set_hovered] = React.useState(false);

  // This reference will give us direct access to the mesh
  const mesh = React.useRef();

  // // Rotate mesh every frame, this is outside of React without overhead
  // useFrame(() => (mesh.current.rotation.z += 0.01));

  useFrame(() => {
    // Hover effect on model
  });

  React.useEffect(() => {
    traverseMaterials(mesh.current, (material) => {
      material.wireframe = hovered;
    });
  }, [hovered]);

  function handleLoadedModel(model) {
    const box = new THREE.Box3().setFromObject(model);
    const size = box.getSize(new THREE.Vector3()).length();

    model.position.x = (-1 * size) / 4;
    model.position.y = (-1 * size) / 4;
    model.position.z = (-1 * size) / 4;

    console.debug("handleLoadedModel", { model, box, size });
  }

  return (
    <mesh
      {...props}
      scale={[1, 1, 1]}
      onClick={(event) => set_hovered(!hovered)}
    >
      <boxBufferGeometry args={[1, 1, 1]} />
      <meshStandardMaterial color={"gray"} />
      <mesh ref={mesh} scale={[1, 1, 1]} castShadow>
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
