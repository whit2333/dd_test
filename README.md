DD test
=======


Getting started
---------------

```bash
git clone https://github.com/whit2333/dd_test.git
mkdir dd_test_build && cd project_build
cmake ../dd_test/. -DCMAKE_INSTALL_PREFIX=$HOME
make && make install
```

Installs `recon_test` which should be run from the directory 'src/runtest'.
If the simulation needs to be re-run, use the `run_example` script in the same 
directory.

Send questions to Whitney Armstrong ( whit@jlab.org )

