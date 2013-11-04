# osm-pixel

![](http://farm8.staticflickr.com/7307/10637815984_4d231298c8_b.jpg)

A libosmium-based utility that accepts an osm database as pbf and writes a
png of node locations.

## requirements

* libosmium
* boost, with [gil](http://www.boost.org/doc/libs/1_54_0/libs/gil/doc/index.html)

## build

    make

## run

    ./osm_pixel mexico.pbf
