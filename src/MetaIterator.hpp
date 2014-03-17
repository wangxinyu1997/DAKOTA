/*  _______________________________________________________________________

    DAKOTA: Design Analysis Kit for Optimization and Terascale Applications
    Copyright (c) 2010, Sandia National Laboratories.
    This software is distributed under the GNU Lesser General Public License.
    For more information, see the README file in the top Dakota directory.
    _______________________________________________________________________ */

//- Class:       MetaIterator
//- Description: A multi-level hybrid minimizer which invokes several iterators
//- Owner:       Mike Eldred
//- Checked by:
//- Version: $Id: MetaIterator.hpp 6492 2009-12-19 00:04:28Z briadam $

#ifndef META_ITERATOR_H
#define META_ITERATOR_H

#include "dakota_data_types.hpp"
#include "DakotaIterator.hpp"
#include "DakotaModel.hpp"
#include "IteratorScheduler.hpp"


namespace Dakota {


/// Base class for meta-iterators.

/** This base class shares code for concurrent and hybrid
    meta-iterators, where the former supports multi-start and Pareto
    set iteration and the latter supports sequential, embedded, and
    collaborative hybrids. */

class MetaIterator: public Iterator
{
public:
  
protected:
  
  //
  //- Heading: Constructors and destructor
  //

  /// standard constructor
  MetaIterator(ProblemDescDB& problem_db);
  /// alternate constructor
  MetaIterator(ProblemDescDB& problem_db, Model& model);
  /// destructor
  ~MetaIterator();

  //
  //- Heading: Convenience member functions
  //

  /// initialize the_iterator and the_model based on method_ptr
  void allocate_by_pointer(const String& method_ptr, Iterator& the_iterator,
			   Model& the_model);
  /// initialize the_iterator based on method_string
  void allocate_by_name(const String& method_string, const String& model_ptr,
			Iterator& the_iterator,	Model& the_model);
  /// free communicators for the_iterator and the_model
  void deallocate(Iterator& the_iterator, Model& the_model);

  //
  //- Heading: Data members
  //

  /// scheduler for concurrent execution of Iterators
  IteratorScheduler iterSched;
  /// maximum number of concurrent sub-iterator executions
  int maxIteratorConcurrency;
};

} // namespace Dakota

#endif