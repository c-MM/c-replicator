/* test for the mate-n-lok replicator connectors
vim: ai ts=2
*/

//import("./c-0926681-03-b2-2d.dxf");
$fn = 1000;

module te_mate_n_lok_2() {
	dx = 2.41;
	dy = 4.45;
	x = 13.46;
	y = 14.35 + 2 * dy;
	z = 10;
	difference() {
		translate([   0,    0, -1])	cube([ x,  y, z], false);
		translate([   0,    0, -1])	cube([dx, dy, z], false);
		translate([x-dx,    0, -1])	cube([dx, dy, z], false);
		translate([   0, y-dy, -1])	cube([dx, dy, z], false);
		translate([x-dx, y-dy, -1])	cube([dx, dy, z], false);
	};
}

module te_mate_n_lok_3() {
	dx = 2.41;
	dy = 4.45;
	x = 13.46;
	y = 20.70 + 2 * dy;
	z = 10;
	difference() {
		translate([   0,    0, -1])	cube([ x,  y, z], false);
		translate([   0,    0, -1])	cube([dx, dy, z], false);
		translate([x-dx,    0, -1])	cube([dx, dy, z], false);
		translate([   0, y-dy, -1])	cube([dx, dy, z], false);
		translate([x-dx, y-dy, -1])	cube([dx, dy, z], false);
	};
}

module te_mate_n_lok_12() {
	dx = 6.99;
	dy = 4.45;
	x = 26.16;
	y = 27.05 + 2 * dy;
	z = 10;
	difference() {
		translate([   0,    0, -1])	cube([ x,  y, z], false);
		translate([   0,    0, -1])	cube([dx, dy, z], false);
		translate([x-dx,    0, -1])	cube([dx, dy, z], false);
		translate([   0, y-dy, -1])	cube([dx, dy, z], false);
		translate([x-dx, y-dy, -1])	cube([dx, dy, z], false);
	};
}

module te_mate_n_lok_15() {
	dx = 6.99;
	dy = 4.45;
	x = 26.16;
	y = 33.40 + 2 * dy;
	z = 10;
	difference() {
		translate([   0,    0, -1])	cube([ x,  y, z], false);
		translate([   0,    0, -1])	cube([dx, dy, z], false);
		translate([x-dx,    0, -1])	cube([dx, dy, z], false);
		translate([   0, y-dy, -1])	cube([dx, dy, z], false);
		translate([x-dx, y-dy, -1])	cube([dx, dy, z], false);
	};
}

module triangle(o_len, a_len, depth, center=false) {
	centroid = center ? [-a_len/3, -o_len/3, -depth/2] : [0, 0, 0];
	translate(centroid) linear_extrude(height=depth) {
		polygon(points=[[0,0],[a_len,0],[0,o_len]], paths=[[0,1,2]]);
	}
}

difference(){
  translate([  -6,  -12,   0]) cube([ 206, 65, 2], false );
	translate([   3,    1,   0]) te_mate_n_lok_12();
	translate([  40,    1,   0]) te_mate_n_lok_15();
	translate([  80,    1,   0]) te_mate_n_lok_15();
	translate([ 120,    1,   0]) te_mate_n_lok_15();
	translate([ 158,    1,   0]) te_mate_n_lok_3();
	translate([ 180,    1,   0]) te_mate_n_lok_2();
}
difference(){
	translate([  -6,  -12,   0]) cube([ 206,  2, 40], false );
	translate([  14,   -5,  15]) rotate([90, 0, 0]) cylinder(r=2.0, h=10);
	translate([  97,   -5,  28]) rotate([90, 0, 0]) cylinder(r=2.0, h=10);
	translate([ 176,   -5,  15]) rotate([90, 0, 0]) cylinder(r=2.0, h=10);
}

translate([  -6,  -12,    0])    rotate([90,   0,   90]) triangle(12, 12,    206);
translate([  -6,   46.36, 2])    rotate([90,   0,   90]) triangle( 4,  6.63, 206);
translate([  -6,   -7,   38.17]) rotate([90, 211.1, 90]) triangle( 6.3,3.8,  206);
translate([  -6,  -10,    2])    rotate([90,   0  , 90]) triangle(38, 63,      2);
translate([ 198,  -10,    2])    rotate([90,   0,   90]) triangle(38, 63,      2);

