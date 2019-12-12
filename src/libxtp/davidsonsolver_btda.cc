/*
 * Copyright 2009-2019 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <stdexcept>

#include <votca/xtp/davidsonsolver_btda.h>
#include <votca/xtp/eigen.h>

using boost::format;
using std::flush;

namespace votca {
namespace xtp {

using namespace std;

Eigen::MatrixXd DavidsonSolver_BTDA::setupInitialEigenvectors(
    Index size_initial_guess) const {

  Eigen::MatrixXd guess =
      Eigen::MatrixXd::Zero(_preconditioner.size(), size_initial_guess);
  ArrayXl idx = DavidsonSolver_BTDA::argsort(_preconditioner);

  Index ind0 = _preconditioner.size() / 2;
  for (Index j = 0; j < size_initial_guess; j++) {
    guess(idx(ind0 + j), j) = 1.0;
  }

  return guess;
}

ArrayXl DavidsonSolver_BTDA::argsort(const Eigen::VectorXd &V) const {
  /* \brief return the index of the sorted vector */
  ArrayXl idx = ArrayXl::LinSpaced(V.rows(), 0, V.rows() - 1);
  std::sort(idx.data(), idx.data() + idx.size(),
            [&](Index i1, Index i2) { return V[i1] < V[i2]; });
  return idx;
}

}  // namespace xtp
}  // namespace votca