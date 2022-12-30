$assembly = 1;

include<train.base.scad>;
include<injc/Holes.scad>;
use<engineBlock.scad>;
use<wheel.scad>;

// Modul-Übergreifende Variablen dieser Komponente
$fn = 50;

spurweite = _spurweite;
wheelDistance = 26;

engineTopEmptySpace = 2;

engineBottomHolderThickness = _thi0;
engineSize = getEngineSize(_gap);   

engineBoxLength = getEngineSize(_gap).x + 8; // -> 6 länger als der Motor passt ganz gut
engineBoxWidth = spurweite - getWheelHeight(_gap);
engineBoxHeight = getEngineSize(_gap).z + engineBottomHolderThickness + engineTopEmptySpace; 

engineSideHoldersXOffset = (engineBoxLength - wheelDistance )/2;
engineSideHoldersThickness = (engineBoxWidth - engineSize.y)/2;

engineBeginHolderThickness = -getAxisOffset().x + engineSideHoldersXOffset - _gap;

lidThickness = 3;
lidZOffset = -getAxisOffset(_gap).z - engineBottomHolderThickness + engineBoxHeight;

engineEndHolderEngineOffset = 0.5;
engineEndHolderOffset = engineSize.x - getAxisOffset().x + engineEndHolderEngineOffset;
engineEndHolderThickness = (-engineSideHoldersXOffset + engineBoxLength) - engineEndHolderOffset;

    
// Getters
function getEngineBoxSize() = [engineBoxLength, engineBoxWidth, engineBoxHeight];
function getWheelDistance() = wheelDistance;
function getAxisCenter() = [-engineSideHoldersXOffset + engineBoxLength / 2, 0, lidZOffset+lidThickness];
function getLidThickness() = lidThickness;

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
    
    // ----- Engine holders ------
    difference()
    {
        union()
        {        
            // Left side        
            translate([- engineSideHoldersXOffset, engineSize.y/2, -axisOffset.z - _gap])
            cube([engineBoxLength, engineSideHoldersThickness, engineSize.z + engineTopEmptySpace]);
         
            // Right side
            translate([- engineSideHoldersXOffset, -engineSize.y/2 - engineSideHoldersThickness, -axisOffset.z - _gap])
            cube([engineBoxLength, engineSideHoldersThickness, engineSize.z + engineTopEmptySpace]);  
 
            // Begin holder
            translate([-engineSideHoldersXOffset, -engineSize.y/2, -axisOffset.z - _gap])
            cube([engineBeginHolderThickness, engineSize.y, engineSize.z + engineTopEmptySpace]);

            // End Holder
            translate([engineEndHolderOffset, -engineSize.y/2, -axisOffset.z - _gap])
            cube([engineEndHolderThickness, engineSize.y, engineSize.z + engineTopEmptySpace]);
            
            // Lower end Holder
            translate([getEngineSize().x-axisOffset.x-getEngineEndZylinderLength(), -engineSize.y/2, -axisOffset.z - _gap])
            cube([3, engineSize.y, 2.5]);           
        }
                        
        // Axis Holes
        translate([0, -axisLength / 2, 0])
        axisInsertionWedge(axisLength);
                
        // Nut hole
        translate([-axisOffset.x - engineBeginHolderThickness/2, 0,  lidZOffset + _eps])
        insetNutHole();       
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

lidHoleLength = 2;
lidHoleWidth = 10;    
lidHoleHeight = 2;


module axisCenteredEngineBox()
{               
    rectangleEngineBox();     

    // Lid holder bridge
    translate([engineEndHolderOffset, -engineBoxWidth/2, lidZOffset])
    difference()
    {
        cube([engineEndHolderThickness, engineBoxWidth, lidThickness]);
        
        translate([-_eps, (engineBoxWidth - lidHoleWidth) / 2, 0])
        cube([lidHoleLength+_gap, lidHoleWidth+_gap, lidHoleHeight+_gap]);
    }
}

module engineBox()
{    
    //addNotches()
    translateOriginFromAxisToCenter()
    axisCenteredEngineBox();                 
}

module engineBoxLid()
{    
    axisWidth = getAxisRadius()*2;
    axisHoleFillerHeight = 5;

    difference()
    {
        union()
        {

            // The lid itself   
            translate([-engineBoxLength/2, -engineBoxWidth/2, -lidThickness])
            cube([engineBoxLength - engineEndHolderThickness, engineBoxWidth, lidThickness]);
            
            // Axis holes filler
            translate([engineSideHoldersXOffset - engineBoxLength / 2, 0, 0])
            translate([-axisWidth/2, engineSize.y / 2, -axisHoleFillerHeight])
            {
                cube([axisWidth, engineSideHoldersThickness, axisHoleFillerHeight]);
                
                translate([0, -engineSize.y - engineSideHoldersThickness, 0])
                cube([axisWidth, engineSideHoldersThickness, axisHoleFillerHeight]);
            }    
            
            // Lid hole notch
            color("red");
            translate([engineBoxLength/2 - engineEndHolderThickness, -engineBoxWidth/2 + (engineBoxWidth - lidHoleWidth) / 2 + _gap, -lidThickness])
            cube([lidHoleLength-_gap, lidHoleWidth - 2*_gap, lidHoleHeight - _gap]);            
        }
            
        // screw hole
        translate([-engineBoxLength/2 + engineBeginHolderThickness/2 + _gap, 0, -10])
        cylinder(100, 1.4, 1.4);
    }
}

module engineBoxAssembly()
{
    translateOriginFromAxisToCenter()
    if($assembly == 1)
    {    
        translate(-getAxisOffset())
        engineBlock();

        color("red")
        translate([0, spurweite / 2, 0])
        rotate([90, 0, 0])
        drivenWheel();

        color("red")
        translate([0, -spurweite / 2, 0])
        rotate([90, 0, 0])
        drivenWheel();
        
        color("red")
        translate([wheelDistance, -spurweite / 2, 0])
        rotate([90, 0, 0])
        passiveWheel();
        
        color("red")
        translate([wheelDistance, spurweite / 2, 0])
        rotate([90, 0, 0])
        passiveWheel();
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