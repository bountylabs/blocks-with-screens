// This file doesn't go through babel or webpack transformation.
// Make sure the syntax and sources this file requires are compatible with the current node version you are running
// See https://github.com/zeit/next.js/issues/1245 for discussions on Universal Webpack or universal Babel
// https://nextjs.org/docs/#custom-server-and-routing
const express = require("express");

const port = process.env.PORT || 3000;
const app = express();

app.use(express.json());
app.listen(port, err => {
  if (err) throw err;
  console.log(`> Ready On Server http://localhost:${port}`);
});

app.use(express.static("static"));

app.get("/version", (req, res, next) => {
  res.json({
    version: process.env.VERSION
  });
});

app.post("/echo", function(request, response) {
  response.send(request.body);
});

app.all("/api/video", require("./api/video"));
