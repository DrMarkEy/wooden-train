wallThickness = 1.3;
trainWidth = 30; // fixed due to engine size

sensorLength = 20.5;
sensorWidth = 20.5;
sensorThickness = 1.6;

module colorSensor(gap = 0)
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

module colorSensorPocket()
{
    difference()
    {
        translate([-wallThickness, -trainWidth/2, 0])
        cube([sensorLength + 2*wallThickness, trainWidth, 3]);
        
        translate([0,0,-0.01])
        colorSensor(0.1);
    }
}

// Render if not main
if ($main != 1)
{
    colorSensorPocket();
    
    color("red")
    colorSensor();
}