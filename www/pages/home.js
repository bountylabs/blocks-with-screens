import Head from "next/head";
import dynamic from "next/dynamic";
import styles from "../styles/Home.module.css";

const DynamicWebGLDemo = dynamic(() => import("../components/WebGLDemo"), {
  ssr: false,
});

export default function Home() {
  if (!process.browser) return null;

  return (
    <div className={styles.container}>
      <Head>
        <title>Blocks With Screens</title>
        <link rel="icon" href="/favicon.ico" />
      </Head>

      <div className={styles.main}>
        <DynamicWebGLDemo />
      </div>

      <footer className={styles.footer}>
        <a
          href="https://vercel.com?utm_source=create-next-app&utm_medium=default-template&utm_campaign=create-next-app"
          target="_blank"
          rel="noopener noreferrer"
        >
          Powered by <span>💖</span>
        </a>
      </footer>
    </div>
  );
}
