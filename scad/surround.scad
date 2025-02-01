$fn = 200;

use <pins/pin2.scad>
use <common.scad>

legs = 50.8; // 2"
tier3_height = 12;
tier2_height = 22;
tier1_height = 27.2;
lip = 3;
height = tier1_height + legs + lip;
od = 304.8; // 12"
tier3_dia = 277;
tier2_dia = tier3_dia - 2*5;
tier1_dia = tier3_dia - 2*19;
dia_tolerance = 2;
id = tier3_dia + dia_tolerance;

glass = 5;
// glass_dia = 247;
glass_dia = tier1_dia + 2*glass;

module base() {
    cylinder(h=tier3_height, d=tier3_dia);
    cylinder(h=tier2_height, d=tier2_dia);
    cylinder(h=tier1_height, d=tier1_dia);
    // cloche
    cylinder(h=100, d=glass_dia);
}

module led_keepout() {
    // TODO -- lip even with tier 3 or slightly above?
    height = tier1_height - tier2_height;
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
        cylinder(d=glass_dia + dia_tolerance, h=height);
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

module section_w_holes() {
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
}

section_w_holes();

for (off = [0,15]) {
    translate([off,0,0])
    pinpeg();
}

extra_height = 8;

module extra_foot() {
    extra_extra = 5;

    translate([0,0,-extra_height])
    difference() {
        cylinder(h=extra_height + extra_extra, d=od+10);
	cylinder(h=extra_height + extra_extra, d=id);
    }
}

module extra_foot_section() {
    intersection() {
        extra_foot();
	translate([0,0,-extra_height])
	cube(500);
    }
}

extra_foot_section();

//color("red")
//translate([0,0,legs])
//base();

// for testing
//intersection() {
//    section_w_holes();
//    rotate([0,0,90 - 30])
//    section_w_holes();
//}

//led_keepout();

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
