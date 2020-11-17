# Home Assistant Deplay

An example of showing some Home Assistant temperature, humidity, and absolute humidity states on the block. Make sure to update your secrets.h and adapt the program to your Home Assistant entities!

Warning: These OLED displays can experience burn-in. If you leave your block on nonstop with similar text, the ghost of that text will be visible when you do solid colors on the screen later.

## Configuring

Create a `secrets.h` file based on `secrets.h.example` that contains your WiFi credentials, and your Home Assistant information (token, IP, port).
