vcable
------

Audio plugin acting as virtual routing cable (client for sound server)
Basically it lets you plug any program middle of DSP chain.

supported sound servers
-----------------------

Currently there exists plugins for:

- jack

The default search path for plugins is /usr/{lib,<multiarch>}/vcable.
You can also specify additional absolute search path with VCABLE_PATH env variable.

supported hosts
---------------

Currently vcable can be compiled as one of the following hosts:

- ladspa

BUILDING
--------

.. code:: sh

    mkdir target && cd target                            # - create build target directory
    cmake -DCMAKE_BUILD_TYPE=Upstream ..                 # - run CMake
    make                                                 # - compile
