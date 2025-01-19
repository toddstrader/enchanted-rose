$fn = 100;

mount_height = 15;
flange_height = 5;
flange_dia = 60;
min_wall = 3;

stem_od = 10;
stem_id = 8;

bottom_dia = stem_od + min_wall*2;
top_dia = bottom_dia + (flange_dia - bottom_dia) / 1.5;

use <common.scad>

module stem_hole(h=mount_height) {
    cylinder(h=h, d=stem_od);
}

module flange() {

    cylinder(h=flange_height, d=flange_dia);
    cylinder(h=mount_height, d1=top_dia, d2=bottom_dia);
}

module stop() {
    floor = 3;

    difference() {
        translate([0, 0, mount_height - floor])
        stem_hole(h=floor);
	cylinder(h=mount_height, d=stem_id);
    }
}

module mount() {
    difference() {
        flange();
        stem_hole();
    }
    stop();
}

module screw_holes() {
    offset = 6;
    for (ang = [0 : 90 : 360]) {
        rotate([0, 0, ang])
        translate([flange_dia/2 - offset, 0, 0])
        mount_screw_hole(flange_height);
    }
}

difference() {
    mount();
    screw_holes();
}
