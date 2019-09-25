// This file doesn't go through babel or webpack transformation.
// Make sure the syntax and sources this file requires are compatible with the current node version you are running
// See https://github.com/zeit/next.js/issues/1245 for discussions on Universal Webpack or universal Babel
// https://nextjs.org/docs/#custom-server-and-routing
const http = require('http');
const next = require('next');
const expressApp = require('express')();
const server = http.Server(expressApp);

const config = require('./config');

const dev = process.env.NODE_ENV !== 'production';
const nextApp = next({ dev });
const handle = nextApp.getRequestHandler();

// Ping self every 2 minutes
setInterval(() => {
  console.info('Pinging localhost to keep server warm...');
  http.get(config.LOCALHOST);
}, 1000 * 60 * 2);

nextApp.prepare().then(() => {
  expressApp.all('/api/video', require('./api/video'));

  expressApp.get('*', (req, res) => {
    try {
      return handle(req, res);
    } catch (err) {
      console.error('server-catch', err);
      res.status(500).send(err.stack);
    }
  });

  // Setup socket server
  server.listen(config.PORT, err => {
    if (err) throw err;

    console.log(`> Ready on ${config.LOCALHOST}`);
  });
});
