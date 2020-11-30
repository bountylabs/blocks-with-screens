import { Stats } from "@react-three/drei/Stats";

export default function WebGLStats(props) {
  return (
    <Stats
      showPanel={0} // Start-up panel (default=0)
      className="stats" // Optional className to add to the stats container dom element
      {...props} // All stats.js props are valid
    />
  );
}
