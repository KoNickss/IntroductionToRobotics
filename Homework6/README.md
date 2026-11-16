# Homework 6 - Elevator

This project presents an elevator control system, that works as follows: the 3 pushbuttons may be used to send service requests foreach of the 3 floors, which will be reflected in the array of 3 blue LEDs, showing pending requests. Next to them, is a red LED, which, alongside the display "dot point", represents the status of the doors:

- Red led and DP on: doors open
- Red led and DP off: doors closed

The elevator plays relaxing ambient music only as it travels between floors, from a variety of possible songs:

- Starwars cantina band
- Fur elise
- Mountain
- EU anthem
- Nokia ringtone
- O, brad frumos
- We wish you a merry christmas (it's tiiiiiiiiime)

What does an elevator even do, if not sing obnoxious songs? I guess it lifts people sometimes as a nice side effect of the contraption, this one doesn't tho

The elevator also has 2 distinct dings to indicate doors opening and closing.

It uses the algorithm (I believe) most elevators do: go in one direction until the maximal request in that direction is served, then switch directions and continue, again, until maximal floor is served, ad nauseum. Very elegant.

Find: 
[Video Link](videolink.txt)
[KiCAD schematic](schematic.pdf)
