$main = 1;
$assembly = 1;

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

outerBarThickness = 1.8;
outerBarHolderLength = 3;
outerBarHolderWidth = getWheelHeight(_gap) + outerBarThickness;

wheelDistance = getWheelDistance();
engineBoxSize = getEngineBoxSize();
drehgestellWidth = engineBoxSize.y + 2*getWheelHeight(_gap) + 2*outerBarThickness; // 32

axisCenter = getAxisCenter();

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

            rotate([180, 180, ])
            outerBar();
        }
        
        // Angetriebene Achse
        translate([-axisCenter.x, getAxisLength(_gap) / 2, -axisCenter.z])
        rotate([90, 0, 0])
        cylinder(h = getAxisLength(_gap), r = getAxisRadius(_gap));        
    }
}

//nonDrivenAxisThornLength = 3;
//nonDrivenAxisThornRadius = 1;

passiveAxisHeadSize = getPassiveAxisHeadSize();

difference()
{    
    union()
    {
        engineBox();
        outerBars();                
    }
    
    // Nicht angetriebene Achse: Ausschnitt
    translate([axisCenter.x, -getEngineSize().y/2 - _thi2, -axisCenter.z])
    rotate([90, 0, 0])    
    cylinder(h = 100, r = getPassiveAxisRadius());

    translate([axisCenter.x, getEngineSize().y/2 + _thi2, -axisCenter.z])
    rotate([-90, 0, 0])
    cylinder(h = 100, r = getPassiveAxisRadius());    
        
    // Axis head    
    translate([axisCenter.x, -drehgestellWidth/2 + _thi0 - _eps, -axisCenter.z])        
    rotate([90, 0, 0])    
    cylinder(h = passiveAxisHeadSize[1], r = passiveAxisHeadSize[0]); // 1: Soviel ist der Axis head größer als die Achse
    
    // Axis head
    translate([axisCenter.x, drehgestellWidth/2 + -_thi0 + _eps, -axisCenter.z])        
    rotate([-90, 0, 0])    
    cylinder(h = passiveAxisHeadSize[1], r = passiveAxisHeadSize[0]);    
}

// Nicht angetriebene Achse: Dorn
/*translate([axisCenter.x, -getEngineSize().y/2 - _thi2, -axisCenter.z])
rotate([90, 0, 0])
cylinder(h = nonDrivenAxisThornLength, r = nonDrivenAxisThornRadius);

translate([axisCenter.x, getEngineSize().y/2 + _thi2, -axisCenter.z])
rotate([-90, 0, 0])
cylinder(h = nonDrivenAxisThornLength, r = nonDrivenAxisThornRadius);*/


color("red")
if($assembly == 1)
{    
    engineBoxAssembly();
}

