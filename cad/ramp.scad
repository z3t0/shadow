// Sumobot Ramp Design

$fn=100;

// Basically the ramp is a triangle design with an attached axle and cutout for the wheels

// Restrictions:
// - The horizontal distance must be less than ~57.3 to fit the height restriction

// Note all descriptions are with the ramp in active mode

// The modifiable variables:
incline_angle = 20; // degrees of ramp angle
clamp_clearance = 2; // height of clamp on either side up and down

// constant variables:
ramp_height = 11.5 + clamp_clearance; // This is calculated as the schematic gives 37.7 - 26 as the distance from the top of the frame to the ground, and then add clamp_clearance
ramp_length = 65; // TODO: a better size
chassis_height = 5.3; // measured ballast + pcb
clamp_width_inside = 5;
clamp_width = 5;
clamp_height_inside = 5;
clamp_width_total = clamp_width_inside + clamp_width;
axle_diameter = 2;
mount_length = 4;


// Calculate:

// width of the prism using tan(incline_angle)
ramp_width = ramp_height / tan(incline_angle);

echo(clamp_clearance=clamp_clearance);
echo(ramp_height=ramp_height);
echo(ramp_width=ramp_width);

// Export options
// 0 : none
// 1 : ramp
// 2: clamp

export=1;

// Ramp

if (export == 2 || export == 0) {

prism(ramp_length, ramp_width, ramp_height);

translate([-2.5, 4+35, 9.31]) {
   axle(clamp_width, axle_diameter, mount_length);
   translate([0, -6, 2]) {
    cube([4, 4, 4]); 
   }
}
}

if (export == 1 || export ==0) {

if(export == 0) {
translate([0, 30, 0]) {
    clamp(clamp_width_total, clamp_height_inside, axle_diameter, 3);
}
}

translate([0, 37, 0]) {
    clamp(clamp_width_total, clamp_height_inside, axle_diameter, 3);
}

}
// Modules

// Creates axle hole and mount
module axle(outer, inner, length) {
    translate([0, 0, outer / 2]) {
        rotate([90, 90, 0]) {
            translate([0, 0, length/2]) {
                    cylinder(d=outer, h=length, center=false);
                translate([0, 0, -5]) {
                    cylinder(h=length+10, d=inner, center=false);
                }
            }
        }
    }
}


module clamp(width, outer, inner, length) {
    rotate([0, 0, 90]){
        union() {
            difference() {
                whole = clamp_clearance * 2 + chassis_height + outer;
                cube([length, width, whole]);
                translate([0, clamp_width, clamp_clearance]) {
                    cube([length, 5, chassis_height]);
                }
                move_z = whole - (outer /2);
                move_y = 10 - 2.5;
                echo(move_z=move_z);
                translate([0, 2.5, move_z]) {
                    rotate([0, 90, 0]) {
                        cylinder(d= inner, h=length);
                    }
                }
            }
        }
    }
}

// RIGHT TRIANGLE PRISM: https://github.com/dannystaple/OpenSCAD-Parts-Library/blob/master/prism.scad
//Draw a prism based on a 
//right angled triangle
//l - length of prism
//w - width of triangle
//h - height of triangle
module prism(l, w, h) {
       polyhedron(points=[
               [0,0,h],           // 0    front top corner
               [0,0,0],[w,0,0],   // 1, 2 front left & right bottom corners
               [0,l,h],           // 3    back top corner
               [0,l,0],[w,l,0]    // 4, 5 back left & right bottom corners
       ], faces=[ // points for all faces must be ordered clockwise when looking in
               [0,2,1],    // top face
               [3,4,5],    // base face
               [0,1,4,3],  // h face
               [1,2,5,4],  // w face
               [0,3,5,2],  // hypotenuse face
       ]);
}