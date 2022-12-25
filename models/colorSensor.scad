include <train.base.scad>;

// Modul-Übergreifende Variablen dieser Komponente
$fn = 100;

_trainWidth = 30;

sensorLength = 20.5;
sensorWidth = 20.5;
sensorThickness = 1.6;

// TODO: Public Getters

module colorSensor(gap = 0, wallThickness = _thi0, trainWidth = _trainWidth)
{

    $fn = 100;

    _sensorLength = sensorLength + 2*gap;
    _sensorWidth = sensorWidth + 2*gap;
    _sensorThickness = sensorThickness;

    holeRadius = 1.2;
    holeXOffset = 2.5;
    holeYOffset = 2.5;
    
    steckleistenThickness = 3;

    union()
    {
        translate([-gap, -_sensorWidth / 2, 0])
        cube([_sensorLength, _sensorWidth, _sensorThickness]);

        translate([holeXOffset, sensorWidth/2-holeYOffset, -0.01])
        cylinder(10, holeRadius, holeRadius);
        
        translate([holeXOffset, -sensorWidth/2+holeYOffset, -0.01])
        cylinder(10, holeRadius, holeRadius);
        
        translate([sensorLength - steckleistenThickness-gap, -_sensorWidth / 2, 0])
        cube([steckleistenThickness+2*gap, _sensorWidth, 10]);
    }
}

module colorSensorPocket(wallThickness = _thi0, trainWidth = _trainWidth)
{
    difference()
    {
        translate([-wallThickness, -trainWidth/2, 0])
        cube([sensorLength + 2*wallThickness, trainWidth, 3]);
        
        translate([0,0,-_eps])
        colorSensor(_gap);
    }
}

// Render if not main
if ($main != 1)
{
    colorSensorPocket();
    
    color("red")
    colorSensor();
}