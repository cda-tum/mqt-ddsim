Frequently Asked Questions
##########################


**How to run tests?**

The repository also includes some (rudimentary) unit tests (using GoogleTest), which aim to ensure the correct behavior
of the tool. They can be built and executed in the following way:

.. code-block:: console

    $ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
    $ cmake --build build/ --config Release
    $ ./build/test/mqt-ddsim-test
    [...]

----

**Why do I get a linking error at the end of the build process?**

If you are using gcc for building, and you get the error message

.. code-block:: console

    lto1: internal compiler error: Segmentation fault
    Please submit a full bug report,
    with preprocessed source if appropriate.
    See <file:///usr/share/doc/gcc-9/README.Bugs> for instructions.
    lto-wrapper: fatal error: /usr/bin/c++ returned 1 exit status
    compilation terminated.
    /usr/bin/ld: error: lto-wrapper failed
    collect2: error: ld returned 1 exit status
    make[2]: *** [test/CMakeFiles/ddsim_test.dir/build.make:166: test/ddsim_test] Error 1
    make[1]: *** [CMakeFiles/Makefile2:464: test/CMakeFiles/ddsim_test.dir/all] Error 2

Configure the simulator again and add the parameter :code:`-DBUILD_MQT_DDSIM_BINDINGS=ON`
