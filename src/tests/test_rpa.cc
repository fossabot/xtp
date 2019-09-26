/*
 * Copyright 2009-2019 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
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
#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE rpa_test
#include <boost/test/unit_test.hpp>
#include <complex>
#include <votca/xtp/aobasis.h>
#include <votca/xtp/aomatrix.h>
#include <votca/xtp/orbitals.h>
#include <votca/xtp/rpa.h>
#include <votca/xtp/threecenter.h>

using namespace votca::xtp;
using namespace std;

BOOST_AUTO_TEST_SUITE(rpa_test)

BOOST_AUTO_TEST_CASE(rpa_calcenergies) {
  TCMatrix_gwbse Mmn;
  Eigen::VectorXd eigenvals;
  votca::ctp::Logger log;
  RPA rpa(log, Mmn);
  rpa.configure(4, 0, 9);
  Eigen::VectorXd dftenergies = Eigen::VectorXd::Zero(10);
  dftenergies << -0.5, -0.4, -0.3, -0.2, -0.2, -0.1, 0, 0.1, 0.2, 0.3;
  Eigen::VectorXd gwenergies = Eigen::VectorXd::Zero(7);
  gwenergies << -0.15, -0.05, 0.05, 0.15, 0.45, 0.55, 0.65;
  int qpmin = 1;
  rpa.UpdateRPAInputEnergies(dftenergies, gwenergies, qpmin);
  Eigen::VectorXd rpaenergies = rpa.getRPAInputEnergies();
  Eigen::VectorXd rpaenergies_ref = Eigen::VectorXd::Zero(10);
  rpaenergies_ref << -0.5, -0.15, -0.05, 0.05, 0.15, 0.45, 0.55, 0.65, 0.4, 0.5;
  bool e_check = rpaenergies_ref.isApprox(rpaenergies, 0.0001);

  if (!e_check) {
    cout << "energy" << endl;
    cout << rpaenergies << endl;
    cout << "energy_ref" << endl;
    cout << rpaenergies_ref << endl;
  }
}

BOOST_AUTO_TEST_CASE(rpa_full) {

  ofstream xyzfile("molecule.xyz");
  xyzfile << " 5" << endl;
  xyzfile << " methane" << endl;
  xyzfile << " C            .000000     .000000     .000000" << endl;
  xyzfile << " H            .629118     .629118     .629118" << endl;
  xyzfile << " H           -.629118    -.629118     .629118" << endl;
  xyzfile << " H            .629118    -.629118    -.629118" << endl;
  xyzfile << " H           -.629118     .629118    -.629118" << endl;
  xyzfile.close();

  ofstream basisfile("3-21G.xml");
  basisfile << "<basis name=\"3-21G\">" << endl;
  basisfile << "  <element name=\"H\">" << endl;
  basisfile << "    <shell scale=\"1.0\" type=\"S\">" << endl;
  basisfile << "      <constant decay=\"5.447178e+00\">" << endl;
  basisfile << "        <contractions factor=\"1.562850e-01\" type=\"S\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "      <constant decay=\"8.245470e-01\">" << endl;
  basisfile << "        <contractions factor=\"9.046910e-01\" type=\"S\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "    </shell>" << endl;
  basisfile << "    <shell scale=\"1.0\" type=\"S\">" << endl;
  basisfile << "      <constant decay=\"1.831920e-01\">" << endl;
  basisfile << "        <contractions factor=\"1.000000e+00\" type=\"S\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "    </shell>" << endl;
  basisfile << "  </element>" << endl;
  basisfile << "  <element name=\"C\">" << endl;
  basisfile << "    <shell scale=\"1.0\" type=\"S\">" << endl;
  basisfile << "      <constant decay=\"1.722560e+02\">" << endl;
  basisfile << "        <contractions factor=\"6.176690e-02\" type=\"S\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "      <constant decay=\"2.591090e+01\">" << endl;
  basisfile << "        <contractions factor=\"3.587940e-01\" type=\"S\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "      <constant decay=\"5.533350e+00\">" << endl;
  basisfile << "        <contractions factor=\"7.007130e-01\" type=\"S\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "    </shell>" << endl;
  basisfile << "    <shell scale=\"1.0\" type=\"SP\">" << endl;
  basisfile << "      <constant decay=\"3.664980e+00\">" << endl;
  basisfile << "        <contractions factor=\"-3.958970e-01\" type=\"S\"/>"
            << endl;
  basisfile << "        <contractions factor=\"2.364600e-01\" type=\"P\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "      <constant decay=\"7.705450e-01\">" << endl;
  basisfile << "        <contractions factor=\"1.215840e+00\" type=\"S\"/>"
            << endl;
  basisfile << "        <contractions factor=\"8.606190e-01\" type=\"P\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "    </shell>" << endl;
  basisfile << "    <shell scale=\"1.0\" type=\"SP\">" << endl;
  basisfile << "      <constant decay=\"1.958570e-01\">" << endl;
  basisfile << "        <contractions factor=\"1.000000e+00\" type=\"S\"/>"
            << endl;
  basisfile << "        <contractions factor=\"1.000000e+00\" type=\"P\"/>"
            << endl;
  basisfile << "      </constant>" << endl;
  basisfile << "    </shell>" << endl;
  basisfile << "  </element>" << endl;
  basisfile << "</basis>" << endl;
  basisfile.close();

  Orbitals orbitals;
  orbitals.QMAtoms().LoadFromFile("molecule.xyz");
  BasisSet basis;
  basis.Load("3-21G.xml");

  AOBasis aobasis;
  aobasis.Fill(basis, orbitals.QMAtoms());

  Eigen::VectorXd eigenvals = Eigen::VectorXd::Zero(17);
  eigenvals << 0.0468207, 0.0907801, 0.0907801, 0.104563, 0.592491, 0.663355,
      0.663355, 0.768373, 1.69292, 1.97724, 1.97724, 2.50877, 2.98732, 3.4418,
      3.4418, 4.81084, 17.1838;

  Eigen::MatrixXd eigenvectors = Eigen::MatrixXd::Zero(17, 17);
  eigenvectors << 0.0185815, 2.9133e-17, 8.49354e-17, -0.00312916, 0.0420075,
      1.11356e-16, 1.85886e-17, -0.0334732, 0.0485113, -8.71556e-18,
      -3.79994e-17, -0.0346485, -0.0248392, -3.32286e-22, -4.62643e-17,
      0.0144472, 0.996183, 0.166534, 2.80578e-16, 7.85515e-16, -0.0299557,
      0.409156, 1.10045e-15, 1.67666e-16, -0.336895, 0.608646, -1.00343e-16,
      -5.04622e-16, -0.437568, -0.299751, -2.77682e-17, -6.06812e-16, 0.176594,
      -0.0866677, 0.0010572, -0.0210402, -0.0345975, 0.035778, 0.0611836,
      0.0374747, -0.154443, 0.0892921, 0.0842611, -0.35309, -0.0759572,
      0.278374, -0.409082, -0.64367, 0.308248, -0.261525, -0.000315534,
      -0.0010572, -0.0404824, 0.000922593, -0.035778, -0.0611836, -0.115015,
      -0.109676, -0.0892921, -0.0842611, -0.242326, 0.267806, -0.278374,
      0.409082, -0.0548848, 0.711558, 0.261525, 0.000315534, 0.0010572,
      -0.0194422, 0.0355201, 0.035778, 0.0611836, -0.152489, 0.0447677,
      0.0892921, 0.0842611, 0.110764, 0.343764, 0.278374, -0.409082, 0.588785,
      0.403311, -0.261525, -0.000315534, -0.823783, -9.8891e-16, -3.34692e-15,
      0.103497, -0.277613, -5.51463e-16, 4.95594e-17, 0.163544, 0.121215,
      -7.23985e-17, -2.63149e-16, -0.259891, -0.284396, -1.74149e-16,
      -6.65818e-16, 0.208987, 0.00782842, -0.0333718, 0.22696, 0.373203,
      -0.337332, -0.251625, -0.144131, 0.594004, -0.329076, -0.0456626, 0.18588,
      0.0399869, -0.0631275, -0.0704844, -0.231899, 0.111054, -0.189161,
      0.000129868, 0.0333718, 0.436683, -0.009952, 0.337332, 0.251625, 0.442357,
      0.421824, 0.329076, 0.0456626, 0.12757, -0.140984, 0.0631275, 0.0704844,
      -0.0197737, 0.256357, 0.189161, -0.000129868, -0.0333718, 0.209723,
      -0.383155, -0.337332, -0.251625, 0.586489, -0.172181, -0.329076,
      -0.0456626, -0.0583106, -0.180971, -0.0631275, -0.0704844, 0.212125,
      0.145303, -0.189161, 0.000129868, -0.00177478, 0.0553645, -0.00126176,
      -0.0164247, 0.23154, -0.262519, -0.250334, -0.0135392, -0.429472, 0.45567,
      -0.503583, -0.223493, -0.211802, -0.020461, 0.265268, 0.0023362,
      -0.00241145, 0.294363, -0.686239, 0.0156394, 0.204055, -0.360136,
      0.267096, 0.254698, 0.074687, -0.0228668, 0.132236, -0.14614, -0.174986,
      -0.185046, -0.0109958, 0.142556, 0.0661743, 0.0022999, -0.00177478,
      -0.0265895, 0.0485779, -0.0164247, 0.23154, 0.348055, -0.102182,
      -0.0135392, -0.429472, 0.208281, 0.646413, -0.223493, -0.211802,
      -0.219498, -0.150354, 0.0023362, -0.00241145, 0.294363, 0.329576,
      -0.60212, 0.204055, -0.360136, -0.354123, 0.103963, 0.074687, -0.0228668,
      0.0604434, 0.18759, -0.174986, -0.185046, -0.117959, -0.0808008,
      0.0661743, 0.0022999, -0.00177478, -0.028775, -0.0473162, -0.0164247,
      0.23154, -0.0855356, 0.352515, -0.0135392, -0.429472, -0.663951, -0.14283,
      -0.223493, -0.211802, 0.239959, -0.114914, 0.0023362, -0.00241145,
      0.294363, 0.356664, 0.586481, 0.204055, -0.360136, 0.0870267, -0.358661,
      0.074687, -0.0228668, -0.192679, -0.0414494, -0.174986, -0.185046,
      0.128955, -0.0617554, 0.0661743, 0.0022999, 0.00741062, -3.87173e-16,
      -4.31863e-16, 0.0468488, -0.0476991, 7.27357e-16, 1.23654e-15, -0.43422,
      -0.159247, -4.34945e-17, 1.2743e-16, 0.503528, -0.228856, -7.97629e-17,
      -2.53026e-16, 0.689669, -0.00301027, 0.173046, 7.91486e-15, 8.39419e-15,
      -0.717804, -0.0195249, -1.10754e-15, -1.66789e-15, 0.551371, 0.0684292,
      4.15572e-17, -1.84233e-16, 0.0105378, -0.148396, -1.63792e-16,
      -4.6499e-16, 0.351571, 0.00210309;

  TCMatrix_gwbse Mmn;
  Mmn.Initialize(aobasis.AOBasisSize(), 0, 16, 0, 16);
  Mmn.Fill(aobasis, aobasis, eigenvectors);

  votca::ctp::Logger log;
  RPA rpa(log, Mmn);
  rpa.configure(4, 0, 16);
  rpa.setRPAInputEnergies(eigenvals);
  Eigen::MatrixXd e_i = rpa.calculate_epsilon_i(0.5);

  Eigen::MatrixXd i_ref = Eigen::MatrixXd::Zero(17, 17);
  i_ref << 1.00063, -0.000738023, -0.000847951, 0.000847951, -0.000847951,
      -0.00398178, -0.00230038, 0.00230038, -0.00230038, -0.000509516,
      -0.00507621, -0.000509516, -0.00507621, -0.000509516, -0.00507621,
      -0.000383038, -0.00312618, -0.000738023, 1.00263, 0.00161324, -0.00161324,
      0.00161324, 0.00911444, 0.00415952, -0.00415952, 0.00415952, 0.000514703,
      0.00743779, 0.000514703, 0.00743779, 0.000514703, 0.00743779, 0.000660954,
      0.00451844, -0.000847951, 0.00161324, 1.00392, -0.00190885, 0.00190885,
      0.00907072, 0.0108943, -0.00558202, 0.00558202, 0.00178259, 0.0142578,
      0.00178259, 0.0142578, 0.00103807, 0.012368, 0.00152356, 0.00935261,
      0.000847951, -0.00161324, -0.00190885, 1.00392, -0.00190885, -0.00907072,
      -0.00558202, 0.0108943, -0.00558202, -0.00103807, -0.012368, -0.00178259,
      -0.0142578, -0.00178259, -0.0142578, -0.00152356, -0.00935261,
      -0.000847951, 0.00161324, 0.00190885, -0.00190885, 1.00392, 0.00907072,
      0.00558202, -0.00558202, 0.0108943, 0.00178259, 0.0142578, 0.00103807,
      0.012368, 0.00178259, 0.0142578, 0.00152356, 0.00935261, -0.00398178,
      0.00911444, 0.00907072, -0.00907072, 0.00907072, 1.06083, 0.0317799,
      -0.0317799, 0.0317799, 0.00979314, 0.0779294, 0.00979314, 0.0779294,
      0.00979314, 0.0779294, 0.00426666, 0.0435389, -0.00230038, 0.00415952,
      0.0108943, -0.00558202, 0.00558202, 0.0317799, 1.06312, -0.0152029,
      0.0152029, 0.0130855, 0.0740544, 0.0130855, 0.0740544, -0.00319113,
      0.0230598, 0.0056834, 0.0299224, 0.00230038, -0.00415952, -0.00558202,
      0.0108943, -0.00558202, -0.0317799, -0.0152029, 1.06312, -0.0152029,
      0.00319113, -0.0230598, -0.0130855, -0.0740544, -0.0130855, -0.0740544,
      -0.0056834, -0.0299224, -0.00230038, 0.00415952, 0.00558202, -0.00558202,
      0.0108943, 0.0317799, 0.0152029, -0.0152029, 1.06312, 0.0130855,
      0.0740544, -0.00319113, 0.0230598, 0.0130855, 0.0740544, 0.0056834,
      0.0299224, -0.000509516, 0.000514703, 0.00178259, -0.00103807, 0.00178259,
      0.00979314, 0.0130855, 0.00319113, 0.0130855, 1.01191, 0.0403377,
      0.000522925, 0.0147762, 0.000522925, 0.0147762, 0.0012644, 0.0119637,
      -0.00507621, 0.00743779, 0.0142578, -0.012368, 0.0142578, 0.0779294,
      0.0740544, -0.0230598, 0.0740544, 0.0403377, 1.19801, 0.0147762, 0.12575,
      0.0147762, 0.12575, 0.00894661, 0.0796677, -0.000509516, 0.000514703,
      0.00178259, -0.00178259, 0.00103807, 0.00979314, 0.0130855, -0.0130855,
      -0.00319113, 0.000522925, 0.0147762, 1.01191, 0.0403377, 0.000522925,
      0.0147762, 0.0012644, 0.0119637, -0.00507621, 0.00743779, 0.0142578,
      -0.0142578, 0.012368, 0.0779294, 0.0740544, -0.0740544, 0.0230598,
      0.0147762, 0.12575, 0.0403377, 1.19801, 0.0147762, 0.12575, 0.00894661,
      0.0796677, -0.000509516, 0.000514703, 0.00103807, -0.00178259, 0.00178259,
      0.00979314, -0.00319113, -0.0130855, 0.0130855, 0.000522925, 0.0147762,
      0.000522925, 0.0147762, 1.01191, 0.0403377, 0.0012644, 0.0119637,
      -0.00507621, 0.00743779, 0.012368, -0.0142578, 0.0142578, 0.0779294,
      0.0230598, -0.0740544, 0.0740544, 0.0147762, 0.12575, 0.0147762, 0.12575,
      0.0403377, 1.19801, 0.00894661, 0.0796677, -0.000383038, 0.000660954,
      0.00152356, -0.00152356, 0.00152356, 0.00426666, 0.0056834, -0.0056834,
      0.0056834, 0.0012644, 0.00894661, 0.0012644, 0.00894661, 0.0012644,
      0.00894661, 1.00208, 0.00789381, -0.00312618, 0.00451844, 0.00935261,
      -0.00935261, 0.00935261, 0.0435389, 0.0299224, -0.0299224, 0.0299224,
      0.0119637, 0.0796677, 0.0119637, 0.0796677, 0.0119637, 0.0796677,
      0.00789381, 1.06135;

  bool i_check = i_ref.isApprox(e_i, 0.0001);

  if (!i_check) {
    cout << "Epsilon_i" << endl;
    cout << e_i << endl;
    cout << "Epsilon_i_ref" << endl;
    cout << i_ref << endl;
  }
  BOOST_CHECK_EQUAL(i_check, 1);

  Eigen::MatrixXd e_r = rpa.calculate_epsilon_r(0.0);

  Eigen::MatrixXd r_ref = Eigen::MatrixXd::Zero(17, 17);
  r_ref << 1.0006587695, -0.00078799598102, -0.00088922577334, 0.00088922577334,
      -0.00088922577334, -0.0042169065833, -0.002336091509, 0.002336091509,
      -0.002336091509, -0.00050662218169, -0.0052057890458, -0.00050662218169,
      -0.0052057890458, -0.00050662218169, -0.0052057890458, -0.00037564339451,
      -0.0032058806556, -0.00078799598102, 1.0028668699, 0.0017910236524,
      -0.0017910236524, 0.0017910236524, 0.01014801061, 0.0051408879134,
      -0.0051408879134, 0.0051408879134, 0.00036877585664, 0.0073965347948,
      0.00036877585664, 0.0073965347948, 0.00036877585664, 0.0073965347948,
      0.00094029956345, 0.0045022793469, -0.00088922577334, 0.0017910236524,
      1.0067257854, -0.0015465163998, 0.0015465163998, 0.0094511460318,
      0.045585007947, 0.0041655130822, -0.0041655130822, 0.0034824005183,
      0.028928879976, 0.0034824005183, 0.028928879976, -0.003242803598,
      -0.014354267669, 0.0031137836848, 0.0099736516652, 0.00088922577334,
      -0.0017910236524, -0.0015465163998, 1.0067257854, -0.0015465163998,
      -0.0094511460318, 0.0041655130822, 0.045585007947, 0.0041655130822,
      0.003242803598, 0.014354267669, -0.0034824005183, -0.028928879976,
      -0.0034824005183, -0.028928879976, -0.0031137836848, -0.0099736516652,
      -0.00088922577334, 0.0017910236524, 0.0015465163998, -0.0015465163998,
      1.0067257854, 0.0094511460318, -0.0041655130822, 0.0041655130822,
      0.045585007947, 0.0034824005183, 0.028928879976, -0.003242803598,
      -0.014354267669, 0.0034824005183, 0.028928879976, 0.0031137836848,
      0.0099736516652, -0.0042169065833, 0.01014801061, 0.0094511460318,
      -0.0094511460318, 0.0094511460318, 1.0683943743, 0.033253778547,
      -0.033253778547, 0.033253778547, 0.0098928173624, 0.080323544914,
      0.0098928173624, 0.080323544914, 0.0098928173624, 0.080323544914,
      0.0042490675042, 0.044368325677, -0.002336091509, 0.0051408879134,
      0.045585007947, 0.0041655130822, -0.0041655130822, 0.033253778547,
      1.5522819346, 0.1630437813, -0.1630437813, 0.040951984157, 0.29148484041,
      0.040951984157, 0.29148484041, -0.065920061912, -0.39062789159,
      0.019262830405, 0.030434350374, 0.002336091509, -0.0051408879134,
      0.0041655130822, 0.045585007947, 0.0041655130822, -0.033253778547,
      0.1630437813, 1.5522819346, 0.1630437813, 0.065920061912, 0.39062789159,
      -0.040951984157, -0.29148484041, -0.040951984157, -0.29148484041,
      -0.019262830405, -0.030434350374, -0.002336091509, 0.0051408879134,
      -0.0041655130822, 0.0041655130822, 0.045585007947, 0.033253778547,
      -0.1630437813, 0.1630437813, 1.5522819346, 0.040951984157, 0.29148484041,
      -0.065920061912, -0.39062789159, 0.040951984157, 0.29148484041,
      0.019262830405, 0.030434350374, -0.00050662218169, 0.00036877585664,
      0.0034824005183, 0.003242803598, 0.0034824005183, 0.0098928173624,
      0.040951984157, 0.065920061912, 0.040951984157, 1.0212195523,
      0.09982358437, -0.0032454628838, -0.012437519198, -0.0032454628838,
      -0.012437519198, 0.00041150551217, 0.012585676779, -0.0052057890458,
      0.0073965347948, 0.028928879976, 0.014354267669, 0.028928879976,
      0.080323544914, 0.29148484041, 0.39062789159, 0.29148484041,
      0.09982358437, 1.6097216877, -0.012437519198, -0.062989343739,
      -0.012437519198, -0.062989343739, 0.010200010299, 0.082551786343,
      -0.00050662218169, 0.00036877585664, 0.0034824005183, -0.0034824005183,
      -0.003242803598, 0.0098928173624, 0.040951984157, -0.040951984157,
      -0.065920061912, -0.0032454628838, -0.012437519198, 1.0212195523,
      0.09982358437, -0.0032454628838, -0.012437519198, 0.00041150551217,
      0.012585676779, -0.0052057890458, 0.0073965347948, 0.028928879976,
      -0.028928879976, -0.014354267669, 0.080323544914, 0.29148484041,
      -0.29148484041, -0.39062789159, -0.012437519198, -0.062989343739,
      0.09982358437, 1.6097216877, -0.012437519198, -0.062989343739,
      0.010200010299, 0.082551786343, -0.00050662218169, 0.00036877585664,
      -0.003242803598, -0.0034824005183, 0.0034824005183, 0.0098928173624,
      -0.065920061912, -0.040951984157, 0.040951984157, -0.0032454628838,
      -0.012437519198, -0.0032454628838, -0.012437519198, 1.0212195523,
      0.09982358437, 0.00041150551217, 0.012585676779, -0.0052057890458,
      0.0073965347948, -0.014354267669, -0.028928879976, 0.028928879976,
      0.080323544914, -0.39062789159, -0.29148484041, 0.29148484041,
      -0.012437519198, -0.062989343739, -0.012437519198, -0.062989343739,
      0.09982358437, 1.6097216877, 0.010200010299, 0.082551786343,
      -0.00037564339451, 0.00094029956345, 0.0031137836848, -0.0031137836848,
      0.0031137836848, 0.0042490675042, 0.019262830405, -0.019262830405,
      0.019262830405, 0.00041150551217, 0.010200010299, 0.00041150551217,
      0.010200010299, 0.00041150551217, 0.010200010299, 1.0065147698,
      0.0087814549722, -0.0032058806556, 0.0045022793469, 0.0099736516652,
      -0.0099736516652, 0.0099736516652, 0.044368325677, 0.030434350374,
      -0.030434350374, 0.030434350374, 0.012585676779, 0.082551786343,
      0.012585676779, 0.082551786343, 0.012585676779, 0.082551786343,
      0.0087814549722, 1.0692401254;

  bool r_check = r_ref.isApprox(e_r, 0.0001);

  if (!r_check) {
    cout << "Epsilon_r" << endl;
    cout << e_r << endl;
    cout << "Epsilon_r_ref" << endl;
    cout << r_ref << endl;
  }

  BOOST_CHECK_EQUAL(r_check, 1);

  std::complex<double> frequency(0.5, 0.5);
  Eigen::MatrixXcd e_complex = rpa.calculate_epsilon(frequency);
  typedef std::complex<double> cd;
  Eigen::MatrixXcd complex_ref = Eigen::MatrixXcd::Zero(17, 17);
  complex_ref << cd(1.00064, 6.22009e-05), cd(-0.000754644, -9.4922e-05),
      cd(-0.000880948, -9.57553e-05), cd(0.000880948, 9.57553e-05),
      cd(-0.000880948, -9.57553e-05), cd(-0.00407452, -0.000422891),
      cd(-0.0023964, -0.00015584), cd(0.0023964, 0.000155839),
      cd(-0.0023964, -0.00015584), cd(-0.00053486, 3.89755e-06),
      cd(-0.00528947, -0.000306967), cd(-0.000534859, 3.89733e-06),
      cd(-0.00528947, -0.000306967), cd(-0.000534857, 3.89746e-06),
      cd(-0.00528947, -0.000306967), cd(-0.000403477, -1.15957e-05),
      cd(-0.00326868, -0.000186636), cd(-0.000754644, -9.4922e-05),
      cd(1.00264, 0.000347516), cd(0.00167102, 0.000159386),
      cd(-0.00167101, -0.000159385), cd(0.00167102, 0.000159385),
      cd(0.00896434, 0.00146488), cd(0.00427781, 0.000220039),
      cd(-0.00427779, -0.000220036), cd(0.00427781, 0.000220038),
      cd(0.000590949, -9.30416e-05), cd(0.00792882, 0.000144935),
      cd(0.000590956, -9.3041e-05), cd(0.00792883, 0.000144935),
      cd(0.000590958, -9.30417e-05), cd(0.00792883, 0.000144934),
      cd(0.000684423, -2.79913e-06), cd(0.00480771, 5.8691e-05),
      cd(-0.000880948, -9.57553e-05), cd(0.00167102, 0.000159386),
      cd(1.00386, 0.000549499), cd(-0.00195, -0.000214049),
      cd(0.00194999, 0.000214049), cd(0.00950008, 0.000745599),
      cd(0.0100514, 0.00144216), cd(-0.00553694, -0.000567146),
      cd(0.00553692, 0.000567144), cd(0.00185507, 0.00011391),
      cd(0.0145721, 0.00123153), cd(0.00185507, 0.00011391),
      cd(0.0145721, 0.00123153), cd(0.00119869, -7.82518e-06),
      cd(0.0132381, 0.00110212), cd(0.00137022, 0.000218999),
      cd(0.00954192, 0.0011733), cd(0.000880948, 9.57553e-05),
      cd(-0.00167101, -0.000159385), cd(-0.00195, -0.000214049),
      cd(1.00386, 0.000549499), cd(-0.00195, -0.000214049),
      cd(-0.00950006, -0.000745596), cd(-0.00553694, -0.000567147),
      cd(0.0100514, 0.00144216), cd(-0.00553694, -0.000567147),
      cd(-0.00119869, 7.82538e-06), cd(-0.0132381, -0.00110212),
      cd(-0.00185507, -0.00011391), cd(-0.0145721, -0.00123153),
      cd(-0.00185507, -0.00011391), cd(-0.0145721, -0.00123153),
      cd(-0.00137022, -0.000218999), cd(-0.00954191, -0.0011733),
      cd(-0.000880948, -9.57553e-05), cd(0.00167102, 0.000159385),
      cd(0.00194999, 0.000214049), cd(-0.00195, -0.000214049),
      cd(1.00386, 0.0005495), cd(0.00950007, 0.000745598),
      cd(0.00553693, 0.000567144), cd(-0.00553694, -0.000567147),
      cd(0.0100514, 0.00144216), cd(0.00185507, 0.00011391),
      cd(0.0145721, 0.00123153), cd(0.00119869, -7.82496e-06),
      cd(0.0132381, 0.00110212), cd(0.00185507, 0.00011391),
      cd(0.0145721, 0.00123153), cd(0.00137022, 0.000218999),
      cd(0.00954192, 0.0011733), cd(-0.00407452, -0.000422891),
      cd(0.00896434, 0.00146488), cd(0.00950008, 0.000745599),
      cd(-0.00950006, -0.000745596), cd(0.00950007, 0.000745598),
      cd(1.05997, 0.0112088), cd(0.0335553, 0.00278594),
      cd(-0.0335552, -0.00278592), cd(0.0335552, 0.00278594),
      cd(0.0104802, 0.000739174), cd(0.0823301, 0.00705406),
      cd(0.0104802, 0.000739177), cd(0.0823301, 0.00705405),
      cd(0.0104802, 0.000739175), cd(0.0823301, 0.00705405),
      cd(0.0047238, -7.11434e-05), cd(0.0463064, 0.00259636),
      cd(-0.0023964, -0.00015584), cd(0.00427781, 0.000220039),
      cd(0.0100514, 0.00144216), cd(-0.00553694, -0.000567147),
      cd(0.00553693, 0.000567144), cd(0.0335553, 0.00278594),
      cd(1.04866, 0.0171258), cd(-0.0157841, -0.000776446),
      cd(0.015784, 0.000776422), cd(0.0132131, 0.00229283),
      cd(0.0704439, 0.0139593), cd(0.0132131, 0.00229283),
      cd(0.0704439, 0.0139593), cd(-0.00173887, -0.0015594),
      cd(0.0339009, -0.00397269), cd(0.00426646, 0.00128118),
      cd(0.0317988, 0.00237234), cd(0.0023964, 0.000155839),
      cd(-0.00427779, -0.000220036), cd(-0.00553694, -0.000567146),
      cd(0.0100514, 0.00144216), cd(-0.00553694, -0.000567147),
      cd(-0.0335552, -0.00278592), cd(-0.0157841, -0.000776446),
      cd(1.04866, 0.0171259), cd(-0.0157841, -0.000776457),
      cd(0.00173887, 0.00155939), cd(-0.0339008, 0.00397267),
      cd(-0.0132131, -0.00229283), cd(-0.0704439, -0.0139593),
      cd(-0.0132131, -0.00229284), cd(-0.0704439, -0.0139593),
      cd(-0.00426646, -0.00128119), cd(-0.0317987, -0.00237233),
      cd(-0.0023964, -0.00015584), cd(0.00427781, 0.000220038),
      cd(0.00553692, 0.000567144), cd(-0.00553694, -0.000567147),
      cd(0.0100514, 0.00144216), cd(0.0335552, 0.00278594),
      cd(0.015784, 0.000776422), cd(-0.0157841, -0.000776457),
      cd(1.04866, 0.0171259), cd(0.0132131, 0.00229283),
      cd(0.0704438, 0.0139593), cd(-0.00173886, -0.00155939),
      cd(0.0339009, -0.00397267), cd(0.0132131, 0.00229284),
      cd(0.0704439, 0.0139593), cd(0.00426646, 0.00128119),
      cd(0.0317988, 0.00237234), cd(-0.00053486, 3.89755e-06),
      cd(0.000590949, -9.30416e-05), cd(0.00185507, 0.00011391),
      cd(-0.00119869, 7.82538e-06), cd(0.00185507, 0.00011391),
      cd(0.0104802, 0.000739174), cd(0.0132131, 0.00229283),
      cd(0.00173887, 0.00155939), cd(0.0132131, 0.00229283),
      cd(1.01216, 0.00228087), cd(0.0403491, 0.00747047),
      cd(0.000558721, -3.17827e-05), cd(0.0158425, 0.00154377),
      cd(0.00055872, -3.17831e-05), cd(0.0158425, 0.00154377),
      cd(0.00143534, 0.000135555), cd(0.0125666, 0.00142675),
      cd(-0.00528947, -0.000306967), cd(0.00792882, 0.000144935),
      cd(0.0145721, 0.00123153), cd(-0.0132381, -0.00110212),
      cd(0.0145721, 0.00123153), cd(0.0823301, 0.00705406),
      cd(0.0704439, 0.0139593), cd(-0.0339008, 0.00397267),
      cd(0.0704438, 0.0139593), cd(0.0403491, 0.00747047),
      cd(1.19241, 0.037313), cd(0.0158425, 0.00154377), cd(0.135066, 0.0127038),
      cd(0.0158425, 0.00154377), cd(0.135066, 0.0127038),
      cd(0.00930355, 0.00094112), cd(0.0841409, 0.00765577),
      cd(-0.000534859, 3.89733e-06), cd(0.000590956, -9.3041e-05),
      cd(0.00185507, 0.00011391), cd(-0.00185507, -0.00011391),
      cd(0.00119869, -7.82496e-06), cd(0.0104802, 0.000739177),
      cd(0.0132131, 0.00229283), cd(-0.0132131, -0.00229283),
      cd(-0.00173886, -0.00155939), cd(0.000558721, -3.17827e-05),
      cd(0.0158425, 0.00154377), cd(1.01216, 0.00228087),
      cd(0.0403491, 0.00747047), cd(0.000558727, -3.17828e-05),
      cd(0.0158425, 0.00154377), cd(0.00143534, 0.000135555),
      cd(0.0125666, 0.00142675), cd(-0.00528947, -0.000306967),
      cd(0.00792883, 0.000144935), cd(0.0145721, 0.00123153),
      cd(-0.0145721, -0.00123153), cd(0.0132381, 0.00110212),
      cd(0.0823301, 0.00705405), cd(0.0704439, 0.0139593),
      cd(-0.0704439, -0.0139593), cd(0.0339009, -0.00397267),
      cd(0.0158425, 0.00154377), cd(0.135066, 0.0127038),
      cd(0.0403491, 0.00747047), cd(1.19241, 0.037313),
      cd(0.0158425, 0.00154377), cd(0.135066, 0.0127038),
      cd(0.00930355, 0.00094112), cd(0.0841409, 0.00765576),
      cd(-0.000534857, 3.89746e-06), cd(0.000590958, -9.30417e-05),
      cd(0.00119869, -7.82518e-06), cd(-0.00185507, -0.00011391),
      cd(0.00185507, 0.00011391), cd(0.0104802, 0.000739175),
      cd(-0.00173887, -0.0015594), cd(-0.0132131, -0.00229284),
      cd(0.0132131, 0.00229284), cd(0.00055872, -3.17831e-05),
      cd(0.0158425, 0.00154377), cd(0.000558727, -3.17828e-05),
      cd(0.0158425, 0.00154377), cd(1.01216, 0.00228088),
      cd(0.0403492, 0.00747048), cd(0.00143534, 0.000135555),
      cd(0.0125666, 0.00142675), cd(-0.00528947, -0.000306967),
      cd(0.00792883, 0.000144934), cd(0.0132381, 0.00110212),
      cd(-0.0145721, -0.00123153), cd(0.0145721, 0.00123153),
      cd(0.0823301, 0.00705405), cd(0.0339009, -0.00397269),
      cd(-0.0704439, -0.0139593), cd(0.0704439, 0.0139593),
      cd(0.0158425, 0.00154377), cd(0.135066, 0.0127038),
      cd(0.0158425, 0.00154377), cd(0.135066, 0.0127038),
      cd(0.0403492, 0.00747048), cd(1.19241, 0.0373131),
      cd(0.00930355, 0.000941122), cd(0.0841409, 0.00765576),
      cd(-0.000403477, -1.15957e-05), cd(0.000684423, -2.79913e-06),
      cd(0.00137022, 0.000218999), cd(-0.00137022, -0.000218999),
      cd(0.00137022, 0.000218999), cd(0.0047238, -7.11434e-05),
      cd(0.00426646, 0.00128118), cd(-0.00426646, -0.00128119),
      cd(0.00426646, 0.00128119), cd(0.00143534, 0.000135555),
      cd(0.00930355, 0.00094112), cd(0.00143534, 0.000135555),
      cd(0.00930355, 0.00094112), cd(0.00143534, 0.000135555),
      cd(0.00930355, 0.000941122), cd(1.00155, 0.000575121),
      cd(0.00778577, 0.00155258), cd(-0.00326868, -0.000186636),
      cd(0.00480771, 5.8691e-05), cd(0.00954192, 0.0011733),
      cd(-0.00954191, -0.0011733), cd(0.00954192, 0.0011733),
      cd(0.0463064, 0.00259636), cd(0.0317988, 0.00237234),
      cd(-0.0317987, -0.00237233), cd(0.0317988, 0.00237234),
      cd(0.0125666, 0.00142675), cd(0.0841409, 0.00765577),
      cd(0.0125666, 0.00142675), cd(0.0841409, 0.00765576),
      cd(0.0125666, 0.00142675), cd(0.0841409, 0.00765576),
      cd(0.00778577, 0.00155258), cd(1.06059, 0.0128387);
  bool complex_check = complex_ref.isApprox(e_complex, 0.0001);

  if (!complex_check) {
    cout << "Epsilon_complex" << endl;
    cout << e_complex << endl;
    cout << "Epsilon_complex_ref" << endl;
    cout << complex_ref << endl;
  }

  BOOST_CHECK_EQUAL(complex_check, 1);
}

BOOST_AUTO_TEST_SUITE_END()
