///*
// *            Copyright 2009-2019 The VOTCA Development Team
// *                       (http://www.votca.org)
// *
// *      Licensed under the Apache License, Version 2.0 (the "License")
// *
// * You may not use this file except in compliance with the License.
// * You may obtain a copy of the License at
// *
// *              http://www.apache.org/licenses/LICENSE-2.0
// *
// * Unless required by applicable law or agreed to in writing, software
// * distributed under the License is distributed on an "AS IS" BASIS,
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// * See the License for the specific language governing permissions and
// * limitations under the License.
// */

#pragma once
#ifndef VOTCA_XTP_STERNHEIMERW_H
#define VOTCA_XTP_STERNHEIMERW_H

#include <fstream>
#include <votca/tools/property.h>
#include <votca/xtp/aobasis.h>
#include <votca/xtp/logger.h>
#include <votca/xtp/multishift.h>
#include <votca/xtp/orbitals.h>
#include <votca/xtp/padeapprox.h>
 
namespace votca {
namespace xtp {

class SternheimerW {
 public:
  SternheimerW(Orbitals& orbitals, Logger& log)
      : _orbitals(orbitals), _log(log){};

  void Initialize();

  std::vector<Eigen::MatrixXcd> DeltaNOS(std::complex<double> w,
                                         std::string gridtype);

  void printGreensfunction(double omega_start, double omega_end, int steps, double imaginary_shift,
    double lorentzian_broadening, int resolution_output);

  void printDielectricFunction(double omega_start, double omega_end, int steps, double imaginary_shift,
    double lorentzian_broadening, int resolution_output, std::string gridtype);

  std::complex<double> SelfEnergy(
      double omega_start, double omega_end, int steps, double imaginary_shift,
    double lorentzian_broadening, int resolution_output, std::string gridtype);

  
 private:
  Logger& _log;

  Orbitals& _orbitals;

  PadeApprox _pade;

  Multishift _multishift;

  int _num_occ_lvls;

  int _basis_size;

  Eigen::MatrixXcd _Hamiltonian_Matrix;

  Eigen::MatrixXcd _overlap_Matrix;
  
  Eigen::MatrixXcd _inverse_overlap;

  Eigen::MatrixXcd _density_Matrix;

  Eigen::MatrixXd _mo_coefficients;
  Eigen::VectorXd _mo_energies;

  std::vector<std::complex<double>> BuildGrid(double omega_start,
                                              double omega_end, int steps,
                                              double imaginary_shift) const;


  void initializeMultishift(int size);

  void initializePade(int size);
  
  // returns the overlap matrix for all occupied states
  Eigen::MatrixXd OverlapMatrix() const;
  // returns the density matrix for all occupied states
  Eigen::MatrixXd DensityMatrix() const;
  // returns the hamiltonian matrix for all occupied states
  Eigen::MatrixXd Hamiltonian() const;
  // Calculates coulomb matrix
  Eigen::MatrixXcd CoulombMatrix(Eigen::Vector3d gridpoint) const;
  // returns the expansion of the (screened) Coulomb interaction operator
  Eigen::MatrixXcd CalculateDeltaVExpansion(Eigen::MatrixXcd deltaV);
  // sets up the left hand side of the sternheimer equation
  Eigen::MatrixXcd SternheimerLHS(const Eigen::MatrixXcd& hamiltonian,
                                  const Eigen::MatrixXcd& inverse_overlap, double eps,
                                  std::complex<double> w, bool pm) const;
  // sets up the right hand side of the sternheimer equation
  Eigen::VectorXcd SternheimerRHS(const Eigen::MatrixXcd& overlap,
                                  const Eigen::MatrixXcd& density,
                                  const Eigen::MatrixXcd& pertubation,
                                  const Eigen::VectorXcd& coeff) const;
  // solves the sternheimer equation via the Biconjugate gradient method
  Eigen::VectorXcd SternheimerSolve(const Eigen::MatrixXcd& LHS,
                                    const Eigen::VectorXcd& RHS);

  Eigen::MatrixXcd DeltaNOneShot(std::complex<double> w,
                                           Eigen::Vector3d r)const;
  
  Eigen::VectorXd EvaluateBasisAtPosition(const AOBasis& dftbasis,
                                                 const Eigen::Vector3d& pos)const;
  Eigen::MatrixXcd DielectricMatrix(Eigen::MatrixXcd deltaN)const;
  std::complex<double> ScreenedCoulomb(Eigen::Vector3d gridpoint1, Eigen::Vector3d gridpoint2, std::complex<double> frequency)const;

  Eigen::MatrixXcd GreensFunctionLHS(std::complex<double> w) const;
  Eigen::MatrixXcd GreensFunctionRHS(Eigen::Vector3d r) const;

  Eigen::MatrixXcd AnalyticGreensfunction(std::complex<double> w) const;

  double Lorentzian(double r, std::complex<double> freq)const;

  Eigen::MatrixXcd NonAnalyticGreensfunction(std::complex<double> freq)const;

  std::complex<double> GreensFunction(Eigen::Vector3d gridpoint1, Eigen::Vector3d gridpoint2, std::complex<double> frequency)const;

};
}  // namespace xtp
}  // namespace votca
#endif //VOTCA_VOTCA-XTP_