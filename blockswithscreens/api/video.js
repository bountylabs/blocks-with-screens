const fs = require('fs');
const memory = {};

const TMP_DIR = '/tmp/blockswithscreens';

function ensureDirSync(dirpath) {
  try {
    fs.mkdirSync(dirpath, { recursive: true });
  } catch (err) {
    if (err.code !== 'EEXIST') throw err;
  }
}

ensureDirSync(TMP_DIR);

module.exports = (req, res) => {
  const { mode, block } = req.query;

  let data;

  switch (mode) {
    case 'write': {
      const { data } = req.query;
      memory[block] = data;
      fs.writeFile(`${TMP_DIR}/${block}`, data, err => {
        if (err) {
          return console.log(err);
        }

        console.info('The file was saved!');
      });

      return res.json({
        success: true,
      });
    }
    case 'read':
    default: {
      const data = memory[block];

      return res.json({
        block,
        data,
      });
    }
  }
};
