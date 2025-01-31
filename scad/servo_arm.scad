$fn = 25;

new_length = 30;

module old_arm() {
    thickness = 2;
    dia = 7;
    arm_width = 5.5;
    length = 19.5;

    cylinder(d=dia, h=thickness);
    translate([0, arm_width/-2, 0])
    cube([length - dia/2, arm_width, thickness]);
}

module screw_hole() {
    screw_dia = 3.5;
    cylinder(d=screw_dia, h=100);
}

module new_arm() {
    thickness = 4;
    dia = 12;
    arm_width = 9;
    length = new_length + dia/2;

    cylinder(d=dia, h=thickness);
    translate([0, arm_width/-2, 0])
    cube([length - dia/2 - arm_width/2, arm_width, thickness]);
    translate([new_length - arm_width/2,0,0])
    cylinder(d=arm_width, h=thickness);
}

module string_hole() {
    translate([new_length-2.5,0,0])
    cylinder(h=100, d=2);
}

//difference() {
//    old_arm();
//    screw_hole();
//}

difference() {
    new_arm();
    old_arm();
    screw_hole();
    string_hole();
}
