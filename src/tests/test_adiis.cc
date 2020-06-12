/*
 * Copyright 2009-2020 The VOTCA Development Team (http://www.votca.org)
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

#define BOOST_TEST_MODULE adiis_test

// Standard includes
#include <iostream>

// Third party includes
#include <boost/test/unit_test.hpp>

// Local VOTCA includes
#include "votca/xtp/adiis.h"

using namespace votca::xtp;

BOOST_AUTO_TEST_SUITE(adiis_test)

BOOST_AUTO_TEST_CASE(coeffs_test) {
  std::vector<Eigen::MatrixXd> _dmatHist;
  std::vector<Eigen::MatrixXd> _mathist;

  ADIIS adiis;

  _dmatHist.push_back(Eigen::MatrixXd::Zero(17, 17));
  _dmatHist.back() << 0.00157507236, 0.0337453821, 4.48904516e-16,
      -5.93152213e-16, 7.87132607e-17, 0.0308759804, 2.51254428e-16,
      -1.49094084e-16, 5.77899242e-17, 0.00415998114, -0.00445631597,
      0.00415998114, -0.00445631597, 0.00415998114, -0.00445631597,
      0.00415998114, -0.00445631597, 0.0337453821, 0.722983175, 2.6642664e-15,
      -4.44783339e-15, 3.45846178e-16, 0.661507233, 4.39853895e-15,
      -2.02475102e-15, 1.04832445e-15, 0.0891261613, -0.0954750329,
      0.0891261613, -0.0954750329, 0.0891261613, -0.0954750329, 0.0891261613,
      -0.0954750329, 4.48904516e-16, 2.6642664e-15, 1.5219863, 2.88657986e-15,
      2.09034179e-15, -7.942125e-15, 0.215492472, 2.87270208e-15,
      -1.40512602e-15, 0.141933218, -0.0402359119, 0.141933218, -0.0402359119,
      -0.141933218, 0.0402359119, -0.141933218, 0.0402359119, -5.93152213e-16,
      -4.44783339e-15, 2.88657986e-15, 1.5219863, -2.31759056e-15,
      9.21105467e-15, -2.22044605e-15, 0.215492472, 1.62630326e-15, 0.141933218,
      -0.0402359119, -0.141933218, 0.0402359119, -0.141933218, 0.0402359119,
      0.141933218, -0.0402359119, 7.87132607e-17, 3.45846178e-16,
      2.09034179e-15, -2.31759056e-15, 1.5219863, 2.98901767e-15,
      -2.04957579e-15, 4.79737777e-15, 0.215492472, 0.141933218, -0.0402359119,
      -0.141933218, 0.0402359119, 0.141933218, -0.0402359119, -0.141933218,
      0.0402359119, 0.0308759804, 0.661507233, -7.942125e-15, 9.21105467e-15,
      2.98901767e-15, 0.605258649, 2.55488257e-15, 2.77790162e-17,
      1.33758513e-15, 0.0815476797, -0.0873567007, 0.0815476797, -0.0873567007,
      0.0815476797, -0.0873567007, 0.0815476797, -0.0873567007, 2.51254428e-16,
      4.39853895e-15, 0.215492472, -2.22044605e-15, -2.04957579e-15,
      2.55488257e-15, 0.0305107908, 3.2959746e-17, -5.2903645e-16, 0.0200958051,
      -0.00569685557, 0.0200958051, -0.00569685557, -0.0200958051,
      0.00569685557, -0.0200958051, 0.00569685557, -1.49094084e-16,
      -2.02475102e-15, 2.87270208e-15, 0.215492472, 4.79737777e-15,
      2.77790162e-17, 3.2959746e-17, 0.0305107908, 9.55941055e-16, 0.0200958051,
      -0.00569685557, -0.0200958051, 0.00569685557, -0.0200958051,
      0.00569685557, 0.0200958051, -0.00569685557, 5.77899242e-17,
      1.04832445e-15, -1.40512602e-15, 1.62630326e-15, 0.215492472,
      1.33758513e-15, -5.2903645e-16, 9.55941055e-16, 0.0305107908,
      0.0200958051, -0.00569685557, -0.0200958051, 0.00569685557, 0.0200958051,
      -0.00569685557, -0.0200958051, 0.00569685557, 0.00415998114, 0.0891261613,
      0.141933218, 0.141933218, 0.141933218, 0.0815476797, 0.0200958051,
      0.0200958051, 0.0200958051, 0.0506951329, -0.0230263694, -0.00224894011,
      -0.00801752871, -0.00224894011, -0.00801752871, -0.00224894011,
      -0.00801752871, -0.00445631597, -0.0954750329, -0.0402359119,
      -0.0402359119, -0.0402359119, -0.0873567007, -0.00569685557,
      -0.00569685557, -0.00569685557, -0.0230263694, 0.015799236,
      -0.00801752871, 0.0115444576, -0.00801752871, 0.0115444576,
      -0.00801752871, 0.0115444576, 0.00415998114, 0.0891261613, 0.141933218,
      -0.141933218, -0.141933218, 0.0815476797, 0.0200958051, -0.0200958051,
      -0.0200958051, -0.00224894011, -0.00801752871, 0.0506951329,
      -0.0230263694, -0.00224894011, -0.00801752871, -0.00224894011,
      -0.00801752871, -0.00445631597, -0.0954750329, -0.0402359119,
      0.0402359119, 0.0402359119, -0.0873567007, -0.00569685557, 0.00569685557,
      0.00569685557, -0.00801752871, 0.0115444576, -0.0230263694, 0.015799236,
      -0.00801752871, 0.0115444576, -0.00801752871, 0.0115444576, 0.00415998114,
      0.0891261613, -0.141933218, -0.141933218, 0.141933218, 0.0815476797,
      -0.0200958051, -0.0200958051, 0.0200958051, -0.00224894011,
      -0.00801752871, -0.00224894011, -0.00801752871, 0.0506951329,
      -0.0230263694, -0.00224894011, -0.00801752871, -0.00445631597,
      -0.0954750329, 0.0402359119, 0.0402359119, -0.0402359119, -0.0873567007,
      0.00569685557, 0.00569685557, -0.00569685557, -0.00801752871,
      0.0115444576, -0.00801752871, 0.0115444576, -0.0230263694, 0.015799236,
      -0.00801752871, 0.0115444576, 0.00415998114, 0.0891261613, -0.141933218,
      0.141933218, -0.141933218, 0.0815476797, -0.0200958051, 0.0200958051,
      -0.0200958051, -0.00224894011, -0.00801752871, -0.00224894011,
      -0.00801752871, -0.00224894011, -0.00801752871, 0.0506951329,
      -0.0230263694, -0.00445631597, -0.0954750329, 0.0402359119, -0.0402359119,
      0.0402359119, -0.0873567007, 0.00569685557, -0.00569685557, 0.00569685557,
      -0.00801752871, 0.0115444576, -0.00801752871, 0.0115444576,
      -0.00801752871, 0.0115444576, -0.0230263694, 0.015799236;

  _dmatHist.push_back(Eigen::MatrixXd::Zero(17, 17));
  _dmatHist.back() << 0.00113067308, 0.0234807013, 2.75153357e-15,
      2.20623526e-15, 2.55343929e-15, 0.0242194843, -1.14836338e-14,
      -1.19349082e-14, -1.16818695e-14, 0.00233470139, -0.00503225244,
      0.00233470139, -0.00503225244, 0.00233470139, -0.00503225244,
      0.00233470139, -0.00503225244, 0.0234807013, 0.506795831, 1.96929344e-14,
      1.47658637e-14, 1.82565083e-14, 0.449981495, -1.47659218e-13,
      -1.58085765e-13, -1.53118968e-13, 0.0652840601, -0.0572374845,
      0.0652840601, -0.0572374845, 0.0652840601, -0.0572374845, 0.0652840601,
      -0.0572374845, 2.75153357e-15, 1.96929344e-14, 1.06570509, 8.25406367e-15,
      7.13839524e-15, -1.35007168e-13, 0.148654077, -3.17926675e-14,
      -3.34484393e-14, 0.101236262, -0.0202701873, 0.101236262, -0.0202701873,
      -0.101236262, 0.0202701873, -0.101236262, 0.0202701873, 2.20623526e-15,
      1.47658637e-14, 8.25406367e-15, 1.06570509, 4.63958841e-15,
      -1.21838502e-13, -3.61166392e-14, 0.148654077, -3.27138056e-14,
      0.101236262, -0.0202701873, -0.101236262, 0.0202701873, -0.101236262,
      0.0202701873, 0.101236262, -0.0202701873, 2.55343929e-15, 1.82565083e-14,
      7.13839524e-15, 4.63958841e-15, 1.06570509, -1.27947338e-13,
      -3.37586513e-14, -3.05460952e-14, 0.148654077, 0.101236262, -0.0202701873,
      -0.101236262, 0.0202701873, 0.101236262, -0.0202701873, -0.101236262,
      0.0202701873, 0.0242194843, 0.449981495, -1.35007168e-13, -1.21838502e-13,
      -1.27947338e-13, 0.665224564, 1.39489732e-12, 1.48799728e-12,
      1.43172697e-12, 0.00358237918, -0.238424879, 0.00358237918, -0.238424879,
      0.00358237918, -0.238424879, 0.00358237918, -0.238424879, -1.14836338e-14,
      -1.47659218e-13, 0.148654077, -3.61166392e-14, -3.37586513e-14,
      1.39489732e-12, 0.0366081166, 1.73861099e-13, 1.75622228e-13,
      0.00095821862, -0.0589496747, 0.00095821862, -0.0589496747,
      -0.00095821862, 0.0589496747, -0.00095821862, 0.0589496747,
      -1.19349082e-14, -1.58085765e-13, -3.17926675e-14, 0.148654077,
      -3.05460952e-14, 1.48799728e-12, 1.73861099e-13, 0.0366081166,
      1.688846e-13, 0.00095821862, -0.0589496747, -0.00095821862, 0.0589496747,
      -0.00095821862, 0.0589496747, 0.00095821862, -0.0589496747,
      -1.16818695e-14, -1.53118968e-13, -3.34484393e-14, -3.27138056e-14,
      0.148654077, 1.43172697e-12, 1.75622228e-13, 1.688846e-13, 0.0366081166,
      0.00095821862, -0.0589496747, -0.00095821862, 0.0589496747, 0.00095821862,
      -0.0589496747, -0.00095821862, 0.0589496747, 0.00233470139, 0.0652840601,
      0.101236262, 0.101236262, 0.101236262, 0.00358237918, 0.00095821862,
      0.00095821862, 0.00095821862, 0.0811405658, 0.164877199, -0.000991884457,
      -0.0135897555, -0.000991884457, -0.0135897555, -0.000991884457,
      -0.0135897555, -0.00503225244, -0.0572374845, -0.0202701873,
      -0.0202701873, -0.0202701873, -0.238424879, -0.0589496747, -0.0589496747,
      -0.0589496747, 0.164877199, 0.735401758, -0.0135897555, -0.0598903608,
      -0.0135897555, -0.0598903608, -0.0135897555, -0.0598903608, 0.00233470139,
      0.0652840601, 0.101236262, -0.101236262, -0.101236262, 0.00358237918,
      0.00095821862, -0.00095821862, -0.00095821862, -0.000991884457,
      -0.0135897555, 0.0811405658, 0.164877199, -0.000991884457, -0.0135897555,
      -0.000991884457, -0.0135897555, -0.00503225244, -0.0572374845,
      -0.0202701873, 0.0202701873, 0.0202701873, -0.238424879, -0.0589496747,
      0.0589496747, 0.0589496747, -0.0135897555, -0.0598903608, 0.164877199,
      0.735401758, -0.0135897555, -0.0598903608, -0.0135897555, -0.0598903608,
      0.00233470139, 0.0652840601, -0.101236262, -0.101236262, 0.101236262,
      0.00358237918, -0.00095821862, -0.00095821862, 0.00095821862,
      -0.000991884457, -0.0135897555, -0.000991884457, -0.0135897555,
      0.0811405658, 0.164877199, -0.000991884457, -0.0135897555, -0.00503225244,
      -0.0572374845, 0.0202701873, 0.0202701873, -0.0202701873, -0.238424879,
      0.0589496747, 0.0589496747, -0.0589496747, -0.0135897555, -0.0598903608,
      -0.0135897555, -0.0598903608, 0.164877199, 0.735401758, -0.0135897555,
      -0.0598903608, 0.00233470139, 0.0652840601, -0.101236262, 0.101236262,
      -0.101236262, 0.00358237918, -0.00095821862, 0.00095821862,
      -0.00095821862, -0.000991884457, -0.0135897555, -0.000991884457,
      -0.0135897555, -0.000991884457, -0.0135897555, 0.0811405658, 0.164877199,
      -0.00503225244, -0.0572374845, 0.0202701873, -0.0202701873, 0.0202701873,
      -0.238424879, 0.0589496747, -0.0589496747, 0.0589496747, -0.0135897555,
      -0.0598903608, -0.0135897555, -0.0598903608, -0.0135897555, -0.0598903608,
      0.164877199, 0.735401758;
  _dmatHist.push_back(Eigen::MatrixXd::Zero(17, 17));
  _dmatHist.back() << 0.000793901464, 0.0119355753, -1.40268798e-14,
      -1.33736083e-14, -1.38284636e-14, 0.0131916418, -1.90038465e-14,
      -1.7115047e-14, -1.83111368e-14, -0.000639170975, -0.00567797114,
      -0.000639170975, -0.00567797114, -0.000639170975, -0.00567797114,
      -0.000639170975, -0.00567797114, 0.0119355753, 0.288311753,
      4.43648439e-14, 4.18260894e-14, 4.49262449e-14, 0.251249645,
      -4.31548671e-14, -5.88017665e-14, -4.92704812e-14, 0.0488884146,
      -0.0128520365, 0.0488884146, -0.0128520365, 0.0488884146, -0.0128520365,
      0.0488884146, -0.0128520365, -1.40268798e-14, 4.43648439e-14, 0.609528476,
      2.43268911e-13, 2.3613892e-13, 6.89095958e-13, 0.0942236916,
      -2.95492904e-13, -3.04053598e-13, 0.0783697835, 0.0355432545,
      0.0783697835, 0.0355432545, -0.0783697835, -0.0355432545, -0.0783697835,
      -0.0355432545, -1.33736083e-14, 4.18260894e-14, 2.43268911e-13,
      0.609528476, 2.40424903e-13, 6.58378116e-13, -3.05059296e-13,
      0.0942236916, -3.01807142e-13, 0.0783697835, 0.0355432545, -0.0783697835,
      -0.0355432545, -0.0783697835, -0.0355432545, 0.0783697835, 0.0355432545,
      -1.38284636e-14, 4.49262449e-14, 2.3613892e-13, 2.40424903e-13,
      0.609528476, 6.82646824e-13, -3.03093482e-13, -3.00641592e-13,
      0.0942236916, 0.0783697835, 0.0355432545, -0.0783697835, -0.0355432545,
      0.0783697835, 0.0355432545, -0.0783697835, -0.0355432545, 0.0131916418,
      0.251249645, 6.89095958e-13, 6.58378116e-13, 6.82646824e-13, 0.36941951,
      1.26872449e-12, 1.29369708e-12, 1.28160653e-12, 0.00483257769,
      -0.128030592, 0.00483257768, -0.128030592, 0.00483257768, -0.128030592,
      0.00483257768, -0.128030592, -1.90038465e-14, -4.31548671e-14,
      0.0942236916, -3.05059296e-13, -3.03093482e-13, 1.26872449e-12,
      0.0278620861, -3.63843298e-13, -3.64665746e-13, 0.014801376,
      -0.00263635698, 0.014801376, -0.00263635698, -0.014801376, 0.00263635698,
      -0.014801376, 0.00263635698, -1.7115047e-14, -5.88017665e-14,
      -2.95492904e-13, 0.0942236916, -3.00641592e-13, 1.29369708e-12,
      -3.63843298e-13, 0.0278620861, -3.69133933e-13, 0.014801376,
      -0.00263635698, -0.014801376, 0.00263635698, -0.014801376, 0.00263635698,
      0.014801376, -0.00263635698, -1.83111368e-14, -4.92704812e-14,
      -3.04053598e-13, -3.01807142e-13, 0.0942236916, 1.28160653e-12,
      -3.64665746e-13, -3.69133933e-13, 0.0278620861, 0.014801376,
      -0.00263635698, -0.014801376, 0.00263635698, 0.014801376, -0.00263635698,
      -0.014801376, 0.00263635698, -0.000639170975, 0.0488884146, 0.0783697835,
      0.0783697835, 0.0783697835, 0.00483257769, 0.014801376, 0.014801376,
      0.014801376, 0.148361157, 0.295418249, -0.00505338103, -0.0308437927,
      -0.00505338103, -0.0308437927, -0.00505338103, -0.0308437927,
      -0.00567797114, -0.0128520365, 0.0355432545, 0.0355432545, 0.0355432545,
      -0.128030592, -0.00263635698, -0.00263635698, -0.00263635698, 0.295418249,
      0.817121133, -0.0308437927, -0.102944558, -0.0308437927, -0.102944558,
      -0.0308437927, -0.102944558, -0.000639170975, 0.0488884146, 0.0783697835,
      -0.0783697835, -0.0783697835, 0.00483257768, 0.014801376, -0.014801376,
      -0.014801376, -0.00505338103, -0.0308437927, 0.148361157, 0.295418249,
      -0.00505338103, -0.0308437927, -0.00505338103, -0.0308437927,
      -0.00567797114, -0.0128520365, 0.0355432545, -0.0355432545, -0.0355432545,
      -0.128030592, -0.00263635698, 0.00263635698, 0.00263635698, -0.0308437927,
      -0.102944558, 0.295418249, 0.817121133, -0.0308437927, -0.102944558,
      -0.0308437927, -0.102944558, -0.000639170975, 0.0488884146, -0.0783697835,
      -0.0783697835, 0.0783697835, 0.00483257768, -0.014801376, -0.014801376,
      0.014801376, -0.00505338103, -0.0308437927, -0.00505338103, -0.0308437927,
      0.148361157, 0.295418249, -0.00505338103, -0.0308437927, -0.00567797114,
      -0.0128520365, -0.0355432545, -0.0355432545, 0.0355432545, -0.128030592,
      0.00263635698, 0.00263635698, -0.00263635698, -0.0308437927, -0.102944558,
      -0.0308437927, -0.102944558, 0.295418249, 0.817121133, -0.0308437927,
      -0.102944558, -0.000639170975, 0.0488884146, -0.0783697835, 0.0783697835,
      -0.0783697835, 0.00483257768, -0.014801376, 0.014801376, -0.014801376,
      -0.00505338103, -0.0308437927, -0.00505338103, -0.0308437927,
      -0.00505338103, -0.0308437927, 0.148361157, 0.295418249, -0.00567797114,
      -0.0128520365, -0.0355432545, 0.0355432545, -0.0355432545, -0.128030592,
      0.00263635698, -0.00263635698, 0.00263635698, -0.0308437927, -0.102944558,
      -0.0308437927, -0.102944558, -0.0308437927, -0.102944558, 0.295418249,
      0.817121133;

  _mathist.push_back(Eigen::MatrixXd::Zero(17, 17));
  _mathist.back() << 20.6650929, -0.736046685, 6.91220224e-13, 5.86301232e-13,
      6.54773712e-13, 0.975818014, 1.31720614e-12, 1.30234711e-12,
      1.31179879e-12, 0.037823494, 0.408464785, 0.037823494, 0.408464785,
      0.037823494, 0.408464785, 0.037823494, 0.408464785, -0.736046685,
      1.41566044, 6.24139139e-14, 4.44946759e-14, 3.61974852e-14, 0.401429427,
      -3.69193959e-14, 2.39697516e-14, -2.35870753e-14, -0.0738834915,
      0.112395726, -0.0738834915, 0.112395726, -0.0738834915, 0.112395726,
      -0.0738834915, 0.112395726, 6.91220224e-13, 6.24139139e-14, 1.51376515,
      6.0278598e-14, 6.92737071e-14, -2.29854399e-14, 0.186532603,
      -1.32778711e-13, -1.0779655e-13, -0.0234162405, 0.0126669166,
      -0.0234162405, 0.0126669166, 0.0234162405, -0.0126669166, 0.0234162405,
      -0.0126669166, 5.86301232e-13, 4.44946759e-14, 6.0278598e-14, 1.51376515,
      1.1067034e-13, 1.74326027e-14, -1.327853e-13, 0.186532603,
      -1.26673688e-13, -0.0234162405, 0.0126669166, 0.0234162405, -0.0126669166,
      0.0234162405, -0.0126669166, -0.0234162405, 0.0126669166, 6.54773712e-13,
      3.61974852e-14, 6.92737071e-14, 1.1067034e-13, 1.51376515,
      -2.24511004e-14, -1.07769541e-13, -1.26632088e-13, 0.186532603,
      -0.0234162405, 0.0126669166, 0.0234162405, -0.0126669166, -0.0234162405,
      0.0126669166, 0.0234162405, -0.0126669166, 0.975818014, 0.401429427,
      -2.29854399e-14, 1.74326027e-14, -2.24511004e-14, 0.284310819,
      -6.98729641e-14, 1.15034194e-14, -4.87658342e-14, -0.08451792,
      0.0823249934, -0.08451792, 0.0823249934, -0.08451792, 0.0823249934,
      -0.08451792, 0.0823249934, 1.31720614e-12, -3.69193959e-14, 0.186532603,
      -1.327853e-13, -1.07769541e-13, -6.98729641e-14, 0.270713036,
      -1.43602445e-13, -1.28956243e-13, -0.0285982779, 0.0450793738,
      -0.0285982779, 0.0450793738, 0.0285982779, -0.0450793738, 0.0285982779,
      -0.0450793738, 1.30234711e-12, 2.39697516e-14, -1.32778711e-13,
      0.186532603, -1.26632088e-13, 1.15034194e-14, -1.43602445e-13,
      0.270713036, -1.4092587e-13, -0.0285982779, 0.0450793738, 0.0285982779,
      -0.0450793738, 0.0285982779, -0.0450793738, -0.0285982779, 0.0450793738,
      1.31179879e-12, -2.35870753e-14, -1.0779655e-13, -1.26673688e-13,
      0.186532603, -4.87658342e-14, -1.28956243e-13, -1.4092587e-13,
      0.270713036, -0.0285982779, 0.0450793738, 0.0285982779, -0.0450793738,
      -0.0285982779, 0.0450793738, 0.0285982779, -0.0450793738, 0.037823494,
      -0.0738834915, -0.0234162405, -0.0234162405, -0.0234162405, -0.08451792,
      -0.0285982779, -0.0285982779, -0.0285982779, 0.412114637, -0.121917634,
      -0.0187677523, -0.0548799033, -0.0187677523, -0.0548799033, -0.0187677523,
      -0.0548799033, 0.408464785, 0.112395726, 0.0126669166, 0.0126669166,
      0.0126669166, 0.0823249934, 0.0450793738, 0.0450793738, 0.0450793738,
      -0.121917634, 0.0714492549, -0.0548799033, 0.00130926797, -0.0548799033,
      0.00130926797, -0.0548799033, 0.00130926797, 0.037823494, -0.0738834915,
      -0.0234162405, 0.0234162405, 0.0234162405, -0.08451792, -0.0285982779,
      0.0285982779, 0.0285982779, -0.0187677523, -0.0548799033, 0.412114637,
      -0.121917634, -0.0187677523, -0.0548799033, -0.0187677523, -0.0548799033,
      0.408464785, 0.112395726, 0.0126669166, -0.0126669166, -0.0126669166,
      0.0823249934, 0.0450793738, -0.0450793738, -0.0450793738, -0.0548799033,
      0.00130926797, -0.121917634, 0.0714492549, -0.0548799033, 0.00130926797,
      -0.0548799033, 0.00130926797, 0.037823494, -0.0738834915, 0.0234162405,
      0.0234162405, -0.0234162405, -0.08451792, 0.0285982779, 0.0285982779,
      -0.0285982779, -0.0187677523, -0.0548799033, -0.0187677523, -0.0548799033,
      0.412114637, -0.121917634, -0.0187677523, -0.0548799033, 0.408464785,
      0.112395726, -0.0126669166, -0.0126669166, 0.0126669166, 0.0823249934,
      -0.0450793738, -0.0450793738, 0.0450793738, -0.0548799033, 0.00130926797,
      -0.0548799033, 0.00130926797, -0.121917634, 0.0714492549, -0.0548799033,
      0.00130926797, 0.037823494, -0.0738834915, 0.0234162405, -0.0234162405,
      0.0234162405, -0.08451792, 0.0285982779, -0.0285982779, 0.0285982779,
      -0.0187677523, -0.0548799033, -0.0187677523, -0.0548799033, -0.0187677523,
      -0.0548799033, 0.412114637, -0.121917634, 0.408464785, 0.112395726,
      -0.0126669166, 0.0126669166, -0.0126669166, 0.0823249934, -0.0450793738,
      0.0450793738, -0.0450793738, -0.0548799033, 0.00130926797, -0.0548799033,
      0.00130926797, -0.0548799033, 0.00130926797, -0.121917634, 0.0714492549;

  _mathist.push_back(Eigen::MatrixXd::Zero(17, 17));
  _mathist.back() << 19.3186093, -0.966888296, 6.71190209e-13, 5.74264023e-13,
      6.37351711e-13, 0.748367435, 1.32103028e-12, 1.30734393e-12,
      1.3160266e-12, 0.0143246683, 0.306658668, 0.0143246683, 0.306658668,
      0.0143246683, 0.306658668, 0.0143246683, 0.306658668, -0.966888296,
      0.564096213, -5.9431304e-14, -8.32022058e-14, -8.56090938e-14,
      -0.168055893, -1.57257468e-13, -1.0396626e-13, -1.45605957e-13,
      -0.195427134, -0.173734713, -0.195427134, -0.173734713, -0.195427134,
      -0.173734713, -0.195427134, -0.173734713, 6.71190209e-13, -5.9431304e-14,
      0.663578514, 2.97117232e-14, 3.78023941e-14, -2.07389032e-13,
      -0.131494819, -6.64515631e-13, -6.41983257e-13, -0.116063354,
      -0.0646849536, -0.116063354, -0.0646849536, 0.116063354, 0.0646849536,
      0.116063354, 0.0646849536, 5.74264023e-13, -8.32022058e-14,
      2.97117232e-14, 0.663578514, 7.5077638e-14, -1.74217732e-13,
      -6.64580111e-13, -0.131494819, -6.5890543e-13, -0.116063354,
      -0.0646849536, 0.116063354, 0.0646849536, 0.116063354, 0.0646849536,
      -0.116063354, -0.0646849536, 6.37351711e-13, -8.56090938e-14,
      3.78023941e-14, 7.5077638e-14, 0.663578514, -2.06952795e-13,
      -6.41975407e-13, -6.58862495e-13, -0.131494819, -0.116063354,
      -0.0646849536, 0.116063354, 0.0646849536, -0.116063354, -0.0646849536,
      0.116063354, 0.0646849536, 0.748367435, -0.168055893, -2.07389032e-13,
      -1.74217732e-13, -2.06952795e-13, -0.21011594, -9.17034549e-14,
      -2.20893609e-14, -7.36995879e-14, -0.21779414, -0.194437617, -0.21779414,
      -0.194437617, -0.21779414, -0.194437617, -0.21779414, -0.194437617,
      1.32103028e-12, -1.57257468e-13, -0.131494819, -6.64580111e-13,
      -6.41975407e-13, -9.17034549e-14, -0.0131305487, -4.90295015e-15,
      7.10463999e-15, -0.117540295, -0.0384350948, -0.117540295, -0.0384350948,
      0.117540295, 0.0384350948, 0.117540295, 0.0384350948, 1.30734393e-12,
      -1.0396626e-13, -6.64515631e-13, -0.131494819, -6.58862495e-13,
      -2.20893609e-14, -4.90295015e-15, -0.0131305487, -4.02382301e-16,
      -0.117540295, -0.0384350948, 0.117540295, 0.0384350948, 0.117540295,
      0.0384350948, -0.117540295, -0.0384350948, 1.3160266e-12, -1.45605957e-13,
      -6.41983257e-13, -6.5890543e-13, -0.131494819, -7.36995879e-14,
      7.10463999e-15, -4.02382301e-16, -0.0131305487, -0.117540295,
      -0.0384350948, 0.117540295, 0.0384350948, -0.117540295, -0.0384350948,
      0.117540295, 0.0384350948, 0.0143246683, -0.195427134, -0.116063354,
      -0.116063354, -0.116063354, -0.21779414, -0.117540295, -0.117540295,
      -0.117540295, 0.130839269, -0.309099441, -0.0235986742, -0.103296492,
      -0.0235986742, -0.103296492, -0.0235986742, -0.103296492, 0.306658668,
      -0.173734713, -0.0646849536, -0.0646849536, -0.0646849536, -0.194437617,
      -0.0384350948, -0.0384350948, -0.0384350948, -0.309099441, -0.189660773,
      -0.103296492, -0.133917177, -0.103296492, -0.133917177, -0.103296492,
      -0.133917177, 0.0143246683, -0.195427134, -0.116063354, 0.116063354,
      0.116063354, -0.21779414, -0.117540295, 0.117540295, 0.117540295,
      -0.0235986742, -0.103296492, 0.130839269, -0.309099441, -0.0235986742,
      -0.103296492, -0.0235986742, -0.103296492, 0.306658668, -0.173734713,
      -0.0646849536, 0.0646849536, 0.0646849536, -0.194437617, -0.0384350948,
      0.0384350948, 0.0384350948, -0.103296492, -0.133917177, -0.309099441,
      -0.189660773, -0.103296492, -0.133917177, -0.103296492, -0.133917177,
      0.0143246683, -0.195427134, 0.116063354, 0.116063354, -0.116063354,
      -0.21779414, 0.117540295, 0.117540295, -0.117540295, -0.0235986742,
      -0.103296492, -0.0235986742, -0.103296492, 0.130839269, -0.309099441,
      -0.0235986742, -0.103296492, 0.306658668, -0.173734713, 0.0646849536,
      0.0646849536, -0.0646849536, -0.194437617, 0.0384350948, 0.0384350948,
      -0.0384350948, -0.103296492, -0.133917177, -0.103296492, -0.133917177,
      -0.309099441, -0.189660773, -0.103296492, -0.133917177, 0.0143246683,
      -0.195427134, 0.116063354, -0.116063354, 0.116063354, -0.21779414,
      0.117540295, -0.117540295, 0.117540295, -0.0235986742, -0.103296492,
      -0.0235986742, -0.103296492, -0.0235986742, -0.103296492, 0.130839269,
      -0.309099441, 0.306658668, -0.173734713, 0.0646849536, -0.0646849536,
      0.0646849536, -0.194437617, 0.0384350948, -0.0384350948, 0.0384350948,
      -0.103296492, -0.133917177, -0.103296492, -0.133917177, -0.103296492,
      -0.133917177, -0.309099441, -0.189660773;

  _mathist.push_back(Eigen::MatrixXd::Zero(17, 17));
  _mathist.back() << 18.2396482, -1.14406063, 6.2768402e-13, 5.42001414e-13,
      5.98041641e-13, 0.57113952, 1.14193136e-12, 1.12980571e-12,
      1.13755399e-12, -0.00390397444, 0.227367628, -0.00390397444, 0.227367628,
      -0.00390397444, 0.227367628, -0.00390397444, 0.227367628, -1.14406063,
      -0.0595793533, 2.33136398e-13, 2.08888935e-13, 2.0805246e-13,
      -0.580803733, -3.19118322e-14, 1.77157687e-14, -2.04882988e-14,
      -0.280839212, -0.380093553, -0.280839212, -0.380093553, -0.280839212,
      -0.380093553, -0.280839212, -0.380093553, 6.2768402e-13, 2.33136398e-13,
      0.0378576103, 1.73743671e-14, 2.74052805e-14, 1.97230495e-15,
      -0.357159535, -7.11933092e-13, -6.91028333e-13, -0.179607515, -0.11900992,
      -0.179607515, -0.11900992, 0.179607515, 0.11900992, 0.179607515,
      0.11900992, 5.42001414e-13, 2.08888935e-13, 1.73743671e-14, 0.0378576103,
      5.59238458e-14, 3.322337e-14, -7.1191182e-13, -0.357159535,
      -7.07394615e-13, -0.179607515, -0.11900992, 0.179607515, 0.11900992,
      0.179607515, 0.11900992, -0.179607515, -0.11900992, 5.98041641e-13,
      2.0805246e-13, 2.74052805e-14, 5.59238458e-14, 0.0378576103,
      3.28773384e-15, -6.91037934e-13, -7.07389016e-13, -0.357159535,
      -0.179607515, -0.11900992, 0.179607515, 0.11900992, -0.179607515,
      -0.11900992, 0.179607515, 0.11900992, 0.57113952, -0.580803733,
      1.97230495e-15, 3.322337e-14, 3.28773384e-15, -0.554873652,
      8.73188288e-14, 1.51058959e-13, 1.02220982e-13, -0.302200962,
      -0.381296987, -0.302200962, -0.381296987, -0.302200962, -0.381296987,
      -0.302200962, -0.381296987, 1.14193136e-12, -3.19118322e-14, -0.357159535,
      -7.1191182e-13, -6.91037934e-13, 8.73188288e-14, -0.192055554,
      -1.88308912e-15, 7.69475431e-15, -0.16807803, -0.0859275816, -0.16807803,
      -0.0859275816, 0.16807803, 0.0859275816, 0.16807803, 0.0859275816,
      1.12980571e-12, 1.77157687e-14, -7.11933092e-13, -0.357159535,
      -7.07389016e-13, 1.51058959e-13, -1.88308912e-15, -0.192055554,
      3.85470593e-15, -0.16807803, -0.0859275816, 0.16807803, 0.0859275816,
      0.16807803, 0.0859275816, -0.16807803, -0.0859275816, 1.13755399e-12,
      -2.04882988e-14, -6.91028333e-13, -7.07394615e-13, -0.357159535,
      1.02220982e-13, 7.69475431e-15, 3.85470593e-15, -0.192055554, -0.16807803,
      -0.0859275816, 0.16807803, 0.0859275816, -0.16807803, -0.0859275816,
      0.16807803, 0.0859275816, -0.00390397444, -0.280839212, -0.179607515,
      -0.179607515, -0.179607515, -0.302200962, -0.16807803, -0.16807803,
      -0.16807803, -0.0143991012, -0.411273518, -0.0270490827, -0.134772773,
      -0.0270490827, -0.134772773, -0.0270490827, -0.134772773, 0.227367628,
      -0.380093553, -0.11900992, -0.11900992, -0.11900992, -0.381296987,
      -0.0859275816, -0.0859275816, -0.0859275816, -0.411273518, -0.338002386,
      -0.134772773, -0.224536493, -0.134772773, -0.224536493, -0.134772773,
      -0.224536493, -0.00390397444, -0.280839212, -0.179607515, 0.179607515,
      0.179607515, -0.302200962, -0.16807803, 0.16807803, 0.16807803,
      -0.0270490827, -0.134772773, -0.0143991012, -0.411273518, -0.0270490827,
      -0.134772773, -0.0270490827, -0.134772773, 0.227367628, -0.380093553,
      -0.11900992, 0.11900992, 0.11900992, -0.381296987, -0.0859275816,
      0.0859275816, 0.0859275816, -0.134772773, -0.224536493, -0.411273518,
      -0.338002386, -0.134772773, -0.224536493, -0.134772773, -0.224536493,
      -0.00390397444, -0.280839212, 0.179607515, 0.179607515, -0.179607515,
      -0.302200962, 0.16807803, 0.16807803, -0.16807803, -0.0270490827,
      -0.134772773, -0.0270490827, -0.134772773, -0.0143991012, -0.411273518,
      -0.0270490827, -0.134772773, 0.227367628, -0.380093553, 0.11900992,
      0.11900992, -0.11900992, -0.381296987, 0.0859275816, 0.0859275816,
      -0.0859275816, -0.134772773, -0.224536493, -0.134772773, -0.224536493,
      -0.411273518, -0.338002386, -0.134772773, -0.224536493, -0.00390397444,
      -0.280839212, 0.179607515, -0.179607515, 0.179607515, -0.302200962,
      0.16807803, -0.16807803, 0.16807803, -0.0270490827, -0.134772773,
      -0.0270490827, -0.134772773, -0.0270490827, -0.134772773, -0.0143991012,
      -0.411273518, 0.227367628, -0.380093553, 0.11900992, -0.11900992,
      0.11900992, -0.381296987, 0.0859275816, -0.0859275816, 0.0859275816,
      -0.134772773, -0.224536493, -0.134772773, -0.224536493, -0.134772773,
      -0.224536493, -0.411273518, -0.338002386;

  Eigen::VectorXd Coeffs = adiis.CalcCoeff(_dmatHist, _mathist);

  Eigen::VectorXd Ref = Eigen::VectorXd::Zero(3);
  Ref << 4.45639501e-17, 1.76102089e-18, 1;

  bool check_adiis = Coeffs.isApprox(Ref, 0.00001);
  if (!check_adiis) {
    std::cout << "Ref:" << Ref << std::endl;
    std::cout << "Coeffs:" << Coeffs << std::endl;
  }

  BOOST_CHECK_EQUAL(check_adiis, 1);
}

BOOST_AUTO_TEST_SUITE_END()
