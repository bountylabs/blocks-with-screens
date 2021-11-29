const chalk = require("chalk");
const fs = require("fs");
const path = require("path");
const { spawn, execSync } = require("child_process");

// Example
// node ./scripts/watch.js pixel-dust

const BLOCKWARE_ROOT = path.resolve(__dirname, "../../../");
const VIRTUAL_BLOX_BUILD_DIR = path.resolve(__dirname, "../build");
const VIRTUAL_BLOX_APP_PUBLIC_DIR = path.resolve(__dirname, "../app/public");
const [, , BLOCKWARE_NAME] = process.argv;

const BLOCKWARE_DIR = path.resolve(BLOCKWARE_ROOT, BLOCKWARE_NAME);
const MAKE_TARGET = `${BLOCKWARE_NAME}.html`;

// ensure `BLOCKWARE_DIR` exists
if (!fs.existsSync(BLOCKWARE_DIR)) {
  const name = bracket(BLOCKWARE_NAME);
  const dir = bracket(BLOCKWARE_DIR);
  console.error(error(`\n❌ ${name} does not exist (${dir}).`));
  process.exit(1);
}

// kick start app
console.log(debug(`\n> 🚀 Starting Virtual Blox App ...`));
spawn("node", [path.resolve(__dirname, "./dev.js")], { stdio: "inherit" });

console.log(
  debug(`\n> 👀 Watching ${bracket(BLOCKWARE_DIR)} for file changes ...`)
);

const DELAY_MAKE_MS = 2 * 1000;

const handleWatch = debounce((event, filename) => {
  if (filename) {
    console.log(alert(`\n  🚨 ${bracket(filename)} file changed.`));
    // spawn("make", [MAKE_TARGET], { stdio: "inherit" });
    execSync(`make ${MAKE_TARGET}`, { stdio: "inherit" });
    const build = path.resolve(VIRTUAL_BLOX_BUILD_DIR, BLOCKWARE_NAME);
    execSync(`cp -r ${build} ${VIRTUAL_BLOX_APP_PUBLIC_DIR}`);
  }
}, DELAY_MAKE_MS);

// listen for file changes
fs.watch(BLOCKWARE_DIR, { recursive: true }, handleWatch);

function debounce(callback, timeoutMs) {
  let timeoutId;

  return function trigger(...args) {
    clearTimeout(timeoutId);
    timeoutId = setTimeout(() => callback(...args), timeoutMs);
  };
}

function bracket(text) {
  return chalk.bold.yellow(`[${chalk.blue(text)}]`);
}

function debug(...args) {
  return chalk.gray(args.join(" "));
}

function alert(...args) {
  return chalk.magentaBright(args.join(" "));
}

function error(...args) {
  return chalk.redBright(args.join(" "));
}
