include <Poor_mans_openscad_screw_library/polyScrewThread.scad>
PI=3.141592;

couplingDiameter = 11;
magnetHeight = 10;

//cylinder(magnetHeight, couplingDiameter/2, couplingDiameter/2);


/* Example 01.
 * Just a 100mm long threaded rod.
 *
 * screw_thread(15,   // Outer diameter of the thread
 *               4,   // Step, traveling length per turn, also, tooth height, whatever...
 *              55,   // Degrees for the shape of the tooth 
 *                       (XY plane = 0, Z = 90, btw, 0 and 90 will/should not work...)
 *             100,   // Length (Z) of the tread
 *            PI/2,   // Resolution, one face each "PI/2" mm of the perimeter, 
 *               0);  // Countersink style:
 *                         -2 - Not even flat ends
 *                         -1 - Bottom (countersink'd and top flat)
 *                          0 - None (top and bottom flat)
 *                          1 - Top (bottom flat)
 *                          2 - Both (countersink'd)
 */

/*difference()
{
 screw_thread(10, 4, 55, 15, PI/4, 2);

$fn=100;
cylinder(20, 0.9, 0.9);
}*/


/* Example 02.
 * A nut for the previous example.
 *
 * hexa_nut(24,  // Distance between flats
 *           8,  // Height 
 *           4,  // Step height (the half will be used to countersink the ends)
 *          55,  // Degrees (same as used for the screw_thread example)
 *          15,  // Outer diameter of the thread to match
 *         0.5)  // Resolution, you may want to set this to small values
 *                  (quite high res) to minimize overhang issues
 *
 */
 hex_nut(12,12,4,55,10.6,0.5);
