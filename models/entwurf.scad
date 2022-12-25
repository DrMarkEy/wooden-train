$main = 1;

include<engineBlock.scad>;


bottomHeight = 5;


$fn = 100;
cylinder(bottomHeight, 16, 16);

translate([0, 0, bottomHeight])
rotate([0, -90, 0])
translate([0, 0, -engineHeight/2])
engineBlock();