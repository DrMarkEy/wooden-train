// Public variables
engineLength = 34.4;
engineWidth = 10;
engineHeight = 12;

axisRadius = 1.65;
axisXOffset = 4;
axisZOffset = 6;
axisLength = 30;

engineEndCylinderLength = 1.3; // Shoule be private...
engineBlockLength = engineLength - engineEndCylinderLength;

module engineBlock()
{
    $fn = 100;
 
    // Private variables
    axisLength = 30;

    connectorLength = 2.0;
    connectorWidth = 1.5;
    connectorHeight = 0.4;
    connectorCenterOffset = 3.5;
        
    engineEndCylinderRadius = 2.5;
    engineEndCylinderZOffset = 6;
    
    engineBlockLength = engineLength - engineEndCylinderLength;    
    
    // Block
    translate([0, -engineWidth / 2, 0])
    cube([engineBlockLength, engineWidth, engineHeight]);
 
    // Axle
    translate([axisXOffset, -axisLength/2, axisZOffset])
    rotate([-90, 0, 0])
    cylinder(axisLength, axisRadius, axisRadius);
    
    // Connectors
    translate([engineBlockLength, -connectorWidth/2, engineHeight / 2 - connectorHeight / 2 + connectorCenterOffset])
    cube([connectorLength, connectorWidth, connectorHeight]);
    
    translate([engineBlockLength, -connectorWidth/2, engineHeight / 2 - connectorHeight / 2 - connectorCenterOffset])
    cube([connectorLength, connectorWidth, connectorHeight]);
    
    // EndCylinder
    translate([engineBlockLength, 0, engineEndCylinderZOffset])
    rotate([0, 90, 0])
    cylinder(engineEndCylinderLength, engineEndCylinderRadius, engineEndCylinderRadius);
}

// Render if not main
if ($main != 1)
    engineBlock();