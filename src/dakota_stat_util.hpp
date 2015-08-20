/*  _______________________________________________________________________

    DAKOTA: Design Analysis Kit for Optimization and Terascale Applications
    Copyright 2014 Sandia Corporation.
    This software is distributed under the GNU Lesser General Public License.
    For more information, see the README file in the top Dakota directory.
    _______________________________________________________________________ */

//- Classes     : BootstrapSamplerBase, BootstrapSampler, BootstrapSamplerWithGS
//- Description : Functors for performing bootstrap sampling on a dataset
//- Owner       : Carson Kent
//- Checked by  :
//- Version     :

#ifndef __DAKOTA_STAT_UTIL_H__
#define __DAKOTA_STAT_UTIL_H__

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#define BOOST_NUMERIC_FUNCTIONAL_STD_VECTOR_SUPPORT
#define BOOST_NUMERIC_FUNCTIONAL_STD_VALARRAY_SUPPORT
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include "Teuchos_SerialDenseVector.hpp"
#include "Teuchos_SerialDenseHelpers.hpp"

namespace Dakota {

/// Base class/interface for the bootstrap sampler

/** BootstrapSamplerBase defines the minimum interface for a bootstrap sampler
    and handles initialization of the random variate generation used by the
    bootstrap. Functor is templated on the data type, but does not actually
    define a data member. */
template<typename Data>
class BootstrapSamplerBase
{
public:

  //
  //- Heading: Static members for access to random variate generation
  //

  static void set_seed(size_t seed)
  {
    bootstrapRNG.seed(seed);
  }

  //
  //- Heading: Constructors and destructor
  //

  /// Constructor for the bootstrap functor base
  BootstrapSamplerBase(size_t data_size, Data orig_data) : dataSize(data_size),
                          origData(orig_data), sampler(0, data_size - 1)
  {
  }

  /// Destructor
  virtual ~BootstrapSamplerBase()
  {
    /* empty destructor */
  }

  //
  //- Heading: Public members functions that perform for boostrap sampling
  //

  /// Generate and store a new boostrapped sample into bootstrapped_sample
  virtual void operator()(size_t num_samp, Data& bootstrapped_sample) = 0;

  /// Obatin the number of samples used in the empirical distribution
  virtual size_t getDataSize()
  {
    return dataSize;
  }

  /// Generate and store an dataSize out of dataSize boostrap sample
  virtual void operator()(Data& bootstrapped_sample)
  {
    (*this)(dataSize, bootstrapped_sample);
  }

  /// Return boostrapped sample
  virtual Data operator()()
  {
    Data sample(origData);
    (*this)(dataSize, sample);
    return sample;
  }

protected:

  // Internal static members for random variate generation

  /// Random number generator to use for sampling
  static boost::random::mt19937 bootstrapRNG;

  // Internal instance members

  /// Uniform distribution to provide samples from the empirical distribution
  boost::random::uniform_int_distribution<> sampler;

  /// Size of the dataset defining the empirical distribution
  const size_t dataSize;

  /// Original data defining the empirical distribution
  /// TODO: Consider if it should be const (breaks Teuchos)
  Data origData;
};

/// The boostrap random number generator
template<typename Data>
boost::random::mt19937 BootstrapSamplerBase<Data>::bootstrapRNG;


/// Acutal boostrap sampler implementation for common data types

template<typename Data>
class BootstrapSampler : public BootstrapSamplerBase<Data>
{
public:

  //
  //- Heading: Type definitions and aliases
  //

  typedef boost::accumulators::accumulator_set<typename Data::value_type,
            boost::accumulators::stats<boost::accumulators::tag::mean> >
            genStats;

  using BootstrapSamplerBase<Data>::operator();

  //
  //- Heading: Constructors and destructor
  //

  /// Constructor for the sampler. Current requirement is that data_size and
  /// the size of orig_data must agree.
  /// TODO: Provide methods to avoid the specification of data_size
  BootstrapSampler(const Data& orig_data, size_t block_size = 1) :
    BootstrapSamplerBase<Data>::BootstrapSamplerBase(block_size ?
                                                     orig_data.size()/block_size :
                                                     orig_data.size(), orig_data),
    blockSize(block_size ? block_size : 1)
  {
    if(block_size &&
      (block_size > this->dataSize || this->dataSize % block_size != 0))
        throw "Boostrap sampler data size must be a multiple of block size.";
  }

  /// Destructor
  virtual ~BootstrapSampler()
  {
    /* empty destructor */
  }

  //
  //- Heading: Public members functions that perform for boostrap sampling
  //

  /// \copydoc
  virtual void operator()(size_t num_samp, Data& bootstrapped_sample)
  {
    if(num_samp > bootstrapped_sample.size()/blockSize)
      throw
        std::out_of_range("Number of samples exceeds the size of container");

    typename Data::iterator beg_data = this->origData.begin();
    for(typename Data::iterator sample = bootstrapped_sample.begin();
        sample != bootstrapped_sample.end(); sample += blockSize)
    {
      typename Data::iterator beg_block = beg_data + sampler(this->bootstrapRNG)
                                          * blockSize;
      for(size_t i = 0; i < blockSize; ++i)
      {
        *(sample + i) = *(beg_block + i);
      }
    }
  }

  // /// \copydoc
  // template<typename Accumulator>
  // Accumulator bootstrappedStatistics(size_t num_samp)
  // {
  //   // std::vector<Accumulator> accs(blockSize);

  //   Accumulator acc;
  //   typename Data::iterator beg_data = this->origData.begin();
  //   for(size_t i = 0; i < num_samp; ++i)
  //   {
  //     typename Data::iterator beg_block = beg_data +
  //       sampler(this->bootstrapRNG) * blockSize;

  //     acc(Data(beg_block, beg_block + blockSize));
  //     // for(size_t j = 0; j < blockSize; ++j)
  //     // {
  //     //   accs[j](*(beg_block + j));
  //     // }
  //   }
  //   return acc;
  //   // return accs;
  // }

  /// Convenience method returning accumulators for common statistics
  // std::vector<genStats> genBSStatistics(size_t num_samp)
  // {
  //   return this->bootstrappedStatistics<genStats>(num_samp);
  // }

  /// Convenience method for common statistics using original data size
  // std::vector<genStats> genBSStatistics()
  // {
  //   return this->bootstrappedStatistics<genStats>(this->dataSize);
  // }

protected:

  // Internal instance members

  /// Size of the block defining a sample
  size_t blockSize;
};


/// Bootstrap sampler that is specialized to allow for the boostrapping of
/// RealMatrix

template<typename OrdinalType, typename ScalarType>
class BootstrapSampler<Teuchos::SerialDenseMatrix<OrdinalType, ScalarType> > :
  public BootstrapSamplerBase<Teuchos::
                              SerialDenseMatrix<OrdinalType, ScalarType> >
{
public:

  //
  //- Heading: Type definitions and aliases
  //

  /// Convenience definition
  typedef Teuchos::SerialDenseMatrix<OrdinalType, ScalarType> MatType;

  using BootstrapSamplerBase<MatType>::operator();

  /// Constructor for the sampler. Current requirement is that data_size and
  /// the size of orig_data must agree.
  /// TODO: Provide methods to avoid the specification of data_size
  BootstrapSampler(const MatType& orig_data, size_t block_size = 1) :
    BootstrapSamplerBase<MatType>::BootstrapSamplerBase(block_size ?
                                                        orig_data.numCols()/block_size :
                                                        orig_data.numCols(), orig_data),
    blockSize(block_size ? block_size : 1)
  {
    if(block_size &&
      (block_size > this->dataSize || this->dataSize % block_size != 0))
        throw "Boostrap sampler data size must be a multiple of block size.";
  }

  /// Destructor
  virtual ~BootstrapSampler()
  {
    /* empty destructor */
  }

  //
  //- Heading: Public members functions that perform boostrap sampling
  //

  /// \copydoc
  virtual void operator()(size_t num_samp, MatType& bootstrapped_sample)
  {
    OrdinalType stride = this->origData.stride();
    if(stride != bootstrapped_sample.stride())
      throw
          std::out_of_range("Dimension of a boostrapped sample differs from "
                            "the dimension of the original dataset");

    if(num_samp > bootstrapped_sample.numCols()/blockSize)
      throw
        std::out_of_range("Number of samples exceeds the size of container");

    for(int i = 0; i < num_samp * blockSize; i += blockSize)
    {
      std::memcpy(bootstrapped_sample[i],
        this->origData[sampler(this->bootstrapRNG) * blockSize],
        blockSize * stride * sizeof(ScalarType));
    }
  }

  // template<typename Accumulator>
  // Accumulator bootstrappedStatistics(size_t num_samp)
  // {
  //   // std::vector<Accumulator> accs(blockSize);

  //   Accumulator acc;
  //   for(size_t i = 0; i < num_samp; ++i)
  //   {

  //     acc(MatType(Teuchos::View,
  //                 this->origData,
  //                 this->origData.numRows(),
  //                 blockSize,
  //                 0,
  //                 sampler(this->bootstrapRNG) * blockSize));
  //     // for(size_t j = 0; j < blockSize; ++j)
  //     // {
  //     //   accs[j](*(beg_block + j));
  //     // }
  //   }
  //   return acc;
  //   // return accs;
  // }

  /// Convenience method returning accumulators for common statistics
  // std::vector<genStats> genBSStatistics(size_t num_samp)
  // {
  //   return this->bootstrappedStatistics<genStats>(num_samp);
  // }

  /// Convenience method for common statistics using original data size
  // std::vector<genStats> genBSStatistics()
  // {
  //   return this->bootstrappedStatistics<genStats>(this->dataSize);
  // }

protected:

  // Internal instance members

  /// Size of the block defining a sample
  size_t blockSize;
};


/// A derived sampler to allow for user specification of the accessor methods

template<typename Data, typename Getter, typename Setter>
class BootstrapSamplerWithGS : public BootstrapSampler<Data>
{
public:

  using BootstrapSampler<Data>::operator();

  //
  //- Heading: Constructors and destructor
  //

  /// Constructor with extra arguments for the accessor methods
  BootstrapSamplerWithGS(const Data& orig_data,
                         Getter getter_method,
                         Setter setter_method) :
                         BootstrapSampler<Data>::BootstrapSampler(orig_data),
                         getterMethod(getter_method),
                         setterMethod(setter_method)
  {
  }

  /// Destructor
 virtual  ~BootstrapSamplerWithGS()
  {
    /* empty destructor */
  }

  //
  //- Heading: Public members functions that perform for boostrap sampling
  //

  /// Generate and store a new boostrapped sample into bootstrapped_sample
  /// TODO: bounds checking
  virtual void operator()(size_t num_samp, Data& bootstrapped_sample)
  {
    for(size_t i = 0; i < num_samp; ++i)
    {
      setterMethod(i, getterMethod(this->sampler(this->bootstrapRNG),
                                   this->origData), bootstrapped_sample);
    }
  }

protected:

  // Internal instance members

  /// Function to obtain a single sample from a Data object. Function should
  /// take a Data object and an unsigned integer corresponding to a sample
  /// index and return the sample
  Getter getterMethod;

  /// Function to place a single sample into a Data object. Function should
  /// take a Data object and an unsigned integer corresponding to the sample
  /// index to set.
  Setter setterMethod;
};

}

#endif // __DAKOTA_STAT_UTIL_H__
