// SumoBot by Rafi Khan

// Maximum
parts();

module parts() {
    // Motors
    wiring_tolerance = 4;
    // R1
    motorAssembly();
    
    // L1
    translate([-wiring_tolerance, 16, 0]) {
        mirror([1, 0, 0]) {
            motorAssembly();
        }
    }
    
    // R2
    translate([0, 32, 0]) {
        motorAssembly();
    }
    
    // L2
    translate([-wiring_tolerance, 48, 0]) {
        mirror([1, 0, 0]) {
            motorAssembly();
        }
    }
}

module motorAssembly() {
    // Motor with wiring and tolerance..?
    width = 28;
    height = 22;
    move =  (width / 2) - (height / 2) + height;

    //echo(move)
    
    color([168/255, 171/255, 175/255,1]) {
        cube([width, 16, 12], center=true);
    }
    
    
    // Wheel
    $fn=100;
    translate([move, 0, 0]) {
        rotate([0, 90, 0]) {
            color([1, 0, 0, 1]) {
                cylinder(h = height, r = 15, center=true);
            }
        }
    }
}

module box() {
    cube(100);
}