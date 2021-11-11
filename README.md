# Blocks With Screens

[![.github/workflows/main.yml](https://github.com/bountylabs/blocks-with-screens/actions/workflows/main.yml/badge.svg)](https://github.com/bountylabs/blocks-with-screens/actions/workflows/main.yml) [![Twitter URL](https://img.shields.io/twitter/url/https/twitter.com/blkswithscreens.svg?style=social&label=%40blkswithscreens)](https://twitter.com/blkswithscreens)

<img alt="Block in its natural habitat" src="https://user-images.githubusercontent.com/218876/99318095-f08e0780-281b-11eb-9c59-674a2c463fbb.jpeg" width="33%"><img alt="Animated 3d block" src="https://user-images.githubusercontent.com/218876/99320417-70b66c00-2820-11eb-983f-6c3783336583.gif" width="33%"><img alt="Puzzle-style block" src="https://user-images.githubusercontent.com/218876/99320816-4b762d80-2821-11eb-8898-2ba34d075f10.jpg" width="33%">

It's Twitterversary blocks, but with screens and internet. Originally a Fall 2019 Twitter HackWeek project by [@johnboiles](https://github.com/johnboiles), [@magus](https://github.com/magus), [@mikeperiscope](https://github.com/mikeperiscope), [@seanfchan](https://github.com/seanfchan), and [@smartfuse](https://github.com/smartfuse).

## Assembly

### [**Click here to view assembly instructions on YouTube**](https://youtu.be/LxgdhXQPZw0)

<a href="https://youtu.be/LxgdhXQPZw0" target="_blank">
  <img width="990" alt="assembly-video-thumbnail" src="https://user-images.githubusercontent.com/290084/140593898-c1b5b431-b42b-4ffd-b91d-24ff13c8175f.png">
</a>

Tips, notes & corrections:
* To protect the pins, we're shipping with two of the circuit boards attached to eachother. You'll need to separate these circuit boards before following the instructions above. Just gently pull them apart to separate them. They're hard to break :)
* Some of the boards may have small tabs protruding from the sides which may cause them not to fit in the wooden block. This is a byproduct of the manufacturing process. Clip these tabs off with nail clippers or wire clippers.
* Feel free to ask us on Twitter [@blkswithscreens](https://twitter.com/blkswithscreens) if you have any questions or problems!

## Components

### Blockware

This is the example software that you can run on the blocks. See [blockware/README.md](https://github.com/bountylabs/blocks-with-screens/blob/main/blockware/README.md) for installation instructions and a listing (with gifs!) of the available blockware.

### CAD

This contains the various design files building the block hardware. Most importantly, it contains the [Autodesk Eagle](https://www.autodesk.com/products/eagle/free-download) design files for the 3 PCBs that make up the block. See the [README.md](https://github.com/bountylabs/blocks-with-screens/blob/main/cad/pcb/README.md) in that directory for an overview of the responsibilities of the three different boards.

### VirtualBlox

An experimental block simulator from [@mikeperiscope](github.com/mikeperiscope). You can compile blockware along with VirtualBlox and run block software locally for testing! This gives a quicker development iteration cycle than uploading to the block itself.
