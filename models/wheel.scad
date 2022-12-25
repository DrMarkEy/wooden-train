include <train.base.scad>;

// Modul-Übergreifende Variablen dieser Komponente
$fn = 100;
_wheelHeight = 3.5;
_wheelRadius = 10;

module wheel(wheelRadius = _wheelRadius, wheelHeight = _wheelHeight)
{            
    wallThickness = 1.3;
    axisRadius = 1.65;
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
    wheel();