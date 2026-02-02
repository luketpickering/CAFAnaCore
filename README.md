### CAFAnaCore -- Base libraries of the CAFAna analysis framework

----------------------------------------------------------------------

  *Package author*:               C. Backhouse <c.backhouse@ucl.ac.uk>

  *Build system*:                 J. Wolcott <jwolcott@fnal.gov>

----------------------------------------------------------------------

This document briefly describes the build process of the CAFAna framework
contained in this package.

For documentation on the classes themselves, automatically generated Doxygen
pages are maintained on our
[GitHub Pages site](https://cafana.github.io/CAFAnaCore/).  (These are
automatically updated when pull requests are merged to the `main` branch.)

## Building Standalone

### Dependencies

#### Required

* `cetmodules`: FNAL CMake toolkit
  + Already fetched: `export cetmodules_ROOT=/path/to/install/prefix`
  + To fetch
```bash
git clone --depth 1 --branch 3.27.03 https://github.com/FNALssi/cetmodules.git
cd cetmodules; mkdir build; cd build;
cmake .. -DCMAKE_INSTALL_PREFIX=$(readlink -f $(uname)); make install;
export cetmodules_ROOT=$(readlink -f $(uname))
```

* ROOT
* Eigen3

#### Optional

* [Stan](https://mc-stan.org)
  + Boost
* IFDH

### Build

```bash
git clone git@github.com:cafana/CAFAnaCore.git
cd CAFAnaCore; mkdir build; cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$(readlink -f $(uname)); make install;
export CAFAnaCore_ROOT=$(readlink -f $(uname))
```

* With Stan:

```bash
cmake .. -DCAFAnaCore_USE_STAN=ON \
         -CAFAnaCore_USE_STAN_THREADS=<ON|OFF> \
         -DCMAKE_INSTALL_PREFIX=$(readlink -f $(uname))
make install;
```

* With IFDH:

```bash
cmake .. -DCAFAnaCore_USE_Ifdhc=ON \
         -DCMAKE_INSTALL_PREFIX=$(readlink -f $(uname))
make install;
```

## Building @ FNAL

### Interactively via ups

- `export QUALIFIER=e26:prof` or `e26:debug`, etc
- `export STAN=stan` or `stanfree` or `stanthread`
- `jenkins/jenkins_build.sh` (or paste parts of it into your terminal)
- set `CAFANACORE_LIB` and `CAFANACORE_INC` manually to point to what you just built
- rebuild your test release
- `export CAFANA_DISABLE_VERSION_CHECK=1`
- Profit!

### NOvA jenkins

#### Build

- Make changes

```
git push
git tag $NEW_TAG_NUMBER
git push --tags
```

- Navigate to https://buildmaster.fnal.gov/buildmaster/view/Nova/job/external/job/cafanacore_build/ and click "Build Now".
- Wait

#### Deploy

```bash
wget https://buildmaster.fnal.gov/buildmaster/view/Nova/job/external/job/cafanacore_collect/lastSuccessfulBuild/artifact/*zip*/archive.zip
unzip archive.zip
mv archive/* .
rm archive.zip

ssh cvmfs${EXPERIMENT}@oasiscfs.fnal.gov
cvmfs_server transaction ${EXPERIMENT}.opensciencegrid.org
# fetch the files you extracted previously to the correct /cvmfs directory
cd /cvmfs/ ... /cafanacore/$NEW_TAG_NUMBER/include
ln -s . CAFAnaCore # can't be done earlier because neither jenkins or scp like symlinks
ln -s CAFAna/Core/ cafanacore
cd -
cvmfs_server publish ${EXPERIMENT}.opensciencegrid.org
```

### Post tag procedure

* __NOvA__:
  + Update `CAFAna/Core/VersionCheck.cxx`, `setup/nova-offline-ups-externals-development`, and `nova-offline-ups-externals-development-prof`
  + Notify `#cmake`
* __DUNE__:
  + Update `cmake/ups_env_setup.sh`

## Usage

Coming soon!
