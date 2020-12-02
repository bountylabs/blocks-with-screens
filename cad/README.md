# CAD Files

This contains the various design files for the mechanical components of the blocks.

## Parts List
* Assembled PCBs (See the README.md in the `pcb` directory in the repository for more details)
  * USB board
  * Microcontroller board
  * OLED board
* 1.5” SSD1351 128x128 OLED screen
* 240mAh 602025 LiPo battery with JST-SH connector
* Laser-cut wood “puzzle” blocks (5 separate pieces that fit together)
* Laser cut paper spacers (for friction fit and alignment)

## Components

### PCBs

The most important part of the block! The `pcb` directory contains the [Autodesk Eagle](https://www.autodesk.com/products/eagle/free-download) design files for the 3 PCBs that make up the block. See the [README.md](https://github.com/bountylabs/blocks-with-screens/blob/main/cad/pcb/README.md) in that directory for an overview of the responsibilities of the three different boards.

### Laser-cut Wood “Puzzle” Blocks

These are the laser-cuttable sides and back of the block. Since these need to fit together with only friction, the design may need to be adjusted based on the laser being used. Designs are included for a 0.23mm kerf laser as well as ones that worked well on @johnboiles's GlowForge. The `puzzleblock.f3d` (Fusion 360) file has the original designs.

### Laser-cut Paper Spacers

These help hold the PCBs inside the block and also helps center them inside the block. These should be cut out of 110 lb card stock paper. The laser width doesn’t matter here since paper bends.

### Block With Screen F3Z File

This Fusion 360 file has all of the components modeled in one place. Used mostly for neat visualizations like the assembly video. Also very useful to visualize the internal spacing when changing PCB components placements. This also contains the original design for the paper spacers.
