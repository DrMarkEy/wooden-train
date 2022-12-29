$main = 1;
//$assembly = 1;

// TODO:
// - Löcher für die angetriebenen Achsen ✅
// - Halter für AußenHalterungen --> woher kommt die differenz von 0.4 ???
// - Löcher für nicht angetriebene Achsen (Außen und Innen) ✅

// - Räder-Abstand vergrößern, um Bodenfreiheit zu gewinnen ✅
// - Löcher in Außenhalterungen größer (und tiefer) machen ✅
// - Dorn in Antriebsloser Achse unnötig (oder sollte zumindest nach außen hin spitzer werden) ✅
// - Nochmal Räder mit passendem (kleineren) Radius drucken

// - Mehr Spiel auf beiden Seiten der antriebslosen Räder, um sie drehbarer zu machen ✅
// ---> Angetriebene Räder Dicker machen als antriebslose ✅
// - Antriebslose Räder + Achsen designen (mit Wiggle-Room) ✅

// - Deckel-Verschluss (Wie Batteriefach mit einer/ohne Schrauben??) designen


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

module drehgestellHinten()
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
        translate([-outerBarHolderLength/2, -drehgestellWidth/2, -axisCenter.z])    
        hull()
        {
            translate([0, 0, - outerBarHeight/2])
            cube([outerBarHolderLength, outerBarHolderWidth, outerBarHeight]);
            
            translate([0, outerBarHolderWidth, - engineBoxSize.z/2 - 0.4]) // TODO: Warum - 0.4 ???
            cube([outerBarHolderLength, 0.1, engineBoxSize.z]);
        }
    }

    module outerBars()
    {    
        difference()
        {
            union()
            {
                outerBar();

                rotate([180, 180, 0])
                outerBar();
            }
            
            // Angetriebene Achse
            translate([-axisCenter.x, getAxisLength(2*_gap) / 2, -axisCenter.z])
            rotate([90, 0, 0])
            cylinder(h = getAxisLength(2*_gap), r = getAxisRadius(2*_gap));        
        }
    }

    passiveAxisHeadSize = getPassiveAxisHeadSize();

    difference()
    {    
        union()
        {
            engineBox();
            outerBars();      
  
            // magnet cylinder
            translate([engineBoxSize.x/2, 0, -engineBoxSize.z - getLidThickness() + magnetHoleHeight])
            rotate([0, 90, 0])
            cylinder(magnetHoleCylinderHeight, magnetHoleCylinderRadius , magnetHoleCylinderRadius);
        }
        
        // Nicht angetriebene Achse: Ausschnitt (innen)
        passiveAxisHole(1, -getEngineSize().y/2 - _thi2, getPassiveAxisRadius(), 100);
        passiveAxisHole(-1, getEngineSize().y/2 + _thi2, getPassiveAxisRadius(), 100);
        
        // Nicht angetriebene Achse: Größerer Ausschnitt in (äußerem holder (extra gap))
        passiveAxisHole(1, -getEngineBoxSize().y/2 - _thi2, getPassiveAxisRadius()+_gap, 100);
        passiveAxisHole(-1, getEngineBoxSize().y/2, getPassiveAxisRadius()+_gap, 100);
        
        // Axis head
        passiveAxisHole(1, -drehgestellWidth/2 + +_thi0 - _eps, passiveAxisHeadSize[0]+_gap, passiveAxisHeadSize[1]);
        passiveAxisHole(-1, drehgestellWidth/2 + -_thi0 + _eps, passiveAxisHeadSize[0]+_gap, passiveAxisHeadSize[1]);                  
        // magnet needle hole
        translate([engineBoxSize.x/2 -10, 0, -engineBoxSize.z - getLidThickness() + magnetHoleHeight])
        rotate([0, 90, 0])
        cylinder(20, magnetHoleDiameter , magnetHoleDiameter);  

        // magnet inset hole
        translate([engineBoxSize.x/2 + magnetHoleCylinderHeight + _eps, 0, -engineBoxSize.z - getLidThickness() + magnetHoleHeight])
        rotate([0, -90, 0])
        cylinder(magnetHoleCylinderHoleDepth, magnetHoleCylinderHoleRadius , magnetHoleCylinderHoleRadius);  
    }    
}

module passiveAxisHole(sig, startYOffset, radius, height)
{
    translate([axisCenter.x, startYOffset, -axisCenter.z])        
    rotate([90*sig, 0, 0])    
    cylinder(h = height, r = radius);
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

