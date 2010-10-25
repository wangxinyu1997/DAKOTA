/* $Id: font07.c 3186 2006-02-15 18:17:33Z slbrow $
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

   Revision 1.1  1992/05/20 21:33:27  furnish
   Initial checkin of the whole PLPLOT project.

*/

      short int subbuffer149[100] = {
       8260,   64, 8249, 8120, 8247, 8376, 8249,    0,  -16, 7116,
       7113, 7624, 7751, 7622, 7495, 7496, 7626, 7755, 8012, 8396,
       8779, 8906, 9032, 9030, 8900, 8771, 8257, 8254,   64, 8396,
       8651, 8778, 8904, 8902, 8772, 8514,   64, 8249, 8120, 8247,
       8376, 8249,    0,  -16, 7116, 7748, 8268, 8133,   64, 8396,
       8133,    0,  -16, 7116, 7240, 7756, 7621,   64, 7884, 7621,
         64, 8780, 8645,   64, 8908, 8645,    0,  -16, 7116, 7367,
       8140, 7883, 7753, 7751, 7877, 8132, 8388, 8645, 8775, 8777,
       8651, 8396, 8140,    0,  -16, 7116, 7240, 8268, 8256,   64,
       7625, 8899,   64, 8905, 7619,    0,  -16, 7116, 6859, 9424
      };
      short int subbuffer150[100] = {
       7088,    0,  -16, 7116, 7367, 8784, 8526, 8267, 8007, 7874,
       7870, 7993, 8245, 8498, 8752,   64, 8526, 8266, 8135, 8002,
       7998, 8121, 8246, 8498,    0,  -16, 7116, 7367, 7760, 8014,
       8267, 8519, 8642, 8638, 8505, 8245, 7986, 7728,   64, 8014,
       8266, 8391, 8514, 8510, 8377, 8246, 7986,    0,  -16, 7116,
       7367, 7888, 7856,   64, 8016, 7984,   64, 7888, 8784,   64,
       7856, 8752,    0,  -16, 7116, 7367, 8528, 8496,   64, 8656,
       8624,   64, 7760, 8656,   64, 7728, 8624,    0,  -16, 7116,
       7239, 8528, 8141, 8010, 8008, 8133, 8387, 8386, 7872, 8382,
       8381, 8123, 7992, 7990, 8115, 8496,   64, 8270, 8139, 8135
      };
      short int subbuffer151[100] = {
       8260,   64, 8252, 8121, 8117, 8242,    0,  -16, 7116, 7368,
       8016, 8397, 8522, 8520, 8389, 8131, 8130, 8640, 8126, 8125,
       8379, 8504, 8502, 8371, 7984,   64, 8270, 8395, 8391, 8260,
         64, 8252, 8377, 8373, 8242,    0,  -16, 7116, 7367, 8656,
       7744, 8624,    0,  -16, 7116, 7367, 7888, 8768, 7856,    0,
        -16, 7116, 7748, 8272, 8240,    0,  -16, 7116, 7367, 7888,
       7856,   64, 8656, 8624,    0,  -16, 7116, 6605, 7104, 9408,
          0,  -16, 7116, 6605, 8265, 8247,   64, 7104, 9408,    0,
        -16, 7116, 6732, 8264, 8247,   64, 7232, 9280,   64, 7223,
       9271,    0,  -16, 7116, 6732, 8264, 8247,   64, 7240, 9288
      };
      short int subbuffer152[100] = {
         64, 7232, 9280,    0,  -16, 7116, 6859, 7367, 9145,   64,
       9159, 7353,    0,  -16, 7116, 7621, 8257, 8128, 8255, 8384,
       8257,    0,  -16, 7116, 6605, 8265, 8136, 8263, 8392, 8265,
         64, 7104, 9408,   64, 8249, 8120, 8247, 8376, 8249,    0,
        -16, 7116, 6605, 7107, 9411,   64, 7101, 9405,    0,  -16,
       7116, 6605, 9161, 7351,   64, 7107, 9411,   64, 7101, 9405,
          0,  -16, 7116, 6605, 7109, 9413,   64, 7104, 9408,   64,
       7099, 9403,    0,  -16, 7116, 6732, 9289, 7232, 9271,    0,
        -16, 7116, 6732, 7241, 9280, 7223,    0,  -16, 7116, 6732,
       9292, 7237, 9278,   64, 7228, 9276,   64, 7223, 9271,    0
      };
      short int subbuffer153[100] = {
        -16, 7116, 6732, 7244, 9285, 7230,   64, 7228, 9276,   64,
       7223, 9271,    0,  -16, 7116, 6733, 9403, 9147, 8892, 8638,
       8258, 8131, 7876, 7620, 7363, 7233, 7231, 7357, 7612, 7868,
       8125, 8254, 8642, 8900, 9157, 9413,    0,  -16, 7116, 6732,
       7101, 7103, 7234, 7491, 7747, 8002, 8511, 8766, 9022, 9279,
       9409,   64, 7103, 7233, 7490, 7746, 8001, 8510, 8765, 9021,
       9278, 9409, 9411,    0,  -16, 7116, 6859, 7230, 8259, 9278,
         64, 7230, 8258, 9278,    0,  -16, 7116, 7494, 8524, 7878,
         64, 8524, 8651, 7878,    0,  -16, 7116, 7494, 8012, 8646,
         64, 8012, 7883, 8646,    0,  -16, 7116, 6986, 7372, 7498
      };
      short int subbuffer154[100] = {
       7752, 8135, 8391, 8776, 9034, 9164,   64, 7372, 7497, 7751,
       8134, 8390, 8775, 9033, 9164,    0,  -16, 7116, 7621, 8266,
       8139, 8268, 8395, 8393, 8263, 8134,    0,  -16, 7116, 7621,
       8396, 8267, 8137, 8135, 8262, 8391, 8264,    0,  -16, 7116,
       7621, 8266, 8395, 8268, 8139, 8137, 8263, 8390,    0,  -16,
       7116, 7621, 8140, 8267, 8393, 8391, 8262, 8135, 8264,    0,
        -16, 7116, 6601, 6981, 7493, 8249,   64, 7365, 8247,   64,
       9424, 8247,    0,  -16, 7116, 6732, 9288, 8392, 7879, 7622,
       7364, 7233, 7231, 7356, 7610, 7865, 8376, 9272,    0,  -16,
       7116, 6732, 7240, 7233, 7357, 7483, 7737, 8120, 8376, 8761
      };
      short int subbuffer155[100] = {
       9019, 9149, 9281, 9288,    0,  -16, 7116, 6732, 7240, 8136,
       8647, 8902, 9156, 9281, 9279, 9148, 8890, 8633, 8120, 7224,
          0,  -16, 7116, 6732, 7224, 7231, 7363, 7493, 7751, 8136,
       8392, 8775, 9029, 9155, 9279, 9272,    0,  -16, 7116, 6732,
       9288, 8392, 7879, 7622, 7364, 7233, 7231, 7356, 7610, 7865,
       8376, 9272,   64, 7232, 8768,    0,  -16, 7116, 6605, 9026,
       9408, 9022,   64, 8645, 9280, 8635,   64, 7104, 9280,    0,
        -16, 7116, 7240, 8006, 8265, 8518,   64, 7619, 8264, 8899,
         64, 8264, 8247,    0,  -16, 7116, 6605, 7490, 7104, 7486,
         64, 7877, 7232, 7867,   64, 7232, 9408,    0,  -16, 7116
      };
      short int subbuffer156[100] = {
       7240, 7994, 8247, 8506,   64, 7613, 8248, 8893,   64, 8265,
       8248,    0,  -16, 7116, 7114, 9024, 8899, 8772, 8517, 8261,
       7876, 7617, 7486, 7483, 7609, 7736, 7991, 8247, 8632, 8890,
       9021, 9154, 9159, 9034, 8907, 8652, 8268, 8011, 7882, 7881,
       8009, 8010,   64, 8261, 8004, 7745, 7614, 7610, 7736,   64,
       8247, 8504, 8762, 8893, 9026, 9031, 8906, 8652,    0,  -16,
       7116, 6986, 7244, 8247,   64, 7372, 8249,   64, 9292, 8247,
         64, 7244, 9292,   64, 7371, 9163,    0,  -16, 7116, 6096,
       6469, 7109, 8249,   64, 6980, 8247,   64,10328, 8247,    0,
        -16, 7116, 6732, 9423, 9294, 9421, 9550, 9551, 9424, 9168
      };
      short int subbuffer157[100] = {
       8911, 8653, 8523, 8392, 8260, 7992, 7860, 7730,   64, 8782,
       8652, 8520, 8252, 8120, 7989, 7859, 7601, 7344, 7088, 6961,
       6962, 7091, 7218, 7089,    0,  -16, 7116, 6732, 9423, 9294,
       9421, 9550, 9551, 9424, 9168, 8911, 8653, 8523, 8392, 8260,
       7992, 7860, 7730,   64, 8782, 8652, 8520, 8252, 8120, 7989,
       7859, 7601, 7344, 7088, 6961, 6962, 7091, 7218, 7089,   64,
       8135, 7750, 7492, 7361, 7359, 7484, 7738, 8121, 8377, 8762,
       9020, 9151, 9153, 9028, 8774, 8391, 8135,    0,  -16, 7116,
       6733, 9535, 9405, 9148, 8892, 8637, 8510, 8130, 8003, 7748,
       7492, 7235, 7105, 7103, 7229, 7484, 7740, 7997, 8126, 8514
      };
      short int subbuffer158[100] = {
       8643, 8900, 9156, 9411, 9537, 9535,    0,  -16, 7116, 6732,
       9420, 7095,   64, 7756, 8010, 8008, 7878, 7621, 7365, 7111,
       7113, 7243, 7500, 7756, 8011, 8394, 8778, 9163, 9420,   64,
       8894, 8637, 8507, 8505, 8759, 9015, 9272, 9402, 9404, 9150,
       8894,    0,  -16, 7116, 6733, 9412, 9283, 9410, 9539, 9540,
       9413, 9285, 9156, 9026, 8765, 8506, 8248, 7991, 7607, 7224,
       7098, 7101, 7231, 8003, 8261, 8391, 8393, 8267, 8012, 7755,
       7625, 7623, 7748, 8001, 8634, 8888, 9271, 9399, 9528, 9529,
         64, 7607, 7352, 7226, 7229, 7359, 7617,   64, 7623, 7749,
       8762, 9016, 9271,    0,  -16, 7116, 6606, 8900, 8774, 8519
      };
      short int subbuffer159[100] = {
       8135, 7878, 7749, 7618, 7615, 7741, 7996, 8380, 8637, 8767,
         64, 8135, 7877, 7746, 7743, 7869, 7996,   64, 8903, 8767,
       8765, 9020, 9276, 9534, 9665, 9667, 9542, 9416, 9162, 8907,
       8524, 8140, 7755, 7498, 7240, 7110, 6979, 6976, 7101, 7227,
       7481, 7736, 8119, 8503, 8888, 9145, 9274,   64, 9031, 8895,
       8893, 9020,    0,  -16, 7116, 6986, 8016, 7987,   64, 8528,
       8499,   64, 9033, 8904, 9031, 9160, 9161, 8907, 8524, 8012,
       7627, 7369, 7367, 7493, 7620, 7875, 8641, 8896, 9150,   64,
       7367, 7621, 7876, 8642, 8897, 9024, 9150, 9146, 8888, 8503,
       7991, 7608, 7354, 7355, 7484, 7611, 7482,    0,  -16, 7116
      };
      short int subbuffer160[100] = {
       6987, 8396, 7472,   64, 9164, 8240,   64, 7489, 9281,   64,
       7355, 9147,    0,  -16, 7116, 7240, 8649, 8520, 8647, 8776,
       8777, 8651, 8396, 8140, 7883, 7753, 7751, 7877, 8131, 8768,
         64, 7877, 8514, 8768, 8894, 8892, 8762, 8504,   64, 8004,
       7746, 7616, 7614, 7740, 7994, 8631,   64, 7740, 8377, 8631,
       8757, 8755, 8625, 8368, 8112, 7857, 7731, 7732, 7861, 7988,
       7859,    0,  -16, 7116, 7240, 8268, 8138, 8264, 8394, 8268,
         64, 8268, 8240,   64, 8257, 8126, 8240, 8382, 8257,   64,
       7493, 7748, 8005, 7750, 7493,   64, 7493, 9029,   64, 8517,
       8772, 9029, 8774, 8517,    0,  -16, 7116, 7240, 8268, 8138
      };
      short int subbuffer161[100] = {
       8264, 8394, 8268,   64, 8268, 8254,   64, 8258, 8128, 8380,
       8250, 8124, 8384, 8258,   64, 8254, 8240,   64, 8244, 8114,
       8240, 8370, 8244,   64, 7493, 7748, 8005, 7750, 7493,   64,
       7493, 9029,   64, 8517, 8772, 9029, 8774, 8517,   64, 7479,
       7734, 7991, 7736, 7479,   64, 7479, 9015,   64, 8503, 8758,
       9015, 8760, 8503,    0,  -16, 7116, 7114, 9036, 9015,   64,
       7372, 9036,   64, 8002, 9026,   64, 7351, 9015,    0,  -16,
       7116, 6606, 8140, 7755, 7369, 7110, 6979, 6976, 7101, 7354,
       7736, 8119, 8503, 8888, 9274, 9533, 9664, 9667, 9542, 9289,
       8907, 8524, 8140,   64, 8259, 8130, 8129, 8256, 8384, 8513
      };
      short int subbuffer162[100] = {
       8514, 8387, 8259,   64, 8258, 8257, 8385, 8386, 8258,    0,
        -16, 7116, 7241, 8012, 7755, 7881, 8136,   64, 8012, 7883,
       7881,   64, 8652, 8907, 8777, 8520,   64, 8652, 8779, 8777,
         64, 8136, 7879, 7750, 7620, 7617, 7743, 7870, 8125, 8509,
       8766, 8895, 9025, 9028, 8902, 8775, 8520, 8136,   64, 8253,
       8247,   64, 8381, 8375,   64, 7738, 8890,    0,  -16, 7116,
       7114, 8268, 7883, 7625, 7494, 7493, 7618, 7872, 8255, 8383,
       8768, 9026, 9157, 9158, 9033, 8779, 8396, 8268,   64, 8255,
       8247,   64, 8383, 8375,   64, 7739, 8891,    0,  -16, 7116,
       6478, 8012, 7627, 7241, 6982, 6851, 6847, 6972, 7225, 7607
      };
      short int subbuffer163[100] = {
       7990, 8502, 8887, 9273, 9532, 9663, 9667, 9542, 9289, 8907,
       8524, 8012,   64, 8268, 8246,   64, 6849, 9665,    0,  -16,
       7116, 6862, 8005, 7620, 7362, 7231, 7230, 7355, 7609, 7992,
       8120, 8505, 8763, 8894, 8895, 8770, 8516, 8133, 8005,   64,
       9675, 8907, 9418, 8644,   64, 9675, 9669, 9545, 8771,   64,
       9546, 8772,    0,  -16, 7116, 6730, 7113, 7243, 7500, 7884,
       8139, 8265, 8262, 8131, 8001, 7743, 7357,   64, 7884, 8011,
       8137, 8133, 8002, 7743,   64, 8780, 8503,   64, 8908, 8375,
         64, 7357, 9149,    0,  -16, 7116, 7114, 7628, 7613,   64,
       7756, 7617,   64, 7617, 7747, 7876, 8133, 8517, 8900, 9026
      };
      short int subbuffer164[100] = {
       9024, 8894, 8636,   64, 8517, 8772, 8898, 8896, 8506, 8504,
       8631, 8887, 9145,   64, 7372, 7756,    0,  -16, 7116, 7114,
       8260, 7875, 7617, 7486, 7485, 7610, 7864, 8247, 8375, 8760,
       9018, 9149, 9150, 9025, 8771, 8388, 8260,   64, 8266, 7752,
       8268, 8260,   64, 8394, 8904, 8396, 8388,   64, 8255, 8126,
       8125, 8252, 8380, 8509, 8510, 8383, 8255,   64, 8254, 8253,
       8381, 8382, 8254,    0,  -16, 7116, 6860, 8138, 8268, 8247,
         64, 8522, 8396, 8375,   64, 7242, 7372, 7365, 7490, 7744,
       8127, 8255,   64, 7626, 7500, 7492, 7617,   64, 9418, 9292,
       9285, 9154, 8896, 8511, 8383,   64, 9034, 9164, 9156, 9025
      };
      short int subbuffer165[100] = {
         64, 7739, 8891,    0,  -16, 7116, 6859, 7500, 7479,   64,
       7628, 7607,   64, 7116, 8652, 9035, 9162, 9288, 9285, 9155,
       9026, 8641, 7617,   64, 8652, 8907, 9034, 9160, 9157, 9027,
       8898, 8641,   64, 7095, 9143, 9148, 9015,    0,  -16, 7116,
       6985, 9163, 8651, 8138, 7752, 7493, 7362, 7359, 7484, 7737,
       8119, 8630, 9142,   64, 9163, 8778, 8392, 8133, 8002, 7999,
       8124, 8377, 8759, 9142,    0,  -16, 7116, 6733, 7873, 7617,
       7360, 7231, 7101, 7099, 7225, 7352, 7607, 7863, 8120, 8249,
       8379, 8381, 8255, 8128, 7873,   64, 8394, 8001,   64, 9288,
       8256,   64, 9537, 8382,    0,  -16, 7116, 6986, 7879, 8633
      };
      short int subbuffer166[100] = {
         64, 8647, 7865,   64, 7363, 9149,   64, 9155, 7357,    0,
        -16, 7116, 6732, 7740, 7485, 7357, 7100, 6970, 6969, 7095,
       7350, 7478, 7735, 7865, 7866, 7740, 7360, 7235, 7237, 7368,
       7626, 8011, 8523, 8906, 9160, 9285, 9283, 9152, 8764, 8634,
       8633, 8759, 9014, 9142, 9399, 9529, 9530, 9404, 9149, 9021,
       8764,   64, 7237, 7367, 7625, 8010, 8522, 8905, 9159, 9285,
          0,  -16, 7116, 6732, 7749, 7492, 7364, 7109, 6983, 6984,
       7114, 7371, 7499, 7754, 7880, 7879, 7749, 7361, 7230, 7228,
       7353, 7607, 7990, 8502, 8887, 9145, 9276, 9278, 9153, 8773,
       8647, 8648, 8778, 9035, 9163, 9418, 9544, 9543, 9413, 9156
      };
      short int subbuffer167[100] = {
       9028, 8773,   64, 7228, 7354, 7608, 7991, 8503, 8888, 9146,
       9276,    0,  -16, 7116, 7113, 7353, 9159,   64, 8134, 8646,
       9159, 9027, 9023,   64, 8646, 9030, 9027,    0,  -16, 7116,
       7113, 9159, 7353,   64, 7489, 7485, 7353, 7866, 8378,   64,
       7485, 7482, 7866,    0,  -16, 7116, 7113, 9145, 7367,   64,
       8390, 7878, 7367, 7491, 7487,   64, 7878, 7494, 7491,    0,
        -16, 7116, 7113, 7367, 9145,   64, 8122, 8634, 9145, 9021,
       9025,   64, 8634, 9018, 9021,    0,  -16, 7116, 6733, 7237,
       7110, 7112, 7242, 7499, 7755, 8010, 8137, 8263, 8386,   64,
       7112, 7370, 7626, 7881, 8008, 8134, 8258, 8247,   64, 9413
      };
      short int subbuffer168[100] = {
       9542, 9544, 9418, 9163, 8907, 8650, 8521, 8391, 8258,   64,
       9544, 9290, 9034, 8777, 8648, 8518, 8386, 8375,    0,  -16,
       7116, 6732, 7115, 7239, 7365, 7619, 8002, 8514, 8899, 9157,
       9287, 9419,   64, 7115, 7240, 7366, 7620, 8003, 8515, 8900,
       9158, 9288, 9419,   64, 8003, 7746, 7617, 7487, 7484, 7610,
       7864, 8119, 8375, 8632, 8890, 9020, 9023, 8897, 8770, 8515,
         64, 8002, 7745, 7615, 7612, 7737,   64, 8761, 8892, 8895,
       8769, 8514,    0,  -16, 7116, 6732, 7624, 7612,   64, 7751,
       7741,   64, 8775, 8765,   64, 8904, 8892,   64, 7115, 7369,
       7624, 8007, 8519, 8904, 9161, 9419,   64, 7097, 7355, 7612
      };
      short int subbuffer169[100] = {
       7997, 8509, 8892, 9147, 9401,    0,  -16, 7116, 6732, 9417,
       7497, 7240, 7110, 7108, 7234, 7489, 7745, 8002, 8132, 8134,
       8008, 9416,   64, 7109, 7235, 7362, 7617,   64, 8133, 8007,
       7880, 7625,   64, 7098, 9018, 9275, 9405, 9407, 9281, 9026,
       8770, 8513, 8383, 8381, 8507, 7099,   64, 9406, 9280, 9153,
       8898,   64, 8382, 8508, 8635, 8890,    0,  -16, 7116, 6731,
       7869, 7614, 7486, 7229, 7099, 7098, 7224, 7479, 7607, 7864,
       7994, 7995, 7869, 7234, 7108, 7111, 7241, 7498, 7883, 8395,
       8906, 9160, 9286, 9283, 9152, 8765, 8635, 8633, 8759, 9015,
       9144, 9274,   64, 7615, 7362, 7236, 7239, 7369, 7498,   64
      };
      short int subbuffer170[100] = {
       8395, 8778, 9032, 9158, 9155, 9024, 8765,    0,  -16, 7116,
       6861, 6983, 7370, 7623, 7612,   64, 7241, 7494, 7484,   64,
       7623, 8010, 8263, 8253,   64, 7881, 8134, 8125,   64, 8263,
       8650, 8903, 8887,   64, 8521, 8774, 8759,   64, 8903, 9290,
       9416, 9541, 9538, 9407, 9277, 9019, 8633, 7991,   64, 9161,
       9288, 9413, 9410, 9279, 9149, 8891, 8505, 7991,    0,  -16,
       7116, 6732, 7103, 7615, 7488, 7363, 7365, 7496, 7754, 8139,
       8395, 8778, 9032, 9157, 9155, 9024, 8895, 9407,   64, 7102,
       7870, 7616, 7491, 7493, 7624, 7882, 8139,   64, 8395, 8650,
       8904, 9029, 9027, 8896, 8638, 9406,   64, 7354, 9146,   64
      };
      short int subbuffer171[100] = {
       7353, 9145,    0,  -16, 7116, 6733, 6854, 7241, 7622, 7610,
         64, 7112, 7493, 7482,   64, 7622, 8009, 8390, 8378,   64,
       7880, 8261, 8250,   64, 8390, 8777, 9158, 9147, 9401,   64,
       8648, 9029, 9018, 9272, 9659,    0,  -16, 7116, 6859, 9289,
       7225,   64, 9289, 8904, 8136,   64, 9031, 8647, 8136,   64,
       9289, 9158, 9152,   64, 9031, 9028, 9152,   64, 8128, 7232,
         64, 7999, 7615, 7232,   64, 8128, 8121,   64, 7999, 7996,
       8121,    0,  -16, 7116, 6732, 6979, 7239, 7869,   64, 7237,
       7867, 8258, 8898, 9283, 9413, 9415, 9289, 9034, 8906, 8649,
       8519, 8517, 8642, 8768, 8893, 8890, 8632,   64, 8906, 8777
      };
      short int subbuffer172[100] = {
       8647, 8645, 8897, 9022, 9019, 8889, 8632,    0,  -16, 7116,
       6732, 7107, 7494, 8004,   64, 7365, 7875, 8262, 8644,   64,
       8133, 8515, 8902, 9156,   64, 8773, 9027, 9414,   64, 7101,
       7488, 7998,   64, 7359, 7869, 8256, 8638,   64, 8127, 8509,
       8896, 9150,   64, 8767, 9021, 9408,    0,  -16, 7116, 6732,
       7242, 7752, 8006, 8131, 8128, 7997, 7739, 7225,   64, 7242,
       7625, 7880, 8134, 8259,   64, 8256, 8125, 7867, 7610, 7225,
         64, 9290, 8905, 8648, 8390, 8259,   64, 8256, 8381, 8635,
       8890, 9273,   64, 9290, 8776, 8518, 8387, 8384, 8509, 8763,
       9273,   64, 7106, 9410,   64, 7105, 9409,    0,  -16, 7116
      };
      short int subbuffer173[100] = {
       7748, 8129, 8127, 8383, 8385, 8129,   64, 8129, 8383,   64,
       8385, 8127,    0,  -16, 7116, 7240, 7618, 8128, 8510, 8764,
       8889, 8887, 8757, 8628,   64, 7617, 8382,   64, 7616, 7999,
       8509, 8763, 8889,    0,  -16, 7116, 7240, 8903, 8773, 8515,
       8001, 7616,   64, 8386, 7615,   64, 8652, 8779, 8905, 8903,
       8772, 8514, 8128, 7614,    0,  -16, 7116, 6986, 8389, 7876,
       7490, 7360, 7358, 7484, 7739, 8123, 8636, 9022, 9152, 9154,
       9028, 8773, 8389,   64, 9028, 8389,   64, 8773, 8132, 7490,
         64, 7876, 7360,   64, 7484, 8123,   64, 7739, 8380, 9022,
         64, 8636, 9152,    0,  -16, 7116, 6986, 8389, 7876, 7490
      };
      short int subbuffer174[100] = {
       7360, 7358, 7484, 7739, 8123, 8636, 9022, 9152, 9154, 9028,
       8773, 8389,   64, 9028, 8389,   64, 8773, 8132, 7490,   64,
       7876, 7360,   64, 7484, 8123,   64, 7739, 8380, 9022,   64,
       8636, 9152,    0,  -16, 7116, 7241, 8389, 8004, 7746, 7616,
       7614, 7740, 7995, 8251, 8636, 8894, 9024, 9026, 8900, 8645,
       8389,   64, 7874, 8645,   64, 7744, 8772,   64, 7614, 8899,
         64, 7741, 9026,   64, 7868, 8896,   64, 7995, 8766,    0,
        -16, 7116, 7240, 7883, 7860,   64, 8652, 8629,   64, 7620,
       8902,   64, 7619, 8901,   64, 7611, 8893,   64, 7610, 8892,
          0,  -16, 7116, 7240, 7756, 7738,   64, 8774, 8756,   64
      };
