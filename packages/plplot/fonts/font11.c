/* $Id: font11.c 3186 2006-02-15 18:17:33Z slbrow $
   $Log$
   Revision 1.1  2006/02/15 18:15:44  slbrow
   This is the commit of the DAKOTA 'autoconfiscation' I've been working on, which
   enables builds using the GNU Autotools.  I have been merging in changes to the
   mainline the entire time I've been working on these changes to my working copy
   (in ~slbrow/projects/Dakota), so none of your changes to DAKOTA will be lost.
   In addition, the final pre-autotools version of DAKOTA has been tagged with:

   Version-3-3-Plus-pre-autotools

   The GNU autotools will be needed by all developers who check DAKOTA out from
   CVS with intentions to build; this is because the version of DAKOTA in the
   repository after this commit does not have any configure or Makefile.in files
   (since they are now machine-generated).  All the source tarballs rolled for
   external consumption (both release and votd) will automatically include all the
   configure and Makefile.in files our non-autotools savvy users will need, while
   still omitting those proprietary sources we may not distribute (DOT, NPSOL).

   To re-generate the missing configure and Makefile.in files in a checkout of
   DAKOTA, only one GNU Autotools script is necessary to run from the
   command-line:

   $ autoreconf --install

   This script (and all the scripts it runs in sequence on a copy of DAKOTA
   checked out of the repository) can be found on the SCICO LAN in the directory:

   /usr/netpub/autotools/bin

   This directory exists on all five of the DAKOTA nightly build platforms (AIX,
   IRIX, Linux, Solaris and Tru64), and should be added to the beginning of your
   $PATH if you are on the SCICO LAN.  If you are not, the versions of the tools
   you need can be easily downloaded from the GNU website.  The versions we use:

   autoconf 2.59  (http://ftp.gnu.org/gnu/autoconf/autoconf-2.59.tar.gz)
   automake 1.9.6 (http://ftp.gnu.org/gnu/automake/automake-1.9.6.tar.gz)
   libtool 1.5.22 (http://ftp.gnu.org/gnu/libtool/libtool-1.5.22.tar.gz)
   m4 1.4.3       (http://ftp.gnu.org/gnu/m4/m4-1.4.3.tar.gz)

   Here is a "short" summary of the major changes made in this )commit:

   1. All configure.in and Makefile.in files are no longer in the CVS repository.
      They are replaced by configure.ac and Makefile.am files, respectively.  When
      you run autoreconf, the configure and Makefile.in files are regenerated
      automatically and recursively.  No $DAKOTA environment variable needs to be
      set in order to build DAKOTA.

   2. Build directories as they were in DAKOTA using the Cygnus configure system
      no longer exist.  Executables built by the GNU Autotools (e.g., the dakota*
      binaries which were built in Dakota/src/i686-unknown-linux on Linux) are now
      built in Dakota/src; the autotools provide deployment targets for installing
      binaries and libraries in a deployment directory, as well as a simple means
      of maintaining more than one build of Dakota for multiple platforms (i.e.,
      using VPATH builds and source- and build-tree separation).

      Suppose you have in your home directory a checked-out and 'autoreconf'd copy
      of Dakota, and you want to simultaneously build versions of DAKOTA for Linux
      and Solaris.  You would create two uniquely named directories in your home
      directory, called 'my_linux_build' and 'my_solaris_build', respectively.
      Assuming two terminals are open on both machines, you would 'cd' into the
      appropriate build directory on each platform and run this command on both:

      $ ../Dakota/configure -C

      This will create a directory tree which is identical to the source tree, but
      containing only Makefiles.  Then, when you run 'make', it will traverse the
      build tree, referencing the sources by relative paths to create objects and
      binaries.  At your option, you can then delete these build directories
      without touching your source tree (the 'autoreconf'd checkout of Dakota).  I
      should note here that the GNU Autotools do not _force_ this separation of
      trees; you can do a 'make distclean' after building for each platform in
      your 'autoreconf'd copy of Dakota with no problems.

   3. All the configure flags which were supported in the old build system (i.e.,
      --without-acro, --enable-debugging, etc.), are supported in the new system
      with the following changes and additions:

      --with-mpi/--without-mpi           -> --enable-mpi/--disable-mpi
      --with-graphics/--without-graphics -> --enable-graphics/--disable-graphics
      --enable-docs (default is off)

      I should note that all the default settings have been preserved.  Further,
      since ACRO, OPT++ and DDACE haven't been integrated yet (see item 9), their
      flags are still commented out in 'Dakota/config/dak_vendoroptimizers.m4'.

   4. The epetra and plplot directories under VendorPackages have been completely
      replaced with the versions available online (versions 3.3 and 5.5.2,
      respectively), which were originally flattened to provide support for our
      build system.  Epetra has been altered from the version distributed by the
      Trilinos team to provide full libtool support as that is necessary for
      building mixed-language archives under Solaris, while plplot has had all
      language-bindings except C++ turned off by default.  Additionally, GSL has
      been updated from version 1.7 available online, but preserving the flattened
      structure it had prior to autoconfiscation.

   5. All other packages in the DAKOTA repository under VendorPackages and
      VendorOptimizers have been 'autoconfiscated' separately from DAKOTA.  To
      avoid duplicated configure-time probes, run configure from the top-level
      DAKOTA directory with the '-C' argument; this will cache the probe results
      from each subpackage, removing the need to do anything other than a lookup
      the next time the probe runs.

   6. No 'mpi' symbolic link needs to be made from VendorPackages/mpi to the MPI
      implementation you wish to build against; MPI wrapper scripts for the C++
      compiler are used instead; if you wish to change the default choices for the
      MPI implementation to build against, you can specify where the wrapper
      scripts you wish to use are via environment variables, which may be set
      on separate lines (using setenv in CSH), or with env on the configure line:

      $ env MPICC=mpicc MPICXX=mpiCC MPIF77=mpif77 ./configure -C

   7. All the clean-up make targets ('clean', 'distclean' and a new one for those
      unfamiliar with the GNU Autotools, 'maintainer-clean') are now machine-
      generated.  The targets remove increasing numbers of files as you would
      expect; the last one removes all the autotools-generated files, returning
      you to a pristine CVS checkout.

   8. Many changes have been made to the sources in Dakota/src:

      A) Fortran calls are now done using FC_FUNC and F77_FUNC macro wrappers,
         removing the need for #ifdef RS6K preprocessor checks for underscores.
      B) Platform testing preprocessor conditionals have been replaced* with the
         feature that was assumed to be supported on that platform 'a priori'
         * mostly: MPI implementation choice is still done by platform.
           See ParallelLibrary.C for details.
      C) TFLOPS and COUGAR #ifdef's have been removed, leaving their #else
         branches to execute unconditionally (as those machines have gone away).

   9. Since the GNU Autotools preferentially look for GCC compilers first; if you
      wish to build on Solaris with the Sun compilers you must set the compilers
      to use in environment variables in the same way the MPI wrappers must be set
      (see item 5 above):

       $ env CC=cc CXX=CC F77=f77 FC=f90 ./configure ...

   Additionally, there are some minor problems to fix with the new build system:

   10. This autoconfiscation covers Dakota, as integration with ACRO, OPT++ and
       DDACE has yet to be done.  I have integrated Surfpack and removed the
       separate ann and kriging packages from VendorPackages, based on positive
       results from some integration testing I've done on the Surfpack versions of
       ann and kriging against those in the separate versions in VendorPackages.

   11. Purely static-linked executables have proved difficult with libtool.  The
       way libtool sets things up, all the DAKOTA package libraries are static and
       linked as such, but system libraries are linked against dynamically (the
       way it is currently done on Solaris).  Strangely though, the MPI libraries
       aren't linked against dynamically, so this is an area for investigation
       (especially as I planned on having it working for the ASC White/Purple
       deployment of Dakota 4.0).

   12. Running 'make -j <n>' with positive integer n for parallel builds is not
       yet supported, as there are a couple packages with dependency requirements
       (LHS and IDR, namely) which preclude compiling in parallel until their
       requirements are met.  In LHS, those requirements arise from the need for
       all the *.mod module files to exist prior to anything which USE's them, and
       in IDR, they arise from the lex/yacc generated source dependencies.

   13. Disabling F90 support (via the --disable-f90 configure flag) doesn't work
       yet.  GNU Automake isn't quite smart enough yet to accept arbitrary
       suffixes for F90 files, so it expects that the suffixes will be .f90 or
       .f95, _not_ .F!  The Automake developers are working on this, so in the
       meantime, I'm using a hack.  I put that hack into Dakota/src/Makefile.am to
       address this situation (the same one I use in LHS, by the way, which sets
       F77 and FFLAGS to their F90-equivalents) doesn't work when F90 is disabled.

   14. The nightly build and test system (in Dakota/test/sqa) will need to be
       modified to use the new system, so the nightlies will be broken until I can
       get the scripts modified to use the new system.  That will be my next CVS
       checkin, and I don't expect it to take very long to make those mods.

   Revision 1.1  1992/05/20 21:33:32  furnish
   Initial checkin of the whole PLPLOT project.

*/

      short int subbuffer250[100] = {
       8652, 8907, 9034, 9160, 9157, 9027, 8898, 8641,   64, 7095,
       7991,    0,  -16, 7116, 6858, 9033, 9158, 9164, 9033, 8779,
       8396, 8140, 7755, 7497, 7367, 7236, 7231, 7356, 7482, 7736,
       8119, 8375, 8760, 9018, 9148,   64, 8140, 7883, 7625, 7495,
       7364, 7359, 7484, 7610, 7864, 8119,    0,  -16, 7116, 7114,
       8268, 8247,   64, 8396, 8375,   64, 7500, 7366, 7372, 9292,
       9286, 9164,   64, 7863, 8759,    0,  -16, 7116, 6987, 7372,
       8252,   64, 7500, 8380,   64, 9292, 8380, 8121, 7992, 7735,
       7607, 7480, 7481, 7610, 7737, 7608,   64, 7116, 7884,   64,
       8780, 9548,    0,  -16, 7116, 6733, 8268, 8247,   64, 8396
      };
      short int subbuffer251[100] = {
       8375,   64, 7884, 8780,   64, 8009, 7496, 7238, 7107, 7104,
       7229, 7483, 7994, 8634, 9147, 9405, 9536, 9539, 9414, 9160,
       8649, 8009,   64, 8009, 7624, 7366, 7235, 7232, 7357, 7611,
       7994,   64, 8634, 9019, 9277, 9408, 9411, 9286, 9032, 8649,
         64, 7863, 8759,    0,  -16, 7116, 6986, 7372, 9015,   64,
       7500, 9143,   64, 9164, 7351,   64, 7116, 7884,   64, 8652,
       9420,   64, 7095, 7863,   64, 8631, 9399,    0,  -16, 7116,
       6732, 7372, 7351,   64, 7500, 7479,   64, 9036, 9015,   64,
       9164, 9143,   64, 6988, 7884,   64, 8652, 9548,   64, 6967,
       9527,   64, 9399, 9520,   64, 9527, 9520,    0,  -16, 7116
      };
      short int subbuffer252[100] = {
       6731, 7372, 7361, 7487, 7870, 8254, 8639, 8897,   64, 7500,
       7489, 7615, 7870,   64, 8908, 8887,   64, 9036, 9015,   64,
       6988, 7884,   64, 8524, 9420,   64, 8503, 9399,    0,  -16,
       7116, 6225, 6860, 6839,   64, 6988, 6967,   64, 8268, 8247,
         64, 8396, 8375,   64, 9676, 9655,   64, 9804, 9783,   64,
       6476, 7372,   64, 7884, 8780,   64, 9292,10188,   64, 6455,
      10167,    0,  -16, 7116, 6225, 6860, 6839,   64, 6988, 6967,
         64, 8268, 8247,   64, 8396, 8375,   64, 9676, 9655,   64,
       9804, 9783,   64, 6476, 7372,   64, 7884, 8780,   64, 9292,
      10188,   64, 6455,10167,   64,10039,10160,   64,10167,10160
      };
      short int subbuffer253[100] = {
          0,  -16, 7116, 6734, 8012, 7991,   64, 8140, 8119,   64,
       7116, 6982, 6988, 8524,   64, 8130, 9026, 9409, 9536, 9662,
       9659, 9529, 9400, 9015, 7607,   64, 9026, 9281, 9408, 9534,
       9531, 9401, 9272, 9015,    0,  -16, 7116, 6351, 6988, 6967,
         64, 7116, 7095,   64, 6604, 7500,   64, 7106, 8002, 8385,
       8512, 8638, 8635, 8505, 8376, 7991, 6583,   64, 8002, 8257,
       8384, 8510, 8507, 8377, 8248, 7991,   64, 9420, 9399,   64,
       9548, 9527,   64, 9036, 9932,   64, 9015, 9911,    0,  -16,
       7116, 6987, 7628, 7607,   64, 7756, 7735,   64, 7244, 8140,
         64, 7746, 8642, 9025, 9152, 9278, 9275, 9145, 9016, 8631
      };
      short int subbuffer254[100] = {
       7223,   64, 8642, 8897, 9024, 9150, 9147, 9017, 8888, 8631,
          0,  -16, 7116, 6987, 7497, 7372, 7366, 7497, 7755, 8140,
       8396, 8779, 9033, 9159, 9284, 9279, 9148, 9018, 8760, 8375,
       7991, 7608, 7481, 7355, 7356, 7485, 7612, 7483,   64, 8396,
       8651, 8905, 9031, 9156, 9151, 9020, 8890, 8632, 8375,   64,
       8002, 9154,    0,  -16, 7116, 6352, 6988, 6967,   64, 7116,
       7095,   64, 6604, 7500,   64, 6583, 7479,   64, 8780, 8395,
       8137, 8007, 7875, 7872, 7996, 8122, 8376, 8759, 9015, 9400,
       9658, 9788, 9920, 9923, 9799, 9673, 9419, 9036, 8780,   64,
       8780, 8523, 8265, 8135, 8003, 8000, 8124, 8250, 8504, 8759
      };
      short int subbuffer255[100] = {
         64, 9015, 9272, 9530, 9660, 9792, 9795, 9671, 9545, 9291,
       9036,   64, 7106, 7874,    0,  -16, 7116, 6859, 8908, 8887,
         64, 9036, 9015,   64, 9420, 7884, 7499, 7370, 7240, 7238,
       7364, 7491, 7874, 8898,   64, 7884, 7627, 7498, 7368, 7366,
       7492, 7619, 7874,   64, 8258, 8001, 7872, 7481, 7352, 7224,
       7097,   64, 8001, 7871, 7608, 7479, 7223, 7097, 7098,   64,
       8503, 9399,    0,  -16, 7116, 7115, 7747, 7746, 7618, 7619,
       7748, 8005, 8517, 8772, 8899, 9025, 9018, 9144, 9271,   64,
       8899, 8890, 9016, 9271, 9399,   64, 8897, 8768, 7999, 7614,
       7484, 7482, 7608, 7991, 8375, 8632, 8890,   64, 7999, 7742
      };
      short int subbuffer256[100] = {
       7612, 7610, 7736, 7991,    0,  -16, 7116, 6986, 9036, 8907,
       8137, 7751, 7492, 7361, 7357, 7482, 7736, 8119, 8375, 8760,
       9018, 9149, 9151, 9026, 8772, 8389, 8133, 7748, 7490, 7359,
         64, 9036, 8906, 8649, 8136, 7750, 7492,   64, 8133, 7876,
       7618, 7487, 7485, 7610, 7864, 8119,   64, 8375, 8632, 8890,
       9021, 9023, 8898, 8644, 8389,    0,  -16, 7116, 6986, 7621,
       7607,   64, 7749, 7735,   64, 7237, 8645, 9028, 9154, 9153,
       9023, 8638,   64, 8645, 8900, 9026, 9025, 8895, 8638,   64,
       7742, 8638, 9021, 9147, 9146, 9016, 8631, 7223,   64, 8638,
       8893, 9019, 9018, 8888, 8631,    0,  -16, 7116, 6984, 7621
      };
      short int subbuffer257[100] = {
       7607,   64, 7749, 7735,   64, 7237, 9029, 9024, 8901,   64,
       7223, 8119,    0,  -16, 7116, 6731, 7749, 7745, 7611, 7480,
       7351,   64, 8901, 8887,   64, 9029, 9015,   64, 7365, 9413,
         64, 7095, 6962, 6967, 9399, 9394, 9271,    0,  -16, 7116,
       6985, 7487, 9023, 9025, 8899, 8772, 8517, 8133, 7748, 7490,
       7359, 7357, 7482, 7736, 8119, 8375, 8760, 9018,   64, 8895,
       8898, 8772,   64, 8133, 7876, 7618, 7487, 7485, 7610, 7864,
       8119,    0,  -16, 7116, 6606, 8261, 8247,   64, 8389, 8375,
         64, 7877, 8773,   64, 7236, 7107, 6980, 7109, 7237, 7364,
       7616, 7743, 7998, 8638, 8895, 9024, 9284, 9413, 9541, 9668
      };
      short int subbuffer258[100] = {
       9539, 9412,   64, 7998, 7741, 7612, 7352, 7223,   64, 7998,
       7740, 7480, 7351, 7095, 6968, 6842,   64, 8638, 8893, 9020,
       9272, 9399,   64, 8638, 8892, 9144, 9271, 9527, 9656, 9786,
         64, 7863, 8759,    0,  -16, 7116, 7113, 7619, 7493, 7489,
       7619, 7748, 8005, 8517, 8900, 9026, 9025, 8895, 8510,   64,
       8517, 8772, 8898, 8897, 8767, 8510,   64, 8126, 8510, 8893,
       9019, 9018, 8888, 8503, 7991, 7608, 7482, 7483, 7612, 7739,
       7610,   64, 8510, 8765, 8891, 8890, 8760, 8503,    0,  -16,
       7116, 6859, 7493, 7479,   64, 7621, 7607,   64, 8901, 8887,
         64, 9029, 9015,   64, 7109, 8005,   64, 8517, 9413,   64
      };
      short int subbuffer259[100] = {
       7095, 7991,   64, 8503, 9399,   64, 8900, 7608,    0,  -16,
       7116, 6859, 7493, 7479,   64, 7621, 7607,   64, 8901, 8887,
         64, 9029, 9015,   64, 7109, 8005,   64, 8517, 9413,   64,
       7095, 7991,   64, 8503, 9399,   64, 8900, 7608,   64, 7883,
       7884, 7756, 7755, 7881, 8136, 8392, 8649, 8779,    0,  -16,
       7116, 6986, 7621, 7607,   64, 7749, 7735,   64, 7237, 8133,
         64, 7742, 7998, 8383, 8512, 8772, 8901, 9029, 9156, 9027,
       8900,   64, 7998, 8381, 8508, 8760, 8887,   64, 7998, 8253,
       8380, 8632, 8759, 9015, 9144, 9274,   64, 7223, 8119,    0,
        -16, 7116, 6859, 7749, 7745, 7611, 7480, 7351, 7223, 7096
      };
      short int subbuffer260[100] = {
       7225, 7352,   64, 8901, 8887,   64, 9029, 9015,   64, 7365,
       9413,   64, 8503, 9399,    0,  -16, 7116, 6860, 7493, 7479,
         64, 7493, 8247,   64, 7621, 8249,   64, 9029, 8247,   64,
       9029, 9015,   64, 9157, 9143,   64, 7109, 7621,   64, 9029,
       9541,   64, 7095, 7863,   64, 8631, 9527,    0,  -16, 7116,
       6859, 7493, 7479,   64, 7621, 7607,   64, 8901, 8887,   64,
       9029, 9015,   64, 7109, 8005,   64, 8517, 9413,   64, 7614,
       8894,   64, 7095, 7991,   64, 8503, 9399,    0,  -16, 7116,
       6986, 8133, 7748, 7490, 7359, 7357, 7482, 7736, 8119, 8375,
       8760, 9018, 9149, 9151, 9026, 8772, 8389, 8133,   64, 8133
      };
      short int subbuffer261[100] = {
       7876, 7618, 7487, 7485, 7610, 7864, 8119,   64, 8375, 8632,
       8890, 9021, 9023, 8898, 8644, 8389,    0,  -16, 7116, 6859,
       7493, 7479,   64, 7621, 7607,   64, 8901, 8887,   64, 9029,
       9015,   64, 7109, 9413,   64, 7095, 7991,   64, 8503, 9399,
          0,  -16, 7116, 6858, 7493, 7472,   64, 7621, 7600,   64,
       7618, 7876, 8133, 8389, 8772, 9026, 9151, 9149, 9018, 8760,
       8375, 8119, 7864, 7610,   64, 8389, 8644, 8898, 9023, 9021,
       8890, 8632, 8375,   64, 7109, 7621,   64, 7088, 7984,    0,
        -16, 7116, 6985, 8898, 8769, 8896, 9025, 9026, 8772, 8517,
       8133, 7748, 7490, 7359, 7357, 7482, 7736, 8119, 8375, 8760
      };
      short int subbuffer262[100] = {
       9018,   64, 8133, 7876, 7618, 7487, 7485, 7610, 7864, 8119,
          0,  -16, 7116, 7114, 8261, 8247,   64, 8389, 8375,   64,
       7621, 7488, 7493, 9157, 9152, 9029,   64, 7863, 8759,    0,
        -16, 7116, 7113, 7493, 8247,   64, 7621, 8249,   64, 9029,
       8247, 7987, 7729, 7472, 7344, 7217, 7346, 7473,   64, 7237,
       8005,   64, 8517, 9285,    0,  -16, 7116, 6987, 8268, 8240,
         64, 8396, 8368,   64, 7884, 8396,   64, 8258, 8132, 8005,
       7749, 7492, 7361, 7355, 7480, 7735, 7991, 8120, 8250,   64,
       7749, 7620, 7489, 7483, 7608, 7735,   64, 8901, 9028, 9153,
       9147, 9016, 8887,   64, 8386, 8516, 8645, 8901, 9156, 9281
      };
      short int subbuffer263[100] = {
       9275, 9144, 8887, 8631, 8504, 8378,   64, 7856, 8752,    0,
        -16, 7116, 6986, 7493, 8887,   64, 7621, 9015,   64, 9029,
       7479,   64, 7237, 8005,   64, 8517, 9285,   64, 7223, 7991,
         64, 8503, 9271,    0,  -16, 7116, 6859, 7493, 7479,   64,
       7621, 7607,   64, 8901, 8887,   64, 9029, 9015,   64, 7109,
       8005,   64, 8517, 9413,   64, 7095, 9399, 9394, 9271,    0,
        -16, 7116, 6859, 7493, 7486, 7612, 7995, 8251, 8636, 8894,
         64, 7621, 7614, 7740, 7995,   64, 8901, 8887,   64, 9029,
       9015,   64, 7109, 8005,   64, 8517, 9413,   64, 8503, 9399,
          0,  -16, 7116, 6352, 6981, 6967,   64, 7109, 7095,   64
      };
      short int subbuffer264[100] = {
       8261, 8247,   64, 8389, 8375,   64, 9541, 9527,   64, 9669,
       9655,   64, 6597, 7493,   64, 7877, 8773,   64, 9157,10053,
         64, 6583,10039,    0,  -16, 7116, 6352, 6981, 6967,   64,
       7109, 7095,   64, 8261, 8247,   64, 8389, 8375,   64, 9541,
       9527,   64, 9669, 9655,   64, 6597, 7493,   64, 7877, 8773,
         64, 9157,10053,   64, 6583,10039,10034, 9911,    0,  -16,
       7116, 6987, 8133, 8119,   64, 8261, 8247,   64, 7493, 7360,
       7365, 8645,   64, 8254, 8766, 9149, 9275, 9274, 9144, 8759,
       7735,   64, 8766, 9021, 9147, 9146, 9016, 8759,    0,  -16,
       7116, 6605, 7237, 7223,   64, 7365, 7351,   64, 6853, 7749
      };
      short int subbuffer265[100] = {
         64, 7358, 7870, 8253, 8379, 8378, 8248, 7863, 6839,   64,
       7870, 8125, 8251, 8250, 8120, 7863,   64, 9157, 9143,   64,
       9285, 9271,   64, 8773, 9669,   64, 8759, 9655,    0,  -16,
       7116, 7241, 7877, 7863,   64, 8005, 7991,   64, 7493, 8389,
         64, 7998, 8510, 8893, 9019, 9018, 8888, 8503, 7479,   64,
       8510, 8765, 8891, 8890, 8760, 8503,    0,  -16, 7116, 7114,
       7619, 7493, 7489, 7619, 7748, 8005, 8389, 8772, 9026, 9151,
       9149, 9018, 8760, 8375, 7991, 7736, 7482, 7483, 7612, 7739,
       7610,   64, 8389, 8644, 8898, 9023, 9021, 8890, 8632, 8375,
         64, 8254, 9022,    0,  -16, 7116, 6479, 7109, 7095,   64
      };
      short int subbuffer266[100] = {
       7237, 7223,   64, 6725, 7621,   64, 6711, 7607,   64, 8773,
       8388, 8130, 7999, 7997, 8122, 8376, 8759, 9015, 9400, 9658,
       9789, 9791, 9666, 9412, 9029, 8773,   64, 8773, 8516, 8258,
       8127, 8125, 8250, 8504, 8759,   64, 9015, 9272, 9530, 9661,
       9663, 9538, 9284, 9029,   64, 7230, 7998,    0,  -16, 7116,
       6858, 8773, 8759,   64, 8901, 8887,   64, 9285, 7877, 7492,
       7362, 7361, 7487, 7870, 8766,   64, 7877, 7620, 7490, 7489,
       7615, 7870,   64, 8510, 8125, 7996, 7736, 7607,   64, 8510,
       8253, 8124, 7864, 7735, 7479, 7352, 7226,   64, 8375, 9271,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0
      };
      short int subbuffer267[100] = {
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0
      };
      short int subbuffer268[100] = {
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0
      };
      short int subbuffer269[100] = {
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
          0,    0,    0,    0,    0,    0,    0,    0,    0,    0
      };

      short int subhersh0[176] = {
        841,  899,  845,  847,  840,  846,  841,  842, 2284, 2281,
        735,  843,  844,  841,  866,  868,  851,  850,  856,  841,
        900,  901,  902,  903,  904,  905,  906,  907, 2263, 2261,
       2262, 2264,  699,  714,  717,  733,  719, 2271,  734,  716,
        721,  722,  728,  725,  711,  724,  710,  720,  700,  701,
        702,  703,  704,  705,  706,  707,  708,  709,  712,  713,
       2241,  726, 2242,  715, 2273,  501,  502,  503,  504,  505,
        506,  507,  508,  509,  510,  511,  512,  513,  514,  515,
        516,  517,  518,  519,  520,  521,  522,  523,  524,  525,
        526, 2223,  729, 2224,  718,  729, 2249,  601,  602,  603,
        604,  605,  606,  607,  608,  609,  610,  611,  612,  613,
        614,  615,  616,  617,  618,  619,  620,  621,  622,  623,
        624,  625,  626, 2225,  723, 2226, 2246,  699,  527,  528,
        529,  530,  531,  532,  533,  534,  535,  536,  537,  538,
        539,  540,  541,  542,  543,  544,  545,  546,  547,  548,
        549,  550,  627,  628,  629,  630,  684,  632,  633,  685,
        635,  636,  637,  638,  639,  640,  641,  642,  643,  644,
        645,  646,  686,  648,  649,  650
      };
      short int subhersh1[176] = {
        841,  899,  845,  847,  840,  846,  841,  842, 2284, 2281,
        735,  843,  844,  841,  866,  868,  851,  850,  856,  841,
        900,  901,  902,  903,  904,  905,  906,  907, 2263, 2261,
       2262, 2264, 2199, 2214, 2217, 2275, 2274, 2271, 2272, 2216,
       2221, 2222, 2219, 2232, 2211, 2231, 2210, 2220, 2200, 2201,
       2202, 2203, 2204, 2205, 2206, 2207, 2208, 2209, 2212, 2213,
       2241, 2238, 2242, 2215, 2273, 2001, 2002, 2003, 2004, 2005,
       2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015,
       2016, 2017, 2018, 2019, 2020, 2021, 2022, 2023, 2024, 2025,
       2026, 2223, 2236, 2224, 2218, 2236, 2249, 2101, 2102, 2103,
       2104, 2105, 2106, 2107, 2108, 2109, 2110, 2111, 2112, 2113,
       2114, 2115, 2116, 2117, 2118, 2119, 2120, 2121, 2122, 2123,
       2124, 2125, 2126, 2225, 2229, 2226, 2246, 2199, 2027, 2028,
       2029, 2030, 2031, 2032, 2033, 2034, 2035, 2036, 2037, 2038,
       2039, 2040, 2041, 2042, 2043, 2044, 2045, 2046, 2047, 2048,
       2049, 2050, 2127, 2128, 2129, 2130, 2184, 2132, 2133, 2185,
       2135, 2136, 2137, 2138, 2139, 2140, 2141, 2142, 2143, 2144,
       2145, 2146, 2186, 2148, 2149, 2150
      };
      short int subhersh2[176] = {
        841,  899,  845,  847,  840,  846,  841,  842, 2284, 2281,
        735,  843,  844,  841,  866,  868,  851,  850,  856,  841,
        900,  901,  902,  903,  904,  905,  906,  907, 2263, 2261,
       2262, 2264, 2199, 2764, 2778, 2275, 2769, 2271, 2768, 2777,
       2771, 2772, 2773, 2775, 2761, 2774, 2760, 2770, 2750, 2751,
       2752, 2753, 2754, 2755, 2756, 2757, 2758, 2759, 2762, 2763,
       2241, 2776, 2242, 2765, 2273, 2051, 2052, 2053, 2054, 2055,
       2056, 2057, 2058, 2059, 2060, 2061, 2062, 2063, 2064, 2065,
       2066, 2067, 2068, 2069, 2070, 2071, 2072, 2073, 2074, 2075,
       2076, 2223, 2236, 2224, 2779, 2236, 2249, 2151, 2152, 2153,
       2154, 2155, 2156, 2157, 2158, 2159, 2160, 2161, 2162, 2163,
       2164, 2165, 2166, 2167, 2168, 2169, 2170, 2171, 2172, 2173,
       2174, 2175, 2176, 2225, 2229, 2226, 2246, 2199, 2027, 2028,
       2029, 2030, 2031, 2032, 2033, 2034, 2035, 2036, 2037, 2038,
       2039, 2040, 2041, 2042, 2043, 2044, 2045, 2046, 2047, 2048,
       2049, 2050, 2127, 2128, 2129, 2130, 2184, 2132, 2133, 2185,
       2135, 2136, 2137, 2138, 2139, 2140, 2141, 2142, 2143, 2144,
       2145, 2146, 2186, 2148, 2149, 2150
      };
      short int subhersh3[176] = {
        841,  899,  845,  847,  840,  846,  841,  842, 2284, 2281,
        735,  843,  844,  841,  866,  868,  851,  850,  856,  841,
        900,  901,  902,  903,  904,  905,  906,  907, 2263, 2261,
       2262, 2264, 2199, 2764, 2778, 2275, 2769, 2271, 2768, 2777,
       2771, 2772, 2773, 2775, 2761, 2774, 2760, 2770, 2750, 2751,
       2752, 2753, 2754, 2755, 2756, 2757, 2758, 2759, 2762, 2763,
       2241, 2776, 2242, 2765, 2273, 2551, 2552, 2553, 2554, 2555,
       2556, 2557, 2558, 2559, 2560, 2561, 2562, 2563, 2564, 2565,
       2566, 2567, 2568, 2569, 2570, 2571, 2572, 2573, 2574, 2575,
       2576, 2223, 2236, 2224, 2779, 2236, 2249, 2651, 2652, 2653,
       2654, 2655, 2656, 2657, 2658, 2659, 2660, 2661, 2662, 2663,
       2664, 2665, 2666, 2667, 2668, 2669, 2670, 2671, 2672, 2673,
       2674, 2675, 2676, 2225, 2229, 2226, 2246, 2199, 2027, 2028,
       2029, 2030, 2031, 2032, 2033, 2034, 2035, 2036, 2037, 2038,
       2039, 2040, 2041, 2042, 2043, 2044, 2045, 2046, 2047, 2048,
       2049, 2050, 2127, 2128, 2129, 2130, 2184, 2132, 2133, 2185,
       2135, 2136, 2137, 2138, 2139, 2140, 2141, 2142, 2143, 2144,
       2145, 2146, 2186, 2148, 2149, 2150
      };
