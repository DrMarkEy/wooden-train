$main = 1;
$assembly = 0;
$fn = 100;

wallThickness = 1.3;
gap = 0.2;

trainLength = 60;//45;
trainWidth = 34;
trainHeight = 50;

spurweite = 24;

use<wheel.scad>;
include<engineBlock.scad>;


engineXOffset = wallThickness + 10;

engineEndHolderLength = 4;
engineEndHolderWidth = 2;
engineEndHolderHeight = 12;

axisHoleRadius = axisRadius + gap;

difference()
{
    // Bottom plate
    translate([0, -trainWidth / 2, 0])
    cube([trainLength, trainWidth, wallThickness]);
    
    // Holes for wheels (left)
    translate([engineXOffset + axisXOffset, spurweite/2, axisZOffset + wallThickness])
    rotate([-90, 0 ,0])
    wheelCase(gap);
    
    // Holes for wheels (right)
    translate([engineXOffset + axisXOffset, -spurweite/2, axisZOffset + wallThickness])
    rotate([-90, 0 ,0])
    wheelCase(gap);
}

// Plate between engine and magnet
translate([engineXOffset-wallThickness, -engineWidth / 2 - gap, wallThickness])
cube([wallThickness-gap, engineWidth + 2*gap, engineHeight]);

// ---- Engine end holder -----
// Left Part
translate([engineBlockLength + engineXOffset + gap, -engineWidth / 2 - gap, wallThickness])
cube([engineEndHolderLength - gap, engineEndHolderWidth+gap, engineEndHolderHeight]);

// Right Part
translate([engineBlockLength + engineXOffset + gap, engineWidth / 2 - engineEndHolderWidth, wallThickness])
cube([engineEndHolderLength - gap, engineEndHolderWidth + gap, engineEndHolderHeight]);


// TODO: Woher kommt die 1.05 ???
outerSideHolderWidth = 1.05 + (trainWidth - axisLength)/2;
innerSideHolderWidth = 3.55;

difference()
{
    union()
    {
        // ----- Engine side holder ------
        
        // Left side
        translate([engineXOffset-wallThickness, engineWidth / 2 + gap, wallThickness])
        cube([engineLength + engineEndHolderLength, wallThickness, engineHeight]);
     
        // Right side
        translate([engineXOffset-wallThickness, -engineWidth / 2 - wallThickness - gap, wallThickness])
        cube([engineLength + engineEndHolderLength, wallThickness, engineHeight]);   
        
        // -------- Outer Wheel side Holder --------          
        translate([0, trainWidth/2, 1.1])
        wheelSideHolder(1, outerSideHolderWidth);

        translate([0, -trainWidth/2, 1.1])
        wheelSideHolder(-1, outerSideHolderWidth);     

        // -------- Inner Wheel side Holder --------          
        translate([0, -engineWidth/2 - gap - wallThickness, 1.1])
        wheelSideHolder(1, innerSideHolderWidth);

        translate([0, engineWidth/2 + gap + wallThickness, 1.1])
        wheelSideHolder(-1, innerSideHolderWidth);             
    }
    
    // Axis Holes
    translate([engineXOffset + axisXOffset, -axisLength / 2, axisZOffset + wallThickness])
    axisInsertionWedge(axisLength);
}





module axisInsertionWedge(w=100)
{
    // Axis Holes
    translate([0, 0, 0])
    rotate([-90, 0, 0])
    cylinder(w, axisHoleRadius, axisHoleRadius);
        
    
    // Top hole for inserting the axis
    translate([- axisHoleRadius, 0, 0])    
    cube([2*axisHoleRadius, w, 100]);
}

module wheelSideHolder(sign=1, sideHolderWidth)
{                    
    sideHolderRadius = 10;
    hull()
    {
        intersection()
        {
            translate([engineXOffset + axisXOffset, 0, axisZOffset + wallThickness])
            {
                rotate([sign*90, 0, 0])
                cylinder(sideHolderWidth, sideHolderRadius, sideHolderRadius);        
            }
            
            translate([-100, -100, 0])
            cube([200, 200, 100]);
        }
                
        translate([engineXOffset + axisXOffset, -sign*sideHolderWidth/2, 0.5])
        rotate([0, 0, sign*180])    
        cube([sideHolderRadius*3, sideHolderWidth, 1], center = true);
    }
    
}



// ----- Engine Block ------
if($assembly == 1)
{
    color("red")
    translate([engineXOffset, 0, wallThickness])
    engineBlock();
}

// ----- Wheels -------
if($assembly == 1)
{
    color("red")
    translate([engineXOffset + axisXOffset, spurweite/2, axisZOffset + wallThickness])
    rotate([-90, 0 ,0])
    wheel();

    color("red")
    translate([engineXOffset + axisXOffset, -spurweite/2, axisZOffset + wallThickness])
    rotate([-90, 0 ,0])
    wheel();
}