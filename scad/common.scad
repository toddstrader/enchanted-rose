module mount_screw_hole(depth) {
    screw_dia = 4;
    taper_depth = 2;

    cylinder(h=depth, d=screw_dia);
    translate([0, 0, depth - taper_depth])
    cylinder(h=taper_depth, d1=screw_dia, d2=screw_dia*1.5);
}

module stem_hole(h=5) {
    stem_dia = 9.5;
    cylinder(h=h, d=stem_dia);
}

module recess(h=25, rot=0, line_rot=0, extra_depth=0, line_dia=3.175, extra_line_depth=0) {
    mag_dia = 6.35;
    mag_depth = 15 + extra_depth;
    line_depth = h - mag_depth;
    
    rotate([rot,0,0])
    translate([0,0,h-mag_depth])
    {
        cylinder(h=mag_depth, d=mag_dia);
	rotate([line_rot,0,0])
	translate([0,0,-line_depth])
        cylinder(h=line_depth + 5 + extra_line_depth, d=line_dia);
    }
}

module tire(h, d, radius_frac=3) {
    radius = h/radius_frac;
    cylinder_height = h - 2*radius;
    cylinder_dia = d - 2*radius;

    translate([0,0,radius])
    minkowski() {
      cylinder(h=cylinder_height, d=cylinder_dia);
      sphere(radius);
    }
}
