# Computer Graphics

## Building the Project

To build the project into an executable just type the following command into
the terminal when you are in the root directory.

    make
    
## Running the Project

Running the project requires that you have an `.obj` model file available.
There should be an example teapot and aeroplane model in the root directory.
Once you have this you can run the program by typing the following command into
the terminal.

    ./viewer <name of model file>

## Moving the Model in the View

There are a number of keyboard controls that you can use to manipulate the
object.

* Up/Down/Left/Right Arrow = Rotate the Object
* W/A/S/D = Translate the object
* Z/X = Zoom Out/In

## Completed Items

The submitted program implements Gouraud shading of a teapot over a floor. It
also includes 2x2 anti-aliasing on the rendered image. The included model file
includes a floor below the teapot.
