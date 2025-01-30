$fn = 100;

use <common.scad>

width = 15;
thickness = 5;
height = 10;
length = 20;

module mount() {
    cube([width, length, thickness]);
    cube([width, 5, height + thickness]);
}

module switch_hole() {
    translate([width/2,0,thickness + height / 2])
    rotate([-90,0,0])
    cylinder(h=thickness, d=6);
}

difference () {
    mount();
    switch_hole();
    translate([width/2,length-7,0])
    mount_screw_hole(thickness);
}
