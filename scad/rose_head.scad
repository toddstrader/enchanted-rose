module layer(h) {
    minkowski() {
      cylinder(h=h, d=4*h);
      // rounded corners
      sphere(h/2);
    }
}

module recess() {
    mag_dia = 5.55625;
    line_dia = 3.175;
    mag_depth = 15;
    line_depth = 30;
    
    translate([0,0,line_depth-mag_depth]) cylinder(h=mag_depth, d=mag_dia);
    translate([0,0,mag_depth-line_depth]) cylinder(h=line_depth, d=line_dia);
}

module head() {
    layer(10);
    translate([0,0,10]) layer(8);
    translate([0,0,19]) layer(5);
}

difference() {
//union() {
    head();
    translate([0,0,-3.5]) recess();
    translate([17,0,-3.5]) recess();
}