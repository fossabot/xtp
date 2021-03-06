/*
 *            Copyright 2009-2020 The VOTCA Development Team
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
#ifndef VOTCA_XTP_SPECTRUM_PRIVATE_H
#define VOTCA_XTP_SPECTRUM_PRIVATE_H

// Standard includes
#include <cstdio>

// Local VOTCA includes
#include "votca/xtp/logger.h"
#include "votca/xtp/qmstate.h"
#include "votca/xtp/qmtool.h"

namespace votca {
namespace xtp {
class Orbitals;

class Spectrum : public QMTool {
 public:
  Spectrum() = default;

  ~Spectrum() override = default;

  std::string Identify() override { return "spectrum"; }

  void Initialize(const tools::Property& user_options) override;
  bool Evaluate() override;

 private:
  std::string _orbfile;
  std::string _output_file = "spectrum.dat";

  Logger _log;

  void CheckContent(const Orbitals& _orbitals);

  double evtonm(double eV);
  double evtoinvcm(double eV);
  double nmtoinvcm(double nm);
  double invcmtonm(double invcm);
  double nmtoev(double nm);

  double _lower = 0.0;  // in eV
  double _upper;        // in eV
  Index _n_pt = 100;

  Index _minexc = 0;
  Index _maxexc = std::numeric_limits<Index>::max();

  double _fwhm;  // in eV
  double _shiftby = 0.0;

  std::string _spectrum_type = "energy";
  // lineshape functions
  double Gaussian(double x, double center, double fwhm);
  double Lorentzian(double x, double center, double fwhm);
};

}  // namespace xtp
}  // namespace votca

#endif  // VOTCA_XTP_SPECTRUM_PRIVATE_H
