$main = 1;
//$assembly = 1;

// Idee: Austauschbare und drehbare Drehgestelle für die Räder
// Vorne: Drehbares Zahnrad mit Servo-Antrieb; Color-Sensor am Boden Zwischen den Lenk-Rädern
// Hinten: Drehbares, angetriebenes Gestell + Motor mit integrierter Rückstellfeder
// -> Achtung! Braucht das hintere Drehgestell dann zwei Achsen??

include<train.base.scad>;
use<engineBlock.scad>;
use<engineBox.scad>;
use<wheel.scad>;

// Modul-Übergreifende Variablen dieser Komponente
$fn = 50;

magnetHoleDiameter = 0.7;
magnetHoleHeight = 10; // Höhe über unterkante des drehgestells
magnetHoleCylinderRadius = 5;
magnetHoleCylinderHeight = 6;
magnetHoleCylinderHoleRadius = 3.5;
magnetHoleCylinderHoleDepth = 1.25;

outerBarThickness = 1.8;
outerBarHolderLength = 3;
outerBarHolderWidth = getWheelHeight(_gap) + outerBarThickness;

wheelDistance = getWheelDistance();
engineBoxSize = getEngineBoxSize();
drehgestellWidth = engineBoxSize.y + 2*getWheelHeight(_gap) + 2*outerBarThickness; // 32

axisCenter = getAxisCenter();


module addNotches()
{
    notchDepth = 4;    
    engineBoxSize = getEngineBoxSize();
    
    difference()    
    {        
        union()
        {
            children();
        }
        
        // Front notch
        translate([-engineBoxSize.x/2, -engineBoxSize.y/2-_eps, -engineBoxSize.z - getLidThickness() + notchDepth])
        rotate([0, 45, 0])
        translate([0, 0, -engineBoxSize.z])
        cube([engineBoxSize.x, engineBoxSize.y+2*_eps, engineBoxSize.z]);
        
        // Back notch
        translate([engineBoxSize.x/2 , -engineBoxSize.y/2-_eps, -engineBoxSize.z - getLidThickness() + notchDepth])
        rotate([0, -45, 0])
        translate([-engineBoxSize.x, 0, -engineBoxSize.z])
        cube([engineBoxSize.x, engineBoxSize.y+2*_eps, engineBoxSize.z]);
    }   
}

module addOuterBars()
{    
    module outerBar()
    {
        outerBarHeight = 8;
        axisCenter = getAxisCenter();
        
        // The bars themselves (cube + 2 cylinders at the end)
        translate([-axisCenter.x, -drehgestellWidth/2, -axisCenter.z])
        {        
            rotate([-90, 0, 0])
            cylinder(outerBarThickness, outerBarHeight/2, outerBarHeight/2);
            
            translate([wheelDistance, 0, 0])
            rotate([-90, 0, 0])
            cylinder(outerBarThickness, outerBarHeight/2, outerBarHeight/2);
            
            translate([0, 0, -outerBarHeight/2])
            cube([wheelDistance, outerBarThickness, outerBarHeight]);        
        }    
        
        // Outer bar holder  
        translate([-outerBarHolderLength/2, -drehgestellWidth/2, 0])    
        hull()
        {
            //color("green")
            //cube([100, 100, 0.1], center=true);
            
            translate([0, 0, - outerBarHeight/2 -axisCenter.z])
            cube([outerBarHolderLength, outerBarHolderWidth, outerBarHeight]);
            
            color("red")
            translate([0, outerBarHolderWidth, -getLidThickness() - getEngineBoxSize().z])
            cube([outerBarHolderLength, 0.1, engineBoxSize.z]);
        }
    }    

    union()
    {
        children();
        
        outerBar();

        rotate([180, 180, 0])
        outerBar();
    }
}


module addMagnetConnector(xPos)
{
    difference()
    {
        union()
        {
            children();
            
            // magnet cylinder
            translate([xPos, 0, -engineBoxSize.z - getLidThickness() + magnetHoleHeight])
            rotate([0, 90, 0])
            cylinder(magnetHoleCylinderHeight, magnetHoleCylinderRadius , magnetHoleCylinderRadius);
        }
        
        // magnet needle hole
        translate([xPos -10, 0, -engineBoxSize.z - getLidThickness() + magnetHoleHeight])
        rotate([0, 90, 0])
        cylinder(20, magnetHoleDiameter , magnetHoleDiameter);  

        // magnet inset hole
        translate([engineBoxSize.x/2 + magnetHoleCylinderHeight + _eps, 0, -engineBoxSize.z - getLidThickness() + magnetHoleHeight])
        rotate([0, -90, 0])
        cylinder(magnetHoleCylinderHoleDepth, magnetHoleCylinderHoleRadius , magnetHoleCylinderHoleRadius);  
    }
}


module addPassiveAxisHole(xPos)
{
    module passiveAxisHole(sig, startYOffset, radius, height)
    {
        translate([xPos, startYOffset, -axisCenter.z])        
        rotate([90*sig, 0, 0])    
        cylinder(h = height, r = radius);
    }
    
    passiveAxisHeadSize = getPassiveAxisHeadSize();

    difference()
    {
        children();
        
        // Nicht angetriebene Achse: Ausschnitt (innen)
        passiveAxisHole(1, -getEngineSize().y/2 - _thi2, getPassiveAxisRadius(), 100);
        passiveAxisHole(-1, getEngineSize().y/2 + _thi2, getPassiveAxisRadius(), 100);
        
        // Nicht angetriebene Achse: Größerer Ausschnitt in (äußerem holder (extra gap))
        passiveAxisHole(1, -getEngineBoxSize().y/2 - _thi2, getPassiveAxisRadius()+_gap, 100);
        passiveAxisHole(-1, getEngineBoxSize().y/2, getPassiveAxisRadius()+_gap, 100);
        
        // Axis head
        passiveAxisHole(1, -drehgestellWidth/2 + +_thi0 - _eps, passiveAxisHeadSize[0]+_gap, passiveAxisHeadSize[1]);
        passiveAxisHole(-1, drehgestellWidth/2 + -_thi0 + _eps, passiveAxisHeadSize[0]+_gap, passiveAxisHeadSize[1]);
    }
}


module addDrivenAxisHole(xPos)
{
    difference()
    {
        children();
        
        // Angetriebene Achse
        translate([xPos, getAxisLength(2*_gap) / 2, -axisCenter.z])
        rotate([90, 0, 0])
        cylinder(h = getAxisLength(2*_gap), r = getAxisRadius(_gap));       
        
        // Extra Gap außen                
        translate([xPos, -getEngineBoxSize().y/2 - _eps, -axisCenter.z])
        rotate([90, 0, 0])
        cylinder(h = getAxisLength(2*_gap)/2 - getEngineBoxSize().y/2, r = getAxisRadius(2*_gap));       
        
        // Extra Gap außen                
        translate([xPos, getEngineBoxSize().y/2 + _eps, -axisCenter.z])
        rotate([-90, 0, 0])
        cylinder(h = getAxisLength(2*_gap)/2 - getEngineBoxSize().y/2, r = getAxisRadius(2*_gap));       
    }
}


module drehgestellHinten()
{
    addMagnetConnector(engineBoxSize.x/2)        
    addDrivenAxisHole(-axisCenter.x)            
    addPassiveAxisHole(axisCenter.x)               
    addOuterBars()                   
    addNotches() 
    engineBox();                   
}

module passiveAxis()
{    
    passiveAxisLength = drehgestellWidth/2 + (-getEngineSize().y/2 - _thi2) - _gap;    
    makePassiveAxis(passiveAxisLength);
}


drehgestellHinten();
//passiveAxis();


/*
// TODO: cylinder-Aufbau
cylinder(3, 10, 10);

translate([0, 0, 3])
cylinder(2, 6, 6);

translate([0, 0, 5])
scale([1.7, 1, 1])
cylinder(2, 6, 6);*/
if($assembly == 1)
{    
    color("red")
    engineBoxAssembly();
    
    color("darkorange")
    {
        translate([wheelDistance/2, drehgestellWidth/2, -axisCenter.z])
        rotate([90, 0, 0])
        passiveAxis();
        
        translate([wheelDistance/2, -drehgestellWidth/2, -axisCenter.z])
        rotate([-90, 0, 0])
        passiveAxis();
    }
    
    color("orange")
    engineBoxLid();
}

