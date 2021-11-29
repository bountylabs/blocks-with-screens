const path = require("path");
const { spawn, execSync } = require("child_process");

const APP_DIR = path.resolve(__dirname, "../app");

// kick start app
process.chdir(APP_DIR);
run(`pwd`);
run("open http://localhost:3000");
run(`yarn dev`);

function run(command) {
  console.log("[run]", { command });
  execSync(command, { stdio: "inherit" });
}
