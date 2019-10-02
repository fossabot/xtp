/*
 *            Copyright 2009-2019 The VOTCA Development Team
 *                       (http://www.votca.org)
 *
 *      Licensed under the Apache License, Version 2.0 (the "License")
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once
#ifndef _VOTCA_XTP_CARTESIAN_COORD
#define _VOTCA_XTP_CARTESIAN_COORD

#include <votca/xtp/coordbase.h>
#include <votca/xtp/eigen.h>
#include <votca/xtp/orbitals.h>
#include <votca/xtp/qmatom.h>

namespace votca {
namespace xtp {

class CartesianCoords : public CoordBase {
 public:
  CartesianCoords(const QMMolecule& system);
};

}  // namespace xtp
}  // namespace votca
#endif  // _VOTCA_XTP_CARTESIAN_COORD
