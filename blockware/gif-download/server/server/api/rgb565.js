const { createCanvas, loadImage } = require("canvas");

module.exports = (req, res) => {
  drawCanvas().then(() => {
    console.info("drawCanvas", "done");
    res.send(200);
  });
};

function drawCanvas() {
  return new Promise(resolve => {
    const canvas = createCanvas(200, 200);
    const ctx = canvas.getContext("2d");

    // Draw cat with lime helmet
    loadImage("http://openweathermap.org/img/wn/01n.png").then(image => {
      ctx.drawImage(image, 0, 0, 200, 200);

      console.log('<img src="' + canvas.toDataURL() + '" />');
      resolve();
    });
  });
}
