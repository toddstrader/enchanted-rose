$fn = 100;

use <common.scad>

module head() {
    width = 70;

    intersection() {
        tire(34, width, radius_frac=6);
	union() {
            cylinder(h=15, d1=20, d2=width);
            translate([0,0,15])
            cylinder(h=100, d=width);
	}
    }
}

module cavity() {
    translate([0,0,3])
    cylinder(h=5, d=35);
    cylinder(h=5, d=8);
}

module negative_space() {
    stem_hole_height = 5;

    recess(34, extra_depth=3);
    for (ang = [0 : 90 : 360]) {
        rotate([0,0,ang])
	translate([0,-15,0])
        recess(34, 0, 50, extra_depth=4);
    }
    for (ang = [45 : 90 : 360 + 45]) {
        rotate([0,0,ang])
	translate([0,-25,0])
        recess(34, 0, 75, extra_depth=5);
    }
    translate([0,0,stem_hole_height])
    cavity();
    stem_hole(stem_hole_height);
}

module finished_head() {
    difference() {
        head();
	negative_space();
    }
}

finished_head();

//translate([70,0,0])
//negative_space();
//
//translate([-70,0,0])
//union() {
//    head();
//    color("red")
//    negative_space();
//}
