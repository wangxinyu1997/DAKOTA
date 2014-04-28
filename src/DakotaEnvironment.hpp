/*  _______________________________________________________________________

    DAKOTA: Design Analysis Kit for Optimization and Terascale Applications
    Copyright (c) 2010, Sandia National Laboratories.
    This software is distributed under the GNU Lesser General Public License.
    For more information, see the README file in the top Dakota directory.
    _______________________________________________________________________ */

//- Class:        Environment
//- Description:  Abstract base class to logically represent a variety
//-               of Dakota environment objects in a very generic fashion.
//- Owner:        Mike Eldred
//- Version: $Id: DakotaEnvironment.hpp 6492 2009-12-19 00:04:28Z briadam $

#ifndef DAKOTA_ENVIRONMENT_H
#define DAKOTA_ENVIRONMENT_H

#include "dakota_data_types.hpp"
#include "MPIManager.hpp"
#include "ProgramOptions.hpp"
#include "OutputManager.hpp"
#include "ParallelLibrary.hpp"
#include "ProblemDescDB.hpp"
#include "DakotaIterator.hpp"

/// The primary namespace for DAKOTA

/** The Dakota namespace encapsulates the core classes of the DAKOTA
    framework and prevents name clashes with third-party libraries
    from methods and packages.  The C++ source files defining these
    core classes reside in Dakota/src as *.[ch]pp. */

namespace Dakota {


/// Base class for the environment class hierarchy.

/** The Environment class is the base class for the class hierarchy
    providing the top level control in DAKOTA.  The environment is
    responsible for creating and managing iterators and models.  For
    memory efficiency and enhanced polymorphism, the environment
    hierarchy employs the "letter/envelope idiom" (see Coplien
    "Advanced C++", p. 133), for which the base class (Environment)
    serves as the envelope and one of the derived classes (selected in
    Environment::get_environment()) serves as the letter. */

class Environment
{
public:

  //
  //- Heading: Constructors, destructor, assignment operator
  //

  /// default constructor: empty envelope
  Environment();
  /// envelope constructor for ExecutableEnvironment letter 
  Environment(int argc, char* argv[]);
  // envelope constructor for LibraryEnvironment letter 
  Environment(ProgramOptions prog_opts);
  // envelope constructor for LibraryEnvironment letter 
  Environment(MPI_Comm dakota_mpi_comm, 
	      ProgramOptions prog_opts = ProgramOptions());
  /// envelope constructor for letter type identifed by String
  Environment(const String& env_type);
  /// copy constructor
  Environment(const Environment& env);

  /// destructor
  virtual ~Environment();

  /// assignment operator
  Environment operator=(const Environment& env);

  //
  //- Heading: Virtual functions
  //

  /// the run function for the environment: invoke the iterator(s) on
  /// the model(s).  Called from main.cpp.
  virtual void execute();

  //
  //- Heading: Member functions
  //

  /// return mpiManager
  MPIManager& mpi_manager();
  /// return programOptions
  ProgramOptions& program_options();
  /// return outputManager
  OutputManager& output_manager();

  /// return parallelLib
  ParallelLibrary& parallel_library();
  /// return probDescDB
  ProblemDescDB& problem_description_db();

  /// return the final environment solution (variables)
  const Variables& variables_results() const;
  /// return the final environment solution (response)
  const Response&  response_results() const;

protected:

  //
  //- Heading: Constructors
  //

  /// constructor initializes the base class part of default-constructed letters
  Environment(BaseConstructor);
  /// constructor initializes the base class part of executable letter classes
  Environment(BaseConstructor, int argc, char* argv[]);
  /// constructor initializes the base class part of library letter classes
  Environment(BaseConstructor, ProgramOptions prog_opts,
	      MPI_Comm dakota_mpi_comm = MPI_COMM_WORLD);

  //
  //- Heading: Member functions
  //

  /// parse inputs, callbacks, and optionally check and broadcast
  void parse(bool check_bcast_database = true, 
	     DbCallbackFunctionPtr callback = NULL, void* callback_data = NULL);

  /// Instantiate topLevelIterator
  void construct();
  /// Deallocate parallel partitioning for topLevelIterator
  void destruct();

  //
  //- Heading: Data
  //

  // Note: pushing these down into derived class allows initializer list usage.
  // Alternative is to separate construction from initialization.

  MPIManager      mpiManager;     ///< the MPI manager instance
  ProgramOptions  programOptions; ///< the command line options manager
  OutputManager   outputManager;  ///< (tagged) output stream manager

  ParallelLibrary parallelLib; ///< the parallel library instance
  ProblemDescDB   probDescDB;  ///< the parser database instance

  Iterator topLevelIterator;  ///< the top level (meta-)iterator
  //Model  topLevelModel;     //   the top level Model for non-meta-iteration

private:

  //
  //- Heading: Member functions
  //

  /// Used by the envelope to instantiate the correct letter class.
  Environment* get_environment(const String& env_type);

  //
  //- Heading: Data
  //

  /// pointer to the letter (initialized only for the envelope)
  Environment* environmentRep;
  /// number of objects sharing environmentRep
  int referenceCount;
};


inline MPIManager& Environment::mpi_manager()
{ return (environmentRep) ? environmentRep->mpiManager : mpiManager; }


inline ProgramOptions& Environment::program_options()
{ return (environmentRep) ? environmentRep->programOptions : programOptions; }


inline OutputManager& Environment::output_manager()
{ return (environmentRep) ? environmentRep->outputManager : outputManager; }


inline ParallelLibrary& Environment::parallel_library()
{ return (environmentRep) ? environmentRep->parallelLib : parallelLib; }


inline ProblemDescDB& Environment::problem_description_db()
{ return (environmentRep) ? environmentRep->probDescDB : probDescDB; }


inline const Variables& Environment::variables_results() const
{
  return (environmentRep) ?
    environmentRep->topLevelIterator.variables_results() :
    topLevelIterator.variables_results();
}


inline const Response& Environment::response_results() const
{
  return (environmentRep) ?
    environmentRep->topLevelIterator.response_results() :
    topLevelIterator.response_results();
}

} // namespace Dakota

#endif
