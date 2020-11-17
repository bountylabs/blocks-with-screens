// This file doesn't go through babel or webpack transformation.
// Make sure the syntax and sources this file requires are compatible with the current node version you are running
// See https://github.com/zeit/next.js/issues/1245 for discussions on Universal Webpack or universal Babel
// https://nextjs.org/docs/#custom-server-and-routing
const express = require("express");
const fs = require("fs");
const https = require("https");
const path = require("path");

const STATIC_PATH = path.join(__dirname, "../static");
const port = process.env.PORT || 3000;
const { HTTP, ON_GCLOUD } = process.env;
const app = express();

app.get("/version", (req, res, next) => {
  res.json({
    version: process.env.VERSION
  });
});

app.post("/echo", function(request, response) {
  response.send(request.body);
});

app.all("/api/video", require("./api/video"));
app.all("/api/gif", require("./api/gif"));
app.all("/api/rgb565", require("./api/rgb565"));

// do this last so we don't match too early
if (process.env.NODE_ENV !== "production" || ON_GCLOUD) {
  // mimic now.json routes config with express
  // for local development
  app.use("/static", express.static(STATIC_PATH));

  app.use("/", (req, res) =>
    res.sendFile(path.join(STATIC_PATH, "index.html"))
  );
}

// HTTPS locally when HTTP flag passed
if (process.env.NODE_ENV !== "production" && !HTTP) {
  https
    .createServer(
      {
        key: fs.readFileSync("devcerts/server.key"),
        cert: fs.readFileSync("devcerts/server.cert")
      },
      app
    )
    .listen(port, err => {
      if (err) throw err;
      console.log(`> Ready On Server https://localhost:${port}`);
    });
} else {
  // HTTP
  app.listen(port, err => {
    if (err) throw err;
    console.log(`> Ready On Server http://localhost:${port}`);
  });
}
