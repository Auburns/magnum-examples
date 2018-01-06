This example demonstrates usage of cube map textures and different texture
layers for simulation of open world with two tarnish reflective spheres in
front of the camera. This example also demonstrates usage of scene graph,
resource manager and pre-made primitives.

![Cube Map](cubemap.png)

Usage
-----

The application tries to load cube map texture from the directory in which it
is run. Cube map texture consists of six JEPG files `+x.jpg`, `-x.jpg`,
`+y.jpg`, `-y.jpg`, `+z.jpg` and `-z.jpg`, each representing one side of the
cube. Note that all images must be turned upside down (+Y is top):

              +----+
              | -Y |
    +----+----+----+----+
    | -Z | -X | +Z | +X |
    +----+----+----+----+
              | +Y |
              +----+

You can also pass file path prefix to the application as parameter, for
example

    ./magnum-cubemap ~/images/city

The application will then load `~/images/city+x.jpg`, `~/images/city-x.jpg`
etc. as a cube map texture.

Sample cube map files are supplied alongside the source. If you install the
examples, the images are also copied into
`<prefix>/share/magnum/examples/cubemap/`. Running the example with the bundled
files can be then done like this:

    ./magnum-cubemap <path-to-example-source>/

Key controls
------------

**Arrow keys** *rotate* the camera around the spheres. It is not possible, due
to nature of the cube map projection, to *move* around the scene.

Credits
-------

The sample images provided in this example are work of Emil Persson, aka Humus --
http://www.humus.name, licensed under a [Creative Commons Attribution 3.0 Unported License](http://creativecommons.org/licenses/by/3.0/).

Ports
-----

The [ports branch](https://github.com/mosra/magnum-examples/tree/ports/src/cubemap)
contains additional patches for iOS support that aren't present in `master` in
order to keep the example code as simple as possible.
