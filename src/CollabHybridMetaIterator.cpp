/*  _______________________________________________________________________

    DAKOTA: Design Analysis Kit for Optimization and Terascale Applications
    Copyright 2014 Sandia Corporation.
    This software is distributed under the GNU Lesser General Public License.
    For more information, see the README file in the top Dakota directory.
    _______________________________________________________________________ */

//- Class:       CollabHybridMetaIterator
//- Description: Implementation code for the CollabHybridMetaIterator class
//- Owner:       Patty Hough/John Siirola
//- Checked by:

#include "CollabHybridMetaIterator.hpp"
#include "ProblemDescDB.hpp"
#include "ParallelLibrary.hpp"

static const char rcsId[]="@(#) $Id: CollabHybridMetaIterator.cpp 6715 2010-04-02 21:58:15Z wjbohnh $";


namespace Dakota {

CollabHybridMetaIterator::CollabHybridMetaIterator(ProblemDescDB& problem_db):
  MetaIterator(problem_db)
  //hybridCollabType(
  //  problem_db.get_string("method.hybrid.collaborative_type"))
{
  const StringArray& method_ptrs
    = problem_db.get_sa("method.hybrid.method_pointers");
  const StringArray& method_names
    = problem_db.get_sa("method.hybrid.method_names");
  if (!method_ptrs.empty())
    { methodList = method_ptrs;  lightwtCtor = false; }
  else if (!method_names.empty())
    { methodList = method_names; lightwtCtor = true;  }

  maxIteratorConcurrency = methodList.size();
  if (!maxIteratorConcurrency) { // verify at least one method in list
    if (parallelLib.world_rank() == 0)
      Cerr << "Error: hybrid method list must have a least one entry."
	   << std::endl;
    abort_handler(-1);
  }
}


CollabHybridMetaIterator::
CollabHybridMetaIterator(ProblemDescDB& problem_db, Model& model):
  MetaIterator(problem_db, model)
  //hybridCollabType(
  //  problem_db.get_string("method.hybrid.collaborative_type"))
{
  const StringArray& method_ptrs
    = problem_db.get_sa("method.hybrid.method_pointers");
  const StringArray& method_names
    = problem_db.get_sa("method.hybrid.method_names");
  if (!method_ptrs.empty())
    { methodList = method_ptrs;  lightwtCtor = false; }
  else if (!method_names.empty())
    { methodList = method_names; lightwtCtor = true;  }

  size_t i, num_iterators = methodList.size();
  if (!num_iterators) { // verify at least one method in list
    if (parallelLib.world_rank() == 0)
      Cerr << "Error: hybrid method list must have a least one entry."
	   << std::endl;
    abort_handler(-1);
  }

  // validate iteratedModel against any model pointers
  String empty_str;
  if (!lightwtCtor)
    for (i=0; i<num_iterators; ++i)
      check_model(method_ptrs[i], empty_str);
  else {
    StringArray model_ptrs = probDescDB.get_sa("method.hybrid.model_pointers");
    if (!model_ptrs.empty()) {
      Pecos::inflate_scalar(model_ptrs, num_iterators);
      for (i=0; i<num_iterators; ++i)
	check_model(empty_str, model_ptrs[i]);
    }
  }

  maxIteratorConcurrency = num_iterators;
}


CollabHybridMetaIterator::~CollabHybridMetaIterator()
{
  // Virtual destructor handles referenceCount at Iterator level.
}


void CollabHybridMetaIterator::derived_init_communicators(ParLevLIter pl_iter)
{
  size_t i, num_iterators = methodList.size();
  StringArray model_ptrs; bool models = false;
  if (lightwtCtor) {
    model_ptrs = probDescDB.get_sa("method.hybrid.model_pointers");
    if (!model_ptrs.empty()) models = true;
  }
  if (models)
    Pecos::inflate_scalar(model_ptrs, num_iterators);
  selectedIterators.resize(num_iterators); // slaves also need for run_iterator
  if (!lightwtCtor || models) // this test is conservative
    selectedModels.resize(num_iterators);

  int min_ppi = INT_MAX, max_ppi = 0;
  std::pair<int, int> ppi_pr; String empty_str; BitArray new_mod(num_iterators);
  for (i=0; i<num_iterators; ++i) {
    if (lightwtCtor) {
      const String& model_ptr = (models) ? model_ptrs[i] : empty_str;
      new_mod[i] = new_model(empty_str, model_ptr);
      Model& selected_model = (new_mod[i]) ? selectedModels[i] : iteratedModel;
      ppi_pr = estimate_by_name(methodList[i], model_ptr,
				selectedIterators[i], selected_model);
    }
    else {
      new_mod[i] = new_model(methodList[i], empty_str);
      Model& selected_model = (new_mod[i]) ? selectedModels[i] : iteratedModel;
      ppi_pr = estimate_by_pointer(methodList[i], selectedIterators[i],
				   selected_model);
    }
    if (ppi_pr.first  < min_ppi) min_ppi = ppi_pr.first;
    if (ppi_pr.second > max_ppi) max_ppi = ppi_pr.second;
  }

  iterSched.init_iterator_parallelism(maxIteratorConcurrency, min_ppi, max_ppi);
  size_t pl_index = parallelLib.parallel_level_index(pl_iter);
  miPLIndexMap[pl_index] = iterSched.miPLIndex; // same or one beyond pl_iter
  summaryOutputFlag = iterSched.lead_rank();
  // from this point on, we can specialize logic in terms of iterator servers.
  // An idle partition need not instantiate iterators/models (empty Iterator
  // envelopes are adequate for serve_iterators()), so return now.
  if (iterSched.iteratorServerId > iterSched.numIteratorServers)
    return;

  // Instantiate all Models and Iterators
  if (lightwtCtor)
    for (i=0; i<num_iterators; ++i) {
      const String& model_ptr = (models) ? model_ptrs[i] : empty_str;
      Model& selected_model = (new_mod[i]) ? selectedModels[i] : iteratedModel;
      allocate_by_name(methodList[i], model_ptr, selectedIterators[i],
		       selected_model);
    }
  else
    for (i=0; i<num_iterators; ++i) {
      Model& selected_model = (new_mod[i]) ? selectedModels[i] : iteratedModel;
      allocate_by_pointer(methodList[i], selectedIterators[i], selected_model);
    }
  // Note: rather than the standard allocate_methods() approach, this is where
  // logic would be placed to process the model list and create
  // CollaborativeModel recursions, one for each unique user-defined model.
  // These CollaborativeModels would then be passed into the selectedIterators
  // instantiations and would support a shared processing queue.
}


void CollabHybridMetaIterator::derived_set_communicators(ParLevLIter pl_iter)
{
  size_t pl_index = parallelLib.parallel_level_index(pl_iter),
      mi_pl_index = miPLIndexMap[pl_index]; // same or one beyond pl_iter
  iterSched.update(mi_pl_index);
  if (iterSched.iteratorServerId <= iterSched.numIteratorServers) {
    ParLevLIter si_pl_iter
      = methodPCIter->mi_parallel_level_iterator(mi_pl_index);
    size_t i, num_iterators = methodList.size();
    for (i=0; i<num_iterators; ++i)
      iterSched.set_iterator(selectedIterators[i], si_pl_iter);
  }

  /* See notes in NestedModel::derived_set_communicators()
  size_t i, num_iterators = methodList.size(),
    mi_pl_index = methodPCIter->mi_parallel_level_index(pl_iter);
  iterSched.update(mi_pl_index);
  if (pl_iter->message_pass() || pl_iter->idle_partition()) { // wrong level!
    ParLevLIter next_pl_iter
      = methodPCIter->mi_parallel_level_iterator(++mi_pl_index);
    for (i=0; i<num_iterators; ++i)
      iterSched.set_iterator(selectedIterators[i], next_pl_iter);
  }
  else
    for (i=0; i<num_iterators; ++i)
      iterSched.set_iterator(selectedIterators[i], pl_iter);
  */
}


void CollabHybridMetaIterator::derived_free_communicators(ParLevLIter pl_iter)
{
  // free the communicators for selectedIterators
  size_t pl_index = parallelLib.parallel_level_index(pl_iter),
      mi_pl_index = miPLIndexMap[pl_index]; // same or one beyond pl_iter
  iterSched.update(mi_pl_index);
  if (iterSched.iteratorServerId <= iterSched.numIteratorServers) {
    ParLevLIter si_pl_iter
      = methodPCIter->mi_parallel_level_iterator(mi_pl_index);
    size_t i, num_iterators = methodList.size();
    for (i=0; i<num_iterators; ++i)
      iterSched.free_iterator(selectedIterators[i], si_pl_iter);
  }

  /* See notes in NestedModel::derived_set_communicators()
  size_t i, num_iterators = methodList.size(),
    mi_pl_index = methodPCIter->mi_parallel_level_index(pl_iter);
  iterSched.update(mi_pl_index);
  if (pl_iter->message_pass() || pl_iter->idle_partition()) { // ***
    ParLevLIter next_pl_iter
      = methodPCIter->mi_parallel_level_iterator(++mi_pl_index);
    for (i=0; i<num_iterators; ++i)
      iterSched.free_iterator(selectedIterators[i], next_pl_iter);
  }
  else
    for (i=0; i<num_iterators; ++i)
      iterSched.free_iterator(selectedIterators[i], pl_iter);
  */

  // deallocate the mi_pl parallelism level
  iterSched.free_iterator_parallelism();

  miPLIndexMap.erase(pl_index);
}


void CollabHybridMetaIterator::core_run()
{
  // THIS CODE IS JUST A PLACEHOLDER

  bool lead_rank = iterSched.lead_rank();
  size_t i, num_iterators = methodList.size();
  int server_id =  iterSched.iteratorServerId;
  bool    rank0 = (iterSched.iteratorCommRank == 0);
  for (i=0; i<num_iterators; i++) {

    if (lead_rank)
      Cout << "\n>>>>> Running Collaborative Hybrid with iterator "
	   << methodList[i] << ".\n";

    Iterator& curr_iterator = selectedIterators[i];

    // For graphics data, limit to iterator server comm leaders; this is
    // further segregated within initialize_graphics(): all iterator masters
    // stream tabular data, but only server 1 generates a graphics window.
    if (rank0 && server_id > 0 && server_id <= iterSched.numIteratorServers)
      curr_iterator.initialize_graphics(server_id);

    iterSched.schedule_iterators(*this, curr_iterator);
  }
}

} // namespace Dakota
