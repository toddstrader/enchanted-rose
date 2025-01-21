$fn = 100;

use <common.scad>

module head() {
    intersection() {
        tire(37, 50, radius_frac=6);
	union() {
	    cylinder(h=15, d1=20, d2=50);
	    translate([0,0,15])
	    cylinder(h=100, d=50);
	}
    }
}

module cavity() {
    translate([0,0,3])
    cylinder(h=3, d=20);
    cylinder(h=3, d=8);
}

module negative_space() {
    stem_hole_height = 5;

    recess(37, extra_depth=3);
    for (ang = [0 : 90 : 360]) {
        rotate([0,0,ang])
        recess(40, 17, 1, extra_depth=3);
    }
    for (ang = [45 : 90 : 360 + 45]) {
        rotate([0,0,ang])
        recess(43, 28, 7, extra_depth=3);
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
