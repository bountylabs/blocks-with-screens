const { Duplex } = require("stream");
const fs = require("fs");
const path = require("path");

const memory = {};

function bufferToStream(buffer) {
  const stream = new Duplex();
  stream.push(buffer);
  stream.push(null);
  return stream;
}

// http://localhost:3000/api/video?mode=write&block=56&data=0001
// http://localhost:3000/api/video?mode=read&block=56

// Working with Octet Streams in Javascript
// https://medium.com/@julientregoat/working-with-octet-streams-in-javascript-d43d81ad7f47

module.exports = (req, res) => {
  const timeStart = Date.now();

  const { mode, block } = req.query;

  // console.info(req.url, { block, mode });

  switch (mode) {
    case "write": {
      const data = [];

      req
        .on("data", chunk => {
          // console.info('chunk', chunk);
          data.push(chunk);
          console.info("chunk");
        })
        .on("end", () => {
          //at this point data is an array of Buffers
          //so Buffer.concat() can make us a new Buffer
          //of all of them together
          const buffer = Buffer.concat(data);

          console.info("writing to memory at", block);
          memory[block] = buffer;

          const timeEnd = Date.now();
          const response = {
            success: true,
            time: timeEnd - timeStart
          };

          console.info(req.url, response);
          return res.json(response);
        });
      break;
    }
    case "read":
    default: {
      console.info("memory at ", block, !!memory[block]);
      if (memory[block]) {
        // Content-type is very interesting part that guarantee that
        // Web browser will handle response in an appropriate manner.
        res.writeHead(200, {
          "Content-Type": "application/octet-stream",
          "transfer-encoding": ""
        });

        bufferToStream(memory[block]).pipe(res);

        const timeEnd = Date.now();
        console.info(req.url, timeEnd - timeStart);
        return;
      }

      return res.status(404).json({ success: false });
    }
  }
};
