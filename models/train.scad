$main = 1;
$assembly = 1;
$fn = 100;

use<wheel.scad>;
include<engineBlock.scad>;
include<colorSensor.scad>;


wallThickness = 1.3;
gap = 0.2;

trainLength = 40;//45;
trainWidth = 30; // fixed due to engine size
trainHeight = 50;

engineXOffset = wallThickness;

engineEndHolderLength = 4;
engineEndHolderWidth = 2;
engineEndHolderHeight = 12;


axisHoleRadius = axisRadius + gap;

/*magnetOverTrackHeight = 12;
trainOverTrackHeight = 3;
trackGrooveHeight = 3;

wheelOverlap = trackGrooveHeight + trainOverTrackHeight; // how much the wheel shos out of the train

backWheelOffset = 2;

//engineAxleOffset = 6;
*/

// Bottom plate
translate([0, -trainWidth / 2, 0])
cube([trainLength, trainWidth, wallThickness]);

// Plate between engine and magnet
translate([0, -engineWidth / 2 - gap, wallThickness])
cube([wallThickness-gap, engineWidth + 2*gap, engineHeight]);

// ---- Engine end holder -----
// Left Part
translate([engineBlockLength + engineXOffset + gap, -engineWidth / 2 - gap, wallThickness])
cube([engineEndHolderLength - gap, engineEndHolderWidth+gap, engineEndHolderHeight]);

// Right Part
translate([engineBlockLength + engineXOffset + gap, engineWidth / 2 - engineEndHolderWidth, wallThickness])
cube([engineEndHolderLength - gap, engineEndHolderWidth + gap, engineEndHolderHeight]);

// ----- Engine side holder ------
difference()
{
    union()
    {
        // Left side
        translate([0, engineWidth / 2 + gap, wallThickness])
        cube([engineLength + engineEndHolderLength, wallThickness, engineHeight]);
     
        // Right side
        translate([0, -engineWidth / 2 - wallThickness - gap, wallThickness])
        cube([engineLength + engineEndHolderLength, wallThickness, engineHeight]);   
    }
    
    // Axis Holes
    translate([axisXOffset + wallThickness, 50, axisZOffset + wallThickness])
    rotate([90, 0, 0])
    cylinder(100, axisHoleRadius, axisHoleRadius);
        
    
    // Top hole for inserting the axis
    translate([axisXOffset + wallThickness - axisHoleRadius, -10, axisZOffset + wallThickness])    
    cube([2*axisHoleRadius, 20, 100]);
}


// ----- Engine Block ------
if($assembly == 1)
{
    color("red")
    translate([engineXOffset, 0, wallThickness])
    engineBlock();
}

// ------ Color Sensor ------
translate([40, 0, 0])
{
    colorSensorPocket();

    if($assembly == 1)
    {    
        color("red")
        colorSensor();
    }
}