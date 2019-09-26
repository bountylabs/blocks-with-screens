// This file doesn't go through babel or webpack transformation.
// Make sure the syntax and sources this file requires are compatible with the current node version you are running
// See https://github.com/zeit/next.js/issues/1245 for discussions on Universal Webpack or universal Babel
// https://nextjs.org/docs/#custom-server-and-routing
const express = require("express");
const path = require("path");

const STATIC_PATH = path.join(__dirname, "../static");
const port = process.env.PORT || 3000;
const app = express();

app.listen(port, err => {
  if (err) throw err;
  console.log(`> Ready On Server http://localhost:${port}`);
});

if (process.env.NODE_ENV !== "production") {
  // mimic now.json routes config with express
  // for local development
  app.use("/$", (req, res) =>
    res.sendFile(path.join(STATIC_PATH, "index.html"))
  );
  console.info({ STATIC_PATH });
  app.use("/static", express.static(STATIC_PATH));
}

app.get("/version", (req, res, next) => {
  res.json({
    version: process.env.VERSION
  });
});

app.post("/echo", function(request, response) {
  response.send(request.body);
});

app.all("/api/video", require("./api/video"));
