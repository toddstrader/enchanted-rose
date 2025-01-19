$fn = 100;

dia = 25;

module stem_hole() {
    stem_dia = 10;
    cylinder(h=5, d=stem_dia);
}

module recess() {
    mag_dia = 6.35;
    line_dia = 3.175;
    
    translate([0,0,10]) cylinder(h=15, d=mag_dia);
    cylinder(h=25, d=line_dia);
}

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