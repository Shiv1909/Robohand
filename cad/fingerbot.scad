/*
  DIY Fingerbot — mechanical parts (Phase 4)

  Three printed parts:
    cradle  — holds the MG90S, flat back for VHB tape onto the wall
    arm     — lever that pushes/pulls the rocker. Print 20, 25 and 30 mm.
    tab     — small adhesive pad for the rocker, with a tether hole

  ---------------------------------------------------------------------------
  THESE DIMENSIONS COME FROM DATASHEETS, NOT FROM MEASURING THE REAL PARTS.
  Print ONE cradle and ONE arm first, test-fit them against the actual servo,
  then adjust the numbers below before printing anything else. Everything is
  parametric precisely so that's a one-line change, not a redesign.
  ---------------------------------------------------------------------------

  Render one part at a time:
      openscad -D 'part="cradle"' -o cradle.stl fingerbot.scad
      openscad -D 'part="arm"' -D 'arm_len=20' -o arm20.stl fingerbot.scad
      openscad -D 'part="tab"' -o tab.stl fingerbot.scad

  Print in PETG, not PLA. This part holds force against a wall for months and
  PLA creeps under sustained load and softens in a warm room.

  ORIENTATION, once mounted: the servo lies on its side below the switch, with
  its output shaft axis horizontal and parallel to the wall, so the arm sweeps
  in a vertical plane perpendicular to the wall — pushing the rocker in, and
  pulling it back out via a tether to the tab. See project.md, "The rocker
  problem".
*/

part = "all";  // [all, cradle, arm, tab]

/* [MG90S servo — verify against your actual part] */
// body, excluding mounting flange
servo_l     = 22.8;   // long axis of the body; runs VERTICALLY on the wall
servo_w     = 12.2;   // thin axis; this becomes depth OUT from the wall
servo_h     = 22.5;   // along the output shaft axis; runs horizontally

/* [Arm] */
arm_len       = 20;    // hub centre to tip. Print 20 / 25 / 30 and test.
arm_w         = 8;     // beam width
arm_t         = 4;     // beam thickness
hub_d         = 13;    // outer diameter of the arm's hub
hub_h         = 6.5;   // hub height — must exceed horn_hub_h

// The arm grips the servo's STOCK plastic horn rather than the spline itself.
// A 20-tooth spline at ~4.8 mm is below reliable FDM resolution, and a printed
// one would strip. These are the horn's dimensions, so measure them first.
horn_hub_d    = 7.4;   // round boss at the centre of the stock horn
horn_hub_h    = 4.2;
horn_blade_w  = 5.4;   // width of the single-arm blade
horn_blade_l  = 20;    // how far the blade reaches from centre
horn_blade_t  = 1.9;   // blade thickness
shaft_screw_d = 2.4;   // the servo's own centre retaining screw

tip_hole_d    = 2.2;   // tether hole at the arm tip

/* [Cradle] */
wall  = 2.4;   // shell thickness
clear = 0.4;   // print clearance around the servo body
lip   = 7;     // base flange either side, carries the zip-tie slots
zip_w = 3.6;   // zip tie width
zip_t = 1.8;   // zip tie thickness

/* [Rocker tab] */
tab_l = 14;
tab_w = 10;
tab_t = 3;

/* [Hidden] */
$fn = 48;
eps  = 0.01;

// ---------------------------------------------------------------------------

// Tray the servo drops into. Flat underside (z=0) is the adhesive face.
// Held by two zip ties rather than screws: the flange screw positions are the
// least certain dimension here, and zip ties don't care if we're 1 mm out.
module servo_cradle() {
    pocket_x = servo_h + 2 * clear;
    pocket_y = servo_l + 2 * clear;
    body_x   = pocket_x + 2 * wall;
    body_y   = pocket_y + 2 * wall;
    total_z  = wall + servo_w + clear;

    difference() {
        union() {
            // base plate, extended sideways to carry the zip slots
            translate([0, -lip, 0])
                cube([body_x, body_y + 2 * lip, wall]);
            // walls
            cube([body_x, body_y, total_z]);
        }

        // servo pocket, open at the top
        translate([wall, wall, wall])
            cube([pocket_x, pocket_y, servo_w + clear + 1]);

        // shaft side left fully open so the horn and arm clear the shell
        translate([body_x - wall - eps, wall, wall])
            cube([wall + 1, pocket_y, servo_w + clear + 1]);

        // cable exit at the opposite end
        translate([-1, body_y / 2 - 4, wall + 2])
            cube([wall + 2, 8, servo_w]);

        // zip-tie slots through the base flange
        for (x = [body_x * 0.3, body_x * 0.7])
            for (y = [-lip / 2, body_y + lip / 2])
                translate([x - zip_w / 2, y - zip_t / 2, -1])
                    cube([zip_w, zip_t, wall + 2]);
    }
}

// The lever. Hub captures the stock horn; tip has a hole for the tether.
module fingerbot_arm(len = arm_len) {
    difference() {
        union() {
            cylinder(d = hub_d, h = hub_h);
            translate([0, -arm_w / 2, 0])
                cube([len, arm_w, arm_t]);
            translate([len, 0, 0])
                cylinder(d = arm_w, h = arm_t);
        }

        // recess for the stock horn's central boss
        translate([0, 0, -eps])
            cylinder(d = horn_hub_d, h = horn_hub_h);

        // slot for the horn's blade, radially along +X
        translate([0, -horn_blade_w / 2, -eps])
            cube([horn_blade_l, horn_blade_w, horn_blade_t]);

        // servo's own centre screw passes right through
        translate([0, 0, -1])
            cylinder(d = shaft_screw_d, h = hub_h + 2);

        // tether hole at the tip
        translate([len, 0, -1])
            cylinder(d = tip_hole_d, h = arm_t + 2);
    }
}

// Sticks to the rocker face. The tether ties through the hole, letting the arm
// PULL as well as push — which is what makes one contact point switch both ways.
module rocker_tab() {
    difference() {
        cube([tab_l, tab_w, tab_t]);
        translate([tab_l - 4, tab_w / 2, -1])
            cylinder(d = tip_hole_d, h = tab_t + 2);
    }
}

// ---------------------------------------------------------------------------

if (part == "all") {
    servo_cradle();
    translate([0, 55, 0])  fingerbot_arm(20);
    translate([0, 70, 0])  fingerbot_arm(25);
    translate([0, 85, 0])  fingerbot_arm(30);
    translate([0, 100, 0]) rocker_tab();
} else if (part == "cradle") {
    servo_cradle();
} else if (part == "arm") {
    fingerbot_arm(arm_len);
} else if (part == "tab") {
    rocker_tab();
}
