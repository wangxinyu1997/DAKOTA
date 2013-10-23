/*  _______________________________________________________________________

    DAKOTA: Design Analysis Kit for Optimization and Terascale Applications
    Copyright (c) 2010, Sandia National Laboratories.
    This software is distributed under the GNU Lesser General Public License.
    For more information, see the README file in the top Dakota directory.
    _______________________________________________________________________ */

#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include "dakota_system_defs.hpp"
#include "Teuchos_SerialDenseVector.hpp"
#include <boost/multi_array.hpp>
#include "boost/dynamic_bitset.hpp"
#include <list>

namespace Dakota {

// avoid problems with circular dependencies by using fwd declarations
class Iterator;
class Model;
class Variables;
class Interface;
class Response;
class ActiveSet;
class ParamResponsePair;
class ParallelLevel;
class ParallelConfiguration;


// -----------------------------------
// Aliases for fundamental data types:
// -----------------------------------
typedef double Real;
typedef std::string String;

// -----------------------------------
// Numerical arrays (serial/parallel):
// -----------------------------------

typedef Teuchos::SerialDenseVector<int, Real>    RealVector;
typedef Teuchos::SerialDenseMatrix<int, Real>    RealMatrix;
typedef Teuchos::SerialSymDenseMatrix<int, Real> RealSymMatrix;
typedef Teuchos::SerialDenseVector<int, int>     IntVector;
typedef Teuchos::SerialDenseMatrix<int, int>     IntMatrix;

//typedef Tpetra::CrsMatrix<int, Real>             RealSparseMatrix
//typedef Tpetra::Vector<int, Real>                RealParallelVector
//typedef Tpetra::MultiVector<int, Real>           RealParallelMatrix

// ---------------------------------------
// Admin/bookkeeping arrays (serial only):
// ---------------------------------------
typedef std::deque<bool> BoolDeque; // See Meyers' Effective STL #18
typedef boost::dynamic_bitset<unsigned long> BitArray;

typedef std::vector<BoolDeque>      BoolDequeArray;
typedef std::vector<Real>           RealArray;
typedef std::vector<RealArray>      Real2DArray;
typedef std::vector<int>            IntArray;
typedef std::vector<IntArray>       Int2DArray;
typedef std::vector<short>          ShortArray;
typedef std::vector<unsigned short> UShortArray;
typedef std::vector<UShortArray>    UShort2DArray;
typedef std::vector<UShort2DArray>  UShort3DArray;
typedef std::vector<size_t>         SizetArray;
typedef std::vector<SizetArray>     Sizet2DArray;
typedef std::vector<String>         StringArray;
typedef std::vector<StringArray>    String2DArray;

typedef boost::multi_array_types::index_range       idx_range;
typedef boost::multi_array<String, 1>               StringMultiArray;
typedef StringMultiArray::array_view<1>::type       StringMultiArrayView;
typedef StringMultiArray::const_array_view<1>::type StringMultiArrayConstView;
typedef boost::multi_array<unsigned short, 1>       UShortMultiArray;
typedef UShortMultiArray::array_view<1>::type       UShortMultiArrayView;
typedef UShortMultiArray::const_array_view<1>::type UShortMultiArrayConstView;
typedef boost::multi_array<size_t, 1>               SizetMultiArray;
typedef SizetMultiArray::array_view<1>::type        SizetMultiArrayView;
typedef SizetMultiArray::const_array_view<1>::type  SizetMultiArrayConstView;

typedef std::vector<RealVector>        RealVectorArray;
typedef std::vector<RealVectorArray>   RealVector2DArray;
typedef std::vector<RealMatrix>        RealMatrixArray;
typedef std::vector<RealSymMatrix>     RealSymMatrixArray;
typedef std::vector<IntVector>         IntVectorArray;
typedef std::vector<Variables>         VariablesArray;
typedef std::vector<Response>          ResponseArray;
typedef std::vector<ParamResponsePair> PRPArray;
typedef std::vector<PRPArray>          PRP2DArray;
typedef std::vector<Model>             ModelArray;
typedef std::vector<Iterator>          IteratorArray;

typedef std::list<bool>                BoolList;
typedef std::list<int>                 IntList;
typedef std::list<size_t>              SizetList;
typedef std::list<Real>                RealList;
typedef std::list<String>              StringList;
typedef std::list<Variables>           VariablesList;
typedef std::list<Interface>           InterfaceList;
typedef std::list<Response>            ResponseList;
typedef std::list<Model>               ModelList;
typedef std::list<Iterator>            IteratorList;
//typedef std::list<ParallelLevel>         ParLevList;
//typedef std::list<ParallelConfiguration> ParConfigList;

typedef std::pair<int, int>            IntIntPair;
typedef std::pair<int, String>         IntStringPair;
typedef std::pair<Real, Real>          RealRealPair;
typedef std::pair<int, Response>       IntResponsePair;
typedef std::set<Real>                 RealSet;
typedef std::set<int>                  IntSet;
typedef std::set<unsigned short>       UShortSet;
typedef std::set<size_t>               SizetSet;
typedef std::vector<RealSet>           RealSetArray;
typedef std::vector<IntSet>            IntSetArray;
typedef std::vector<UShortSet>         UShortSetArray;
typedef std::map<int, int>             IntIntMap;
typedef std::map<int, short>           IntShortMap;
typedef std::map<int, Real>            IntRealMap;
typedef std::map<Real, Real>           RealRealMap;
typedef std::vector<IntRealMap>        IntRealMapArray;
typedef std::vector<RealRealMap>       RealRealMapArray;
typedef std::map<int, RealVector>      IntRealVectorMap;
typedef std::map<int, ActiveSet>       IntActiveSetMap;
typedef std::map<int, Variables>       IntVariablesMap;
typedef std::map<int, Response>        IntResponseMap;
typedef std::map<IntArray, size_t>     IntArraySizetMap;
typedef std::multimap<RealRealPair, ParamResponsePair> RealPairPRPMultiMap;
//typedef std::multimap<Real, ParamResponsePair> RealPRPMultiMap;

// ---------
// Iterators
// ---------
typedef IntList::iterator                          ILIter;
typedef IntList::const_iterator                    ILCIter;
typedef SizetList::iterator                        StLIter;
typedef SizetList::const_iterator                  StLCIter;
typedef RealList::iterator                         RLIter;
typedef RealList::const_iterator                   RLCIter;
typedef StringList::iterator                       StringLIter;
typedef StringList::const_iterator                 StringLCIter;
typedef VariablesList::iterator                    VarsLIter;
typedef InterfaceList::iterator                    InterfLIter;
typedef ResponseList::iterator                     RespLIter;
typedef ModelList::iterator                        ModelLIter;
typedef ModelList::reverse_iterator                ModelLRevIter;
typedef IteratorList::iterator                     IterLIter;
typedef std::list<ParallelLevel>::iterator         ParLevLIter;
typedef std::list<ParallelConfiguration>::iterator ParConfigLIter;

typedef IntSet::iterator                       ISIter;
typedef IntSet::const_iterator                 ISCIter;
typedef RealSet::iterator                      RSIter;
typedef RealSet::const_iterator                RSCIter;
typedef IntIntMap::iterator                    IntIntMIter;
typedef IntIntMap::const_iterator              IntIntMCIter;
typedef IntShortMap::iterator                  IntShMIter;
typedef IntShortMap::const_iterator            IntShMCIter;
typedef IntRealMap::iterator                   IRMIter;
typedef IntRealMap::const_iterator             IRMCIter;
typedef RealRealMap::iterator                  RRMIter;
typedef RealRealMap::const_iterator            RRMCIter;
typedef IntRealVectorMap::iterator             IntRDVMIter;
typedef IntRealVectorMap::const_iterator       IntRDVMCIter;
typedef IntActiveSetMap::iterator              IntASMIter;
typedef IntVariablesMap::iterator              IntVarsMIter;
typedef IntVariablesMap::const_iterator        IntVarsMCIter;
typedef IntResponseMap::iterator               IntRespMIter;
typedef IntResponseMap::const_iterator         IntRespMCIter;

} // namespace Dakota

#endif // DATA_TYPES_H
