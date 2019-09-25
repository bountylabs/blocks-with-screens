# blockswithscreens-app

## Debugging
Simple app to access camera viewer
```
http://localhost:3000/static/index.html
```

URL containing RGB565 for Block 3
```
http://localhost:3000/api/video?mode=read&block=3
```

Convert RGB565 to PNG
```sh
ffmpeg -vcodec rawvideo -f rawvideo -pix_fmt rgb565be -s 128x128 -i ~/Downloads/b3 -f image2 -vcodec png ~/Downloads/b3.png
```
