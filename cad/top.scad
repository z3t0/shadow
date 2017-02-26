// Sumobot - Top Mount
// ~54 -> 50
// ~86.7 -> 85 -(13/2) = 
// 68.66 -> 68.66
// `1.75 + 0.2 -> 2.05
// offset = radius + tolerance 
// offset = 1.75 + 0.25 = 2.00

$fn=100;

width = 5;
hole = 100;

difference() {
    cube([50, 78.5, width]);
    
    // Mounting Holes
    translate([9.44, 2.00, 0]) {
        union() {
            cylinder(h=hole, d=3.5, center=true);
            
            translate([31.12 , 0, 0]) {
                cylinder(h=hole, d=3.5, center=true);
            }
            
            translate([31.12, +68.66, 0]) {
                cylinder(h=hole, d=3.5, center=true);
            }
            
            translate([0, +68.66, 0]) {
                cylinder(h=hole, d=3.5, center=true);
            }
        }
    }
    
    // Wiring gap : 20 * 15, offset = 20
    translate([15, 20 , 0]) {
        cube([20, 15, hole]);
    }
}
