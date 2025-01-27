$fn = 100;

use <common.scad>

module head() {
    width = 50;

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

    recess(34, extra_depth=3, line_dia=4.5);
    for (ang = [0 : 90 : 360]) {
        rotate([0,0,ang])
	translate([0,-10,0])
        recess(34, 0, 50, extra_depth=4, line_dia=4.5);
    }
    for (ang = [45*1.5 : 45 : 360 + 45*1.5]) {
        rotate([0,0,ang])
	translate([0,-20,0])
        recess(34, 0, 75, extra_depth=5, line_dia=4.5, extra_line_depth=-3);
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

//translate([80,0,0])
//negative_space();
//
//translate([-80,0,0])
//union() {
//    head();
//    color("red")
//    negative_space();
//}
