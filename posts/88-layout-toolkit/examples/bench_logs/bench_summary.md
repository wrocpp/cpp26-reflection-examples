
### variant O2  (3 processes x samples; wall = steady_clock, cpu = thread CPU time)


**pass `if`, N = 1000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.766 (0.756, 0.775, 0.766) | 0.752-0.782 | 0.768 | 41.8 | 1.00x |
| aos24_OrderGood | 24 | 0.771 (0.779, 0.771, 0.770) | 0.761-1.082 | 0.772 | 31.1 | 1.01x |
| aos24_reordered_OrderBad | 24 | 0.776 (0.788, 0.776, 0.773) | 0.763-0.826 | 0.775 | 30.9 | 1.01x |
| aos24_reordered_OrderGood_control | 24 | 0.764 (0.777, 0.758, 0.764) | 0.754-1.127 | 0.763 | 31.4 | 1.00x |
| split_hot24_cold1 | 24 | 0.770 (0.794, 0.770, 0.770) | 0.758-0.797 | 0.774 | 31.2 | 1.01x |
| soa | 17 | 0.774 (0.791, 0.770, 0.774) | 0.761-0.872 | 0.774 | 22.0 | 1.01x |

**pass `if`, N = 100000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 3.001 (3.070, 3.001, 2.968) | 2.921-3.152 | 2.998 | 10.7 | 1.00x |
| aos24_OrderGood | 24 | 3.072 (3.072, 3.091, 2.947) | 2.921-3.131 | 3.032 | 7.8 | 1.02x |
| aos24_reordered_OrderBad | 24 | 3.000 (3.101, 3.000, 2.968) | 2.884-3.222 | 2.999 | 8.0 | 1.00x |
| aos24_reordered_OrderGood_control | 24 | 3.013 (3.041, 3.013, 2.996) | 2.940-3.270 | 3.018 | 8.0 | 1.00x |
| split_hot24_cold1 | 24 | 3.010 (3.010, 3.062, 2.966) | 2.954-3.237 | 3.016 | 8.0 | 1.00x |
| soa | 17 | 3.041 (3.125, 3.041, 3.029) | 2.998-3.746 | 3.041 | 5.6 | 1.01x |

**pass `if`, N = 10000000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 3.159 (3.149, 3.161, 3.159) | 3.110-3.187 | 3.155 | 10.1 | 1.00x |
| aos24_OrderGood | 24 | 3.100 (3.098, 3.100, 3.103) | 3.095-3.245 | 3.100 | 7.7 | 0.98x |
| aos24_reordered_OrderBad | 24 | 3.098 (3.096, 3.099, 3.098) | 3.093-3.136 | 3.098 | 7.7 | 0.98x |
| aos24_reordered_OrderGood_control | 24 | 3.099 (3.096, 3.099, 3.099) | 3.094-3.143 | 3.097 | 7.7 | 0.98x |
| split_hot24_cold1 | 24 | 3.099 (3.099, 3.096, 3.150) | 3.093-3.527 | 3.101 | 7.7 | 0.98x |
| soa | 17 | 3.076 (3.076, 3.073, 3.098) | 3.064-3.385 | 3.076 | 5.5 | 0.97x |

**pass `mask`, N = 1000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.507 (0.503, 0.508, 0.507) | 0.501-0.517 | 0.505 | 63.2 | 1.00x |
| aos24_OrderGood | 24 | 0.500 (0.523, 0.500, 0.499) | 0.491-1.003 | 0.503 | 48.0 | 0.99x |
| aos24_reordered_OrderBad | 24 | 0.495 (0.499, 0.493, 0.495) | 0.492-0.501 | 0.495 | 48.5 | 0.98x |
| aos24_reordered_OrderGood_control | 24 | 0.494 (0.508, 0.494, 0.493) | 0.491-0.512 | 0.495 | 48.6 | 0.98x |
| split_hot24_cold1 | 24 | 0.529 (0.547, 0.526, 0.529) | 0.525-0.555 | 0.529 | 45.4 | 1.04x |
| soa | 17 | 0.326 (0.350, 0.326, 0.319) | 0.319-0.620 | 0.326 | 52.1 | 0.64x |

**pass `mask`, N = 100000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.478 (0.482, 0.478, 0.464) | 0.461-0.572 | 0.475 | 67.0 | 1.00x |
| aos24_OrderGood | 24 | 0.498 (0.498, 0.583, 0.491) | 0.483-0.681 | 0.498 | 48.2 | 1.04x |
| aos24_reordered_OrderBad | 24 | 0.500 (0.500, 0.504, 0.491) | 0.486-0.565 | 0.496 | 48.0 | 1.05x |
| aos24_reordered_OrderGood_control | 24 | 0.497 (0.497, 0.500, 0.493) | 0.489-0.564 | 0.496 | 48.2 | 1.04x |
| split_hot24_cold1 | 24 | 0.512 (0.516, 0.512, 0.511) | 0.505-0.628 | 0.511 | 46.9 | 1.07x |
| soa | 17 | 0.402 (0.396, 0.402, 0.404) | 0.394-0.423 | 0.401 | 42.3 | 0.84x |

**pass `mask`, N = 10000000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.561 (0.578, 0.550, 0.561) | 0.549-0.877 | 0.561 | 57.0 | 1.00x |
| aos24_OrderGood | 24 | 0.489 (0.470, 0.489, 0.490) | 0.468-0.621 | 0.480 | 49.1 | 0.87x |
| aos24_reordered_OrderBad | 24 | 0.473 (0.473, 0.474, 0.471) | 0.468-0.494 | 0.473 | 50.8 | 0.84x |
| aos24_reordered_OrderGood_control | 24 | 0.485 (0.485, 0.472, 0.497) | 0.469-0.575 | 0.482 | 49.5 | 0.86x |
| split_hot24_cold1 | 24 | 0.476 (0.476, 0.469, 0.491) | 0.468-0.513 | 0.476 | 50.4 | 0.85x |
| soa | 17 | 0.402 (0.401, 0.422, 0.402) | 0.400-0.435 | 0.404 | 42.3 | 0.72x |

**pass `all`, N = 1000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.626 (0.624, 0.626, 0.626) | 0.618-0.630 | 0.623 | 51.1 | 1.00x |
| aos24_OrderGood | 24 | 0.622 (0.698, 0.619, 0.622) | 0.615-1.183 | 0.623 | 38.6 | 0.99x |
| aos24_reordered_OrderBad | 24 | 0.618 (0.623, 0.616, 0.618) | 0.613-0.626 | 0.618 | 38.8 | 0.99x |
| split_hot24_cold1 | 25 | 0.626 (0.651, 0.625, 0.626) | 0.622-0.663 | 0.627 | 39.9 | 1.00x |
| soa | 18 | 0.646 (0.649, 0.646, 0.637) | 0.635-0.662 | 0.644 | 27.8 | 1.03x |

**pass `all`, N = 100000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.646 (0.646, 0.662, 0.638) | 0.636-0.669 | 0.646 | 49.5 | 1.00x |
| aos24_OrderGood | 24 | 0.635 (0.635, 0.635, 0.629) | 0.626-0.895 | 0.634 | 37.8 | 0.98x |
| aos24_reordered_OrderBad | 24 | 0.633 (0.636, 0.633, 0.629) | 0.626-0.642 | 0.633 | 37.9 | 0.98x |
| split_hot24_cold1 | 25 | 0.649 (0.645, 0.650, 0.649) | 0.637-0.657 | 0.648 | 38.5 | 1.00x |
| soa | 18 | 0.668 (0.668, 0.675, 0.659) | 0.655-0.711 | 0.666 | 27.0 | 1.03x |

**pass `all`, N = 10000000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.669 (0.679, 0.667, 0.669) | 0.657-0.760 | 0.669 | 47.8 | 1.00x |
| aos24_OrderGood | 24 | 0.633 (0.631, 0.633, 0.635) | 0.629-0.646 | 0.633 | 37.9 | 0.95x |
| aos24_reordered_OrderBad | 24 | 0.635 (0.635, 0.632, 0.638) | 0.629-0.661 | 0.635 | 37.8 | 0.95x |
| split_hot24_cold1 | 25 | 0.650 (0.650, 0.650, 0.654) | 0.648-0.665 | 0.650 | 38.5 | 0.97x |
| soa | 18 | 0.652 (0.652, 0.688, 0.652) | 0.650-0.708 | 0.653 | 27.6 | 0.97x |

### variant O2_novec  (3 processes x samples; wall = steady_clock, cpu = thread CPU time)


**pass `if`, N = 1000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.775 (0.775, 0.771, 0.778) | 0.756-0.789 | 0.775 | 41.3 | 1.00x |
| aos24_OrderGood | 24 | 0.774 (0.776, 0.774, 0.772) | 0.768-0.790 | 0.773 | 31.0 | 1.00x |
| aos24_reordered_OrderBad | 24 | 0.780 (0.775, 0.780, 0.780) | 0.763-0.793 | 0.778 | 30.8 | 1.01x |
| aos24_reordered_OrderGood_control | 24 | 0.775 (0.773, 0.788, 0.775) | 0.768-0.833 | 0.775 | 30.9 | 1.00x |
| split_hot24_cold1 | 24 | 0.775 (0.769, 0.789, 0.775) | 0.765-1.317 | 0.776 | 31.0 | 1.00x |
| soa | 17 | 0.782 (0.771, 0.784, 0.782) | 0.770-0.803 | 0.779 | 21.7 | 1.01x |

**pass `if`, N = 100000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 2.973 (2.973, 3.037, 2.958) | 2.935-3.084 | 2.983 | 10.8 | 1.00x |
| aos24_OrderGood | 24 | 2.964 (2.937, 3.062, 2.964) | 2.928-3.091 | 2.970 | 8.1 | 1.00x |
| aos24_reordered_OrderBad | 24 | 2.961 (2.938, 2.961, 3.026) | 2.930-3.086 | 2.958 | 8.1 | 1.00x |
| aos24_reordered_OrderGood_control | 24 | 2.947 (2.941, 2.954, 2.947) | 2.930-3.021 | 2.952 | 8.1 | 0.99x |
| split_hot24_cold1 | 24 | 3.026 (3.026, 2.996, 3.026) | 2.942-4.781 | 2.999 | 7.9 | 1.02x |
| soa | 17 | 3.081 (3.012, 3.104, 3.081) | 2.979-3.138 | 3.076 | 5.5 | 1.04x |

**pass `if`, N = 10000000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 3.131 (3.131, 3.118, 3.193) | 3.110-3.288 | 3.152 | 10.2 | 1.00x |
| aos24_OrderGood | 24 | 3.106 (3.106, 3.107, 3.103) | 3.098-3.871 | 3.105 | 7.7 | 0.99x |
| aos24_reordered_OrderBad | 24 | 3.102 (3.101, 3.103, 3.102) | 3.095-3.507 | 3.101 | 7.7 | 0.99x |
| aos24_reordered_OrderGood_control | 24 | 3.100 (3.113, 3.098, 3.100) | 3.095-3.158 | 3.100 | 7.7 | 0.99x |
| split_hot24_cold1 | 24 | 3.100 (3.100, 3.098, 3.101) | 3.096-3.144 | 3.100 | 7.7 | 0.99x |
| soa | 17 | 3.080 (3.079, 3.087, 3.080) | 3.068-3.131 | 3.083 | 5.5 | 0.98x |

**pass `mask`, N = 1000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.506 (0.506, 0.501, 0.513) | 0.499-1.018 | 0.505 | 63.3 | 1.00x |
| aos24_OrderGood | 24 | 0.492 (0.515, 0.492, 0.492) | 0.491-0.523 | 0.494 | 48.8 | 0.97x |
| aos24_reordered_OrderBad | 24 | 0.502 (0.511, 0.502, 0.495) | 0.490-0.514 | 0.501 | 47.8 | 0.99x |
| aos24_reordered_OrderGood_control | 24 | 0.502 (0.502, 0.507, 0.495) | 0.491-0.513 | 0.502 | 47.8 | 0.99x |
| split_hot24_cold1 | 24 | 0.528 (0.526, 0.533, 0.528) | 0.522-0.543 | 0.529 | 45.5 | 1.04x |
| soa | 17 | 0.326 (0.319, 0.326, 0.329) | 0.319-0.332 | 0.326 | 52.2 | 0.64x |

**pass `mask`, N = 100000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.465 (0.465, 0.476, 0.463) | 0.461-0.483 | 0.465 | 68.9 | 1.00x |
| aos24_OrderGood | 24 | 0.488 (0.488, 0.502, 0.488) | 0.481-0.507 | 0.489 | 49.1 | 1.05x |
| aos24_reordered_OrderBad | 24 | 0.489 (0.503, 0.489, 0.488) | 0.476-0.505 | 0.490 | 49.1 | 1.05x |
| aos24_reordered_OrderGood_control | 24 | 0.490 (0.490, 0.497, 0.487) | 0.485-0.550 | 0.489 | 49.0 | 1.05x |
| split_hot24_cold1 | 24 | 0.506 (0.506, 0.503, 0.510) | 0.501-0.516 | 0.504 | 47.4 | 1.09x |
| soa | 17 | 0.397 (0.405, 0.396, 0.397) | 0.390-0.412 | 0.398 | 42.8 | 0.85x |

**pass `mask`, N = 10000000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.553 (0.551, 0.553, 0.556) | 0.549-0.575 | 0.554 | 57.9 | 1.00x |
| aos24_OrderGood | 24 | 0.473 (0.474, 0.470, 0.473) | 0.468-0.487 | 0.472 | 50.8 | 0.86x |
| aos24_reordered_OrderBad | 24 | 0.483 (0.483, 0.485, 0.475) | 0.470-0.516 | 0.480 | 49.7 | 0.87x |
| aos24_reordered_OrderGood_control | 24 | 0.473 (0.485, 0.470, 0.473) | 0.470-0.504 | 0.473 | 50.8 | 0.86x |
| split_hot24_cold1 | 24 | 0.473 (0.476, 0.469, 0.473) | 0.469-0.492 | 0.474 | 50.8 | 0.86x |
| soa | 17 | 0.402 (0.404, 0.401, 0.402) | 0.399-0.409 | 0.403 | 42.3 | 0.73x |

**pass `all`, N = 1000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.621 (0.621, 0.620, 0.621) | 0.617-0.717 | 0.621 | 51.6 | 1.00x |
| aos24_OrderGood | 24 | 0.608 (0.621, 0.605, 0.608) | 0.604-1.056 | 0.608 | 39.5 | 0.98x |
| aos24_reordered_OrderBad | 24 | 0.610 (0.610, 0.619, 0.610) | 0.606-0.872 | 0.613 | 39.3 | 0.98x |
| split_hot24_cold1 | 25 | 0.619 (0.619, 0.632, 0.616) | 0.615-0.642 | 0.620 | 40.4 | 1.00x |
| soa | 18 | 0.636 (0.636, 0.658, 0.635) | 0.633-0.668 | 0.637 | 28.3 | 1.03x |

**pass `all`, N = 100000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.645 (0.645, 0.658, 0.637) | 0.635-0.660 | 0.644 | 49.6 | 1.00x |
| aos24_OrderGood | 24 | 0.622 (0.622, 0.634, 0.620) | 0.619-0.644 | 0.622 | 38.6 | 0.96x |
| aos24_reordered_OrderBad | 24 | 0.622 (0.624, 0.622, 0.621) | 0.619-0.630 | 0.622 | 38.6 | 0.97x |
| split_hot24_cold1 | 25 | 0.645 (0.630, 0.649, 0.645) | 0.628-0.650 | 0.642 | 38.8 | 1.00x |
| soa | 18 | 0.665 (0.665, 0.660, 0.736) | 0.652-0.963 | 0.664 | 27.1 | 1.03x |

**pass `all`, N = 10000000**

| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |
|---|---|---|---|---|---|---|
| aos32_OrderBad | 32 | 0.662 (0.663, 0.661, 0.662) | 0.657-0.667 | 0.662 | 48.4 | 1.00x |
| aos24_OrderGood | 24 | 0.630 (0.630, 0.630, 0.632) | 0.627-0.663 | 0.631 | 38.1 | 0.95x |
| aos24_reordered_OrderBad | 24 | 0.635 (0.635, 0.631, 0.640) | 0.628-0.666 | 0.633 | 37.8 | 0.96x |
| split_hot24_cold1 | 25 | 0.648 (0.648, 0.644, 0.648) | 0.643-0.665 | 0.646 | 38.6 | 0.98x |
| soa | 18 | 0.651 (0.656, 0.651, 0.650) | 0.649-1.152 | 0.652 | 27.7 | 0.98x |
