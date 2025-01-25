$fn = 100;

use <pins/pin2.scad>

height = 76.2; // 3"
od = 304.8; // 12"
id = 277 + 2;

module full_surround() {
    difference() {
        cylinder(d=od, h=height);
        cylinder(d=id, h=height);
    }
}

module section() {
    intersection() {
        full_surround();
        cube(500);
    }
}

module pin_holes() {
    offset = 15;
    for (z = [offset, height - offset]) {
        translate([0, od/2 - (od-id)/2/2, z])
        rotate([0,90,0])
        pinhole(fixed=true);
    }
}

difference() {
    section();
    pin_holes();
    rotate([0,0,-90])
    mirror([1,0,0])
    pin_holes();

    // just to test the fit
    //translate([0,0,10])
    //cube(500);
}

for (off = [0,15]) {
    translate([off,0,0])
    pinpeg();
}

//pin();
//
//translate([10,0,0])
//nub(1.5, 1);
//
//translate([20,0,0])
//pinhole(fixed=true);
//
//translate([30,0,0])
//pinpeg();
