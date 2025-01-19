wall_width = 9;
servo_width = 12.5;
servo_height = 22.5 + 0.5;
screw_offset = 2;
screw_dia = 1;
border = 10;
tab = 5;
tab_clearance = 3;

assembly_height = servo_height + border*2;
assembly_width = servo_width*2 + border*2;
mount_width = border - tab;
mount_length = border*5;

$fn = 50;

include <common.scad>

module housing() {
    difference() {
        cube(size=[wall_width, assembly_width, assembly_height]);
        translate([0, border, border])
        cube(size=[wall_width, servo_width*2, servo_height]);
    }
}

module assembly() {
    translate([0, assembly_width/-2, 0])
    union() {
        translate([wall_width/-2, 0, tab_clearance])
        housing();
        translate([mount_length/-2, 0, 0])
        cube([mount_length, assembly_width, mount_width]);
    }
}

module screw_hole() {
    translate([wall_width/-2, 0 ,0])
    rotate([0, 90, 0])
    cylinder(h=wall_width, d=screw_dia);
}

module finished_assembly() {
    difference() {
        assembly();
        for(x=[1, -1]) {
            for (y=[1, -1]) {
                translate([x * mount_length / 2.8, y * assembly_width / 3, 0])
                mount_screw_hole(depth=mount_width);
                
                translate([0, x * servo_width / 2, y * (servo_height / 2 + screw_offset)])
                translate([0, 0, tab_clearance + assembly_height/2])
                screw_hole();
            }
        }
    }
}

finished_assembly();
