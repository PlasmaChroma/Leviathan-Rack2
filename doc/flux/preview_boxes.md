Yep — I can give you the exact numbers Codex needs. Your SVG uses:

* `width="101.6mm" height="128.5mm"`
* `viewBox="0 0 10160 12850"`

So **SVG units are 0.01 mm** (divide by 100 to get mm).

---

## CH1_PREVIEW

### Raw SVG attributes (as stored)

* `x = 1343.3462`
* `y = 7893.9668`
* `w = 2125.3311`
* `h = 1167.9719`
* `transform = matrix(0.9967357, 0, 0, 0.99708045, -982.96276, -994.31743)`

### Effective bounds after transform (panel coordinates, in SVG viewBox units)

Because the matrix is scale+translate only (no rotation/shear), the rect becomes:

* `x' = a*x + e = 355.998355`
* `y' = d*y + f = 6876.602539`
* `w' = a*w = 2118.393382`
* `h' = d*h = 1164.561948`

### Effective bounds in **mm**

(divide by 100)

* **x = 3.55998355 mm**
* **y = 68.76602539 mm**
* **w = 21.18393382 mm**
* **h = 11.64561948 mm**

### Inner drawable rect with **0.2 mm inset** each edge

* **x = 3.75998355 mm**
* **y = 68.96602539 mm**
* **w = 20.78393382 mm**
* **h = 11.24561948 mm**

---

## CH4_PREVIEW

### Raw SVG attributes (no transform)

* `x = 7732.5`
* `y = 6876.6001`
* `w = 2118.3933`
* `h = 1164.562`

### Bounds in **mm**

* **x = 77.32500000 mm**
* **y = 68.76600100 mm**
* **w = 21.18393300 mm**
* **h = 11.64562000 mm**

### Inner drawable rect with **0.2 mm inset**

* **x = 77.52500000 mm**
* **y = 68.96600100 mm**
* **w = 20.78393300 mm**
* **h = 11.24562000 mm**

---

### One important note for Codex

`CH1_PREVIEW` **has a transform**, so if Codex is “hesitating” it’s probably reading raw `x/y/w/h` and ignoring the matrix.

Fastest fix options:

1. **Remove the transform** in Inkscape (apply transforms) so CH1_PREVIEW has clean x/y/w/h like CH4, **or**
2. In code, apply the matrix as shown above when computing the rect.

If you want, paste the snippet Codex currently uses to fetch the element coords, and I’ll tell you exactly where to splice in the transform handling (or how to simplify by flattening it in the SVG).
