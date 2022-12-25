include <train.base.scad>;

// Modul-Übergreifende Variablen dieser Komponente
$fn = 100;

engineLength = 34.4;
engineWidth = 10;
engineHeight = 12;

axisRadius = 1.65;
axisXOffset = 4;
axisZOffset = 6;
axisLength = 30;

// Public Getters
function getEngineSize(gap) = [engineLength+2*gap, engineWidth+2*gap, engineHeight];
function getAxisOffset(gap) = [axisXOffset + gap, 0, axisZOffset + gap];
function getAxisRadius(gap) = axisRadius + gap;
function getAxisLength(gap) = axisLength + 2*gap;

module engineBlock()
{    
    connectorLength = 2.0;
    connectorWidth = 1.5;
    connectorHeight = 0.4;
    connectorCenterOffset = 3.5;

    engineEndCylinderLength = 1.3;
    engineBlockLength = engineLength - engineEndCylinderLength;        
    engineEndCylinderRadius = 2.5;
    engineEndCylinderZOffset = 6;
    
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