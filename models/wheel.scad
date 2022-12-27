include <train.base.scad>;

// Modul-Übergreifende Variablen dieser Komponente
$fn = 100;
_wheelHeight = 3.5;
_wheelRadius = 10;
axisRadius = 1.65;

passiveWheelSideWiggleRoom = 1;
passiveWheelAxisWiggleRoom = 0.6;
passiveWheelExtraRadius = 0.5;

function getWheelHeight(gap=0) = _wheelHeight + 2*gap;
function getAxisRadius(gap=0) = axisRadius + gap;

module drivenWheel(wheelRadius = _wheelRadius, wheelHeight = _wheelHeight)
{            
    axisNotchCenterOffset = 1.2;

    gummiThickness = 1.4;
    gummiWidth = 2.5;
    gummiOffset = (wheelHeight - gummiWidth)/2;

    
    translate([0, 0, -wheelHeight/2])
    union()
    {
        difference()
        {
            union()
            {
                difference()
                {
                    cylinder(wheelHeight, wheelRadius, wheelRadius);
                    
                    translate([0, 0, gummiOffset])
                    cylinder(gummiWidth, wheelRadius+1, wheelRadius+1);
                }
                
                cylinder(wheelHeight, wheelRadius-gummiThickness, wheelRadius-gummiThickness);
            }
            
            translate([0,0,-0.01])
            cylinder(wheelHeight+0.02, axisRadius, axisRadius);
        }

        translate([-2, axisNotchCenterOffset, 0])
        cube([4, 4, wheelHeight]);
    }
}

module passiveWheel(wheelRadius = _wheelRadius + passiveWheelExtraRadius, wheelHeight = _wheelHeight - passiveWheelSideWiggleRoom)
{               
    restHeight = 0.1;
    minkowskiRadius = (wheelHeight - restHeight)/2;
    adjustedRadius = wheelRadius - minkowskiRadius;
        
    translate([0, 0, -restHeight/2])
    union()
    {
        difference()
        {            
            minkowski()
            {
                
                cylinder(restHeight, adjustedRadius, adjustedRadius);
                sphere(minkowskiRadius);
            }
         
            translate([0, 0, -wheelHeight/2])   
            translate([0,0,-0.01])
            cylinder(wheelHeight+0.02+restHeight, axisRadius+passiveWheelAxisWiggleRoom, axisRadius+passiveWheelAxisWiggleRoom);
        }        
    }
}

module wheelCase(gap=0, wheelRadius = _wheelRadius, wheelHeight = _wheelHeight)
{
    translate([0, 0, -wheelHeight/2-gap])
    union()
    {
        cylinder(wheelHeight+2*gap, wheelRadius+gap, wheelRadius+gap);
        
        translate([-wheelRadius-gap, 0, 0])
        cube([2*wheelRadius+2*gap, 2*wheelRadius, wheelHeight+2*gap]);
    }
}


// Render if not main
if ($main != 1)    
    //drivenWheel();
    passiveWheel();