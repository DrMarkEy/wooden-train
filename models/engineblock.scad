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

engineEndCylinderLength = 1.3;

// Public Getters
function getEngineSize(gap=0) = [engineLength+2*gap, engineWidth+2*gap, engineHeight + 2*gap];
function getAxisOffset(gap=0) = [axisXOffset + gap, 0, axisZOffset + gap];
function getAxisRadius(gap=0) = axisRadius + gap;
function getAxisLength(gap=0) = axisLength + 2*gap;

function getEngineEndZylinderLength() = engineEndCylinderLength;


module engineBlock(gap=0)
{    
    connectorLength = 2.0;
    connectorWidth = 1.5;
    connectorHeight = 0.4;
    connectorCenterOffset = 3.5;

    engineBlockLength = engineLength - engineEndCylinderLength;        
    engineEndCylinderRadius = 2.5;
    engineEndCylinderZOffset = 6;
    
    // Block
    translate([0, -engineWidth / 2, 0])    
    gCube([engineBlockLength, engineWidth, engineHeight], gap);
 
    // Axle
    translate([axisXOffset, -axisLength/2, axisZOffset])
    rotate([-90, 0, 0])
    gCylinder(axisLength, axisRadius, axisRadius, [0, gap]);
    
    // Connectors
    translate([engineBlockLength, -connectorWidth/2, engineHeight / 2 - connectorHeight / 2 + connectorCenterOffset])
    gCube([connectorLength, connectorWidth, connectorHeight], [gap, 0, 0]);
    
    translate([engineBlockLength, -connectorWidth/2, engineHeight / 2 - connectorHeight / 2 - connectorCenterOffset])
    gCube([connectorLength, connectorWidth, connectorHeight], [gap, 0, 0]);
    
    // EndCylinder
    translate([engineBlockLength, 0, engineEndCylinderZOffset])
    rotate([0, 90, 0])
    gCylinder(engineEndCylinderLength, engineEndCylinderRadius, engineEndCylinderRadius, [gap, 0]);
}

// Render if not main
if ($main != 1)
    engineBlock();