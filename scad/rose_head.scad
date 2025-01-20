$fn = 100;

use <common.scad>

module layer(h, d) {
    radius = h/3;
    cylinder_height = h - 2*radius;
    cylinder_dia = d - 2*radius;

    translate([0,0,radius])
    minkowski() {
      cylinder(h=cylinder_height, d=cylinder_dia);
      sphere(radius);
    }
}

module head() {
    layer(18, 50);
    translate([0,0,8]) layer(15, 40);
    translate([0,0,17]) layer(10, 30);
}

module cavity() {
    translate([0,0,3])
    cylinder(h=3, d=20);
    cylinder(h=3, d=8);
}

module negative_space() {
    stem_hole_height = 5;

    recess(27);
    for (ang = [0 : 90 : 360]) {
        rotate([0,0,ang])
        recess(31, 28, 5);
    }
    for (ang = [45 : 90 : 360 + 45]) {
        rotate([0,0,ang])
        recess(31, 45, 18);
    }
    translate([0,0,stem_hole_height])
    cavity();
    stem_hole(stem_hole_height);
}

module finished_head() {
    difference() {
    //union() {
        head();
	negative_space();
    }
}

finished_head();

//translate([50,0,0])
//negative_space();

//translate([0,0,2])
//minkowski() {
////union() {
//    cylinder(h=10, d=6);
//    sphere(2);
//}
