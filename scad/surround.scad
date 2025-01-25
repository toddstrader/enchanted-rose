$fn = 25; // TODO -- > 100

use <pins/pin2.scad>
use <common.scad>

legs = 50.8; // 2"
tier1_height = 12;
tier2_height = 22;
tier3_height = 27.2;
lip = 3;
height = tier3_height + legs + lip;
od = 304.8; // 12"
tier3_dia = 277;
tier2_dia = tier3_dia - 2*5;
tier1_dia = tier3_dia - 2*19;
dia_tolerance = 2;
id = tier3_dia + dia_tolerance;

module led_keepout() {
    // TODO -- lip even with tier 3 or slightly above?
    height = tier3_height - tier2_height;
    translate([0,0,tier2_height + legs])
    cylinder(h=height, d=tier3_dia + 1);
}

module surround_blank() {
    intersection() {
        union() {
	    tire_height = 35;
	    translate([0,0,height-tire_height])
	    tire(tire_height, od);
	    cylinder(d=od, h=height-tire_height/2);
	    translate([0,0,-5])
	    tire(20, od+10);
	}
	n = 500;
	translate([0,0,n/2])
	cube(n, true);
    }
}

module full_surround() {
    difference() {
        surround_blank();
        cylinder(d=id, h=tier2_height + legs);
        cylinder(d=tier1_dia + dia_tolerance, h=height);
	led_keepout();
    }
}

module section() {
    intersection() {
        full_surround();
        cube(500);
    }
}

module pin_holes() {
    offset = 17;
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

//led_keepout();

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
