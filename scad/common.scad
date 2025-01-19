module mount_screw_hole(depth) {
    screw_dia = 4;
    taper_depth = 2;

    cylinder(h=depth, d=screw_dia);
    translate([0, 0, depth - taper_depth])
    cylinder(h=taper_depth, d1=screw_dia, d2=screw_dia*1.5);
}
