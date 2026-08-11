# Print Instructions — DIY Fingerbot parts

Hand this to whoever is printing, along with the five `.stl` files in `cad/stl/`.

Everything here is small. **Total is roughly 25 g of filament and about 2 hours**
of printer time for the whole set.

---

## What to print

| File | Qty | What it is |
| --- | --- | --- |
| `cradle.stl` | 1 | Tray that holds the servo. Flat back goes against the wall |
| `arm20.stl` | 1 | Lever, 20 mm |
| `arm25.stl` | 1 | Lever, 25 mm |
| `arm30.stl` | 1 | Lever, 30 mm |
| `tab.stl` | 2 | Small pad that sticks to the light switch |

**Please print all three arm lengths.** They are not alternatives to choose
between on screen — the right length is decided by testing against a real switch,
and the three exist so that test can happen without another print run. They are
about 3 g each.

Two `tab.stl` because they are tiny and one usually gets destroyed removing the
adhesive.

---

## Material: PETG

**Not PLA.** These parts live on a wall for months in a warm room, and PLA loses
stiffness well below its glass transition.

If only PLA is available, print anyway — the parts are still useful for checking
that the dimensions are right. Just say which material was used, because PETG runs
0.1–0.3 mm larger than PLA on small features, which matters here.

---

## Settings

| Setting | Value | Why |
| --- | --- | --- |
| Layer height | 0.2 mm | nothing here needs finer |
| Perimeters / walls | **4** | the arm is structural; walls carry the load, not infill |
| Infill | 40 % | anything above this is wasted on parts this small |
| Supports | **None needed** | see below |
| Scale | **100 %** | do not scale. Dimensions are in mm and must be exact |
| Brim | optional | only if PETG lifts on your bed |

### Orientation — print as the STL comes

All five files are already oriented correctly. Do not rotate them.

- **Arms:** flat, hub and beam on the bed. This is deliberate — the bending load
  then runs *within* the layers rather than across them, so layer adhesion is not
  in the load path. Rotating these upright would make them snap.
- **Cradle:** base plate on the bed, tray opening upward.
- **Tabs:** flat.

### Why no supports

The arms have two cavities in the underside — a 7.4 mm round recess and a 5.4 mm
slot — that the printer bridges over. Both spans are short and bridge cleanly.
Supports inside them would be worse than useless, because leftover support debris
in those cavities stops the arm seating on the servo horn.

The cradle's cable slot is open at the top, so there is nothing to bridge.

---

## What matters most

**The arm's hub is the critical part.** It has a round recess and a narrow slot
that grip the servo's stock plastic horn. Those dimensions came from a datasheet
rather than a measured part, so this print is partly a test of whether they are
right.

If those cavities come out filled in, stringy, or badly under-size, the arm cannot
mount at all. A clean underside on the arms matters more than the finish anywhere
else.

No need to fix anything if it comes out wrong — just say what looked off. The
source is parametric, so corrections are one-line changes and a re-export.

---

## Source

Generated from `cad/fingerbot.scad` (OpenSCAD). Regenerate the STLs with
`cad\render.ps1` after any change to the parameters.
