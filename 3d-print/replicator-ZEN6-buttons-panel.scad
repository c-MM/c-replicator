/* service buttons panel replicator FK-266
vim: ai ts=2
*/

$fn = 100;

difference(){
	translate([-2,-2,  0]) cube([ 66.60, 14.00, 25.00]);
	translate([ 0, 0, -1]) cube([ 62.60, 10.00, 27.00]);
	translate([ 11.15, 1,  12.25]) rotate([ 90,  0,  0]) cylinder(r=4.2, h=4);
	translate([ 34.30, 1,  12.25]) rotate([ 90,  0,  0]) cylinder(r=4.2, h=4);
	translate([ 51.70, 1,  12.25]) rotate([ 90,  0,  0]) cylinder(r=4.2, h=4);
	translate([  8.75, -1.20,  12.90]) cube([ 4.80, 2.00, 12.25]);
	translate([ 31.90, -1.20,  12.90]) cube([ 4.80, 2.00, 12.25]);
	translate([ 49.30, -1.20,  12.90]) cube([ 4.80, 2.00, 12.25]);
}

	translate([   0,    0,     0]) cube([  2.00, 6.00, 25.00]);
	translate([   0,    7.70,  0]) cube([  2.00, 3.00, 25.00]);
	translate([  60.60, 0,     0]) cube([  2.00, 6.00, 25.00]);
	translate([  33.30, 7.70,  0]) cube([  2.00, 3.00, 25.00]);
	translate([  60.60, 7.70,  0]) cube([  2.00, 3.00, 25.00]);

