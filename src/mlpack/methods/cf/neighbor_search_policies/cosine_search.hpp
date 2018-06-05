/**
 * @file cosine_search.hpp
 * @author Wenhao Huang
 *
 * Nearest neighbor search with cosine distance.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_CF_COSINE_SEARCH_HPP
#define MLPACK_METHODS_CF_COSINE_SEARCH_HPP

#include <mlpack/prereqs.hpp>
#include <mlpack/methods/neighbor_search/neighbor_search.hpp>

namespace mlpack {
namespace cf {

/**
 * Nearest neighbor search with cosine distance.
 * Note that, with normalized vectors, neighbor search with cosine distance is
 * equivalent to neighbor search with Euclidean distance. Therefore, instead
 * of performing neighbor search directly with cosine distance, we first
 * normalize all vectors to unit length, and then use neighbor::KNN (i.e.
 * NeighborSearch with Euclidean distance, KDTree). Cosine similarities are
 * calculated from Euclidean distance.
 */
class CosineSearch
{
 public:
  /**
   * Constructor with reference set.
   * All vectors in reference set are normalized to unit length.
   *
   * @param Set of reference points.
   */
  CosineSearch(const arma::mat& referenceSet)
  {
    // Normalize all vectors to unit length.
    arma::mat normalizedSet = arma::normalise(referenceSet, 2, 0);

    neighborSearch.Train(std::move(normalizedSet));
  }

  /**
   * Given a set of query points, find the nearest k neighbors, and return
   * similarities. Similarities are non-negative and no larger than one.
   *
   * @param query A set of query points.
   * @param k Number of neighbors to search.
   * @param neighbors Nearest neighbors.
   * @param similarites Similarities between query point and its neighbors.
   */
  void Search(const arma::mat& query, const size_t k,
      arma::Mat<size_t>& neighbors, arma::mat& similarities)
  {
    // Normalize query vectors to unit length.
    arma::mat normalizedQuery = arma::normalise(query, 2, 0);

    neighborSearch.Search(normalizedQuery, k, neighbors, similarities);

    // Resulting similarities from Search() are Euclidean distance.
    // For unit vectors a and b, cos(a, b) = 1 - dis(a, b) ^ 2 / 2,
    // where dis(a, b) is Euclidean distance.
    similarities = 1 - arma::pow(similarities, 2) / 2.0;

    // The range of cosine value is [-1, 1]. We restrict the range of similarity
    // to be [0, 1].
    similarities = (similarities + 1) / 2.0;
  }

 private:
  //! NeighborSearch object.
  neighbor::KNN neighborSearch;
};

} // namespace cf
} // namespace mlpack

#endif
