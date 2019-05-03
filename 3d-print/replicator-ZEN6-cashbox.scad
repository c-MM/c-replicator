/* cashbox replicator FK-266
vim: ai ts=2
*/

$fn = 100;

difference(){
  translate([  -0,  -0,  -0]) minkowski() {
		cube([ 104, 104, 180], false );
		sphere(2);
	}
	translate([   4,   4,   4]) minkowski() {
		cube([  96,  96, 180], false );
		sphere(4);
	}
}

