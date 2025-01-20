$fn = 100;

use <common.scad>

dia = 25;

//translate([80,0,0]) recess();

module top() {
    translate([0,0,15])
    difference() {
        cylinder(h=10, d=dia);
        cylinder(h=10, d=15);
    }
}

difference() {
    cylinder(h=25, d=dia);
    recess();
    stem_hole();
    top();
}

translate([30,0,0])
recess(25, 0, 10);
