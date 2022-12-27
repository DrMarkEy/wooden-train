$assembly = 1;

// TODO: 
// - Ursprung in Dreh-Achse Verschieben ✅
// - Unterkanten abflachen ✅
// - Deckel draufsetzen ✅
// - Axis hole filler am Deckel ✅

// - Löcher für Schrauben + Muttern, um Deckel zu befestigen
// - Dorn für Drehachse (muss stabil sein!!)
// - Deckel + Dorn muss höher werden als die Räder sind!!
// - Drehgestell sollte dieses File nutzen 

include<train.base.scad>;
include<injc/Holes.scad>;
use<engineBlock.scad>;
use<wheel.scad>;

// Modul-Übergreifende Variablen dieser Komponente
$fn = 50;

spurweite = _spurweite;
wheelDistance = 24;

engineBottomHolderThickness = _thi0;
engineSize = getEngineSize(_gap);   

engineBoxLength = getEngineSize(_gap).x + 6; // -> 6 länger als der Motor passt ganz gut
engineBoxWidth = spurweite - getWheelHeight(_gap);
engineBoxHeight = getEngineSize(_gap).z + engineBottomHolderThickness; 

engineSideHoldersXOffset = (engineBoxLength - wheelDistance )/2;
engineSideHoldersThickness = (engineBoxWidth - engineSize.y)/2;

lidThickness = _thi1;

// Getters
function getEngineBoxSize() = [engineBoxLength, engineBoxWidth, engineBoxHeight];
function getWheelDistance() = wheelDistance;
function getAxisCenter() = [-engineSideHoldersXOffset + engineBoxLength / 2, 0, -getAxisOffset(_gap).z - engineBottomHolderThickness + engineBoxHeight +lidThickness];

// Public modules

module translateOriginFromAxisToCenter()
{
    translate([engineSideHoldersXOffset - engineBoxLength / 2, 0, getAxisOffset(_gap).z + engineBottomHolderThickness - engineBoxHeight -lidThickness])
    {
        children();
    }
}

module rectangleEngineBox()
{
    axisLength = getAxisLength();
    axisOffset = getAxisOffset();      

    engineBeginHolderThickness = -axisOffset.x + engineSideHoldersXOffset - _gap;

    engineEndHolderEngineOffset = 0.5;
    engineEndHolderOffset = engineSize.x - axisOffset.x + engineEndHolderEngineOffset;
    engineEndHolderThickness = (-engineSideHoldersXOffset + engineBoxLength) - engineEndHolderOffset;

    
    // ----- Engine holders ------
    difference()
    {
        union()
        {        
            // Left side        
            translate([- engineSideHoldersXOffset, engineSize.y/2, -axisOffset.z - _gap])
            cube([engineBoxLength, engineSideHoldersThickness, engineSize.z]);
         
            // Right side
            translate([- engineSideHoldersXOffset, -engineSize.y/2 - engineSideHoldersThickness, -axisOffset.z - _gap])
            cube([engineBoxLength, engineSideHoldersThickness, engineSize.z]);  
 
            // Begin holder
            translate([-engineSideHoldersXOffset, -engineSize.y/2, -axisOffset.z - _gap])
            cube([engineBeginHolderThickness, engineSize.y, engineSize.z]);

            // End Holder
            translate([engineEndHolderOffset, -engineSize.y/2, -axisOffset.z - _gap])
            cube([engineEndHolderThickness, engineSize.y, engineSize.z]);
            
            // Lower end Holder
            translate([getEngineSize().x-axisOffset.x-getEngineEndZylinderLength(), -engineSize.y/2, -axisOffset.z - _gap])
            cube([3, engineSize.y, 2.5]);           
        }
                        
        // Axis Holes
        translate([0, -axisLength / 2, 0])
        axisInsertionWedge(axisLength);
                
        // Nut holes
        /*translate([12, 0, 0])
        {
            nutYOffset = 0.5;        
            translate([14, engineSize.y / 2 + engineSideHoldersThickness/2 + nutYOffset, 0])
            rotate([0, 0, 60])
            nut_slot();
            
            translate([14, -engineSize.y / 2 - engineSideHoldersThickness/2 - nutYOffset, 0])
            rotate([0, 0, -60])
            nut_slot();
        }
        
        translate([-7.0, 0, 0])
        rotate([0, 0, 210])
        nut_slot();*/
    }
    
    // Bottom holder
    translate([-engineSideHoldersXOffset, -engineSize.y/2 - engineSideHoldersThickness, axisOffset.z + _gap - engineSize.z - engineBottomHolderThickness])
            cube([engineBoxLength, 2*engineSideHoldersThickness + engineSize.y, engineBottomHolderThickness]);


    module axisInsertionWedge(w=100)
    {        
        axisHoleRadius = getAxisRadius(_gap);
        
        // Axis Holes
        translate([0, 0, 0])
        rotate([-90, 0, 0])
        cylinder(w, axisHoleRadius, axisHoleRadius);
            
        
        // bottom hole for inserting the axis
        translate([- axisHoleRadius, 0, 0])    
        cube([2*axisHoleRadius, w, 100]);
    }
}

module axisCenteredEngineBox()
{
    notchDepth = 2.5;
    
    difference()
    {
        
        {
           rectangleEngineBox();
        }
        
        translate([-engineSideHoldersXOffset , -engineBoxWidth/2-_eps, -getAxisOffset().z + notchDepth])
        rotate([0, 45, 0])
        translate([0, 0, -engineBoxHeight])
        cube([engineBoxLength, engineBoxWidth+2*_eps, engineBoxHeight]);
        
        
        translate([-engineSideHoldersXOffset + engineBoxLength , -engineBoxWidth/2-_eps, -getAxisOffset().z + notchDepth])
        rotate([0, -45, 0])
        translate([-engineBoxLength, 0, -engineBoxHeight])
        cube([engineBoxLength, engineBoxWidth+2*_eps, engineBoxHeight]);
    }    
}

module engineBox()
{
    translateOriginFromAxisToCenter()
    axisCenteredEngineBox();         
}

module engineBoxLid()
{
    axisWidth = getAxisRadius()*2;
    axisHoleFillerHeight = 5;

    // The lid itself   
    translate([-engineBoxLength/2, -engineBoxWidth/2, -lidThickness])
    cube([engineBoxLength, engineBoxWidth, lidThickness]);
    
    // Axis holes filler
    translate([engineSideHoldersXOffset - engineBoxLength / 2, 0, 0])
    translate([-axisWidth/2, engineSize.y / 2, -axisHoleFillerHeight])
    {
        cube([axisWidth, engineSideHoldersThickness, axisHoleFillerHeight]);
        
        translate([0, -engineSize.y - engineSideHoldersThickness, 0])
        cube([axisWidth, engineSideHoldersThickness, axisHoleFillerHeight]);
    }    
}

module engineBoxAssembly()
{
    translateOriginFromAxisToCenter()
    if($assembly == 1)
    {    
        wheelRadius = 10;
        
        translate(-getAxisOffset())
        engineBlock();

        color("red")
        translate([0, spurweite / 2, 0])
        rotate([90, 0, 0])
        drivenWheel(wheelRadius);

        color("red")
        translate([0, -spurweite / 2, 0])
        rotate([90, 0, 0])
        drivenWheel(wheelRadius);
        
        color("red")
        translate([wheelDistance, -spurweite / 2, 0])
        rotate([90, 0, 0])
        passiveWheel(wheelRadius);
        
        color("red")
        translate([wheelDistance, spurweite / 2, 0])
        rotate([90, 0, 0])
        passiveWheel(wheelRadius);
    }
}

// Render part if not main
if($main != 1)
{    
    // Render dependend parts as assembly instructions
    if($assembly)
    color("red")
    engineBoxAssembly();
        
    engineBox();    
    
    color("orange", 0.5)
    engineBoxLid();    
}