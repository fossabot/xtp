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

#include "votca/xtp/aomatrix.h"
#include <numeric>
#include <votca/xtp/statefilter.h>

namespace votca {
namespace xtp {
using std::flush;

void Statefilter::Initialize(tools::Property& options) {
  if (options.exists("oscillator_strength")) {
    _use_oscfilter = true;
    _oscthreshold = options.ifExistsReturnElseThrowRuntimeError<double>(
        "oscillator_strength");
  }
  if (options.exists("overlap")) {
    _use_overlapfilter = true;
    _overlapthreshold =
        options.ifExistsReturnElseReturnDefault<double>("overlap", 0.0);
  }
  if (options.exists("overlapbse")) {
    _use_overlapfilter_bse = true;
    _overlapthreshold =
        options.ifExistsReturnElseReturnDefault<double>("overlapbse", 0.0);
  }
//   if (options.exists("variational")) {
//    _use_variationalfilter = true;
//    _variationalthreshold =
//        options.ifExistsReturnElseReturnDefault<double>("variational", 0.0);
//  }
  
  
  if (options.exists("density")) {
    _use_densityfilter = true;
    _dmatthreshold =
        options.ifExistsReturnElseReturnDefault<double>("density", 0.0);
  }
  if (options.exists("localisation")) {
    _use_localisationfilter = true;
    std::string indices =
        options.ifExistsReturnElseThrowRuntimeError<std::string>(
            "localisation.fragment");
    QMFragment<BSE_Population> reg =
        QMFragment<BSE_Population>("Fragment", 0, indices);
    _loc_threshold = options.ifExistsReturnElseThrowRuntimeError<double>(
        "localisation.threshold");
    _fragment_loc.push_back(reg);
  }

  if (options.exists("charge_transfer")) {
    _use_dQfilter = true;
    std::string indices =
        options.ifExistsReturnElseThrowRuntimeError<std::string>(
            "charge_transfer.fragment");
    QMFragment<BSE_Population> reg =
        QMFragment<BSE_Population>("Fragment", 0, indices);
    _dQ_threshold = options.ifExistsReturnElseThrowRuntimeError<double>(
        "charge_transfer.threshold");
    _fragment_dQ.push_back(reg);
  }
  if (_use_dQfilter && _use_localisationfilter) {
    throw std::runtime_error(
        "Cannot use localisation and charge_transfer filter at the same time.");
  }
}

void Statefilter::PrintInfo() const {
  XTP_LOG(logDEBUG, *_log) << "Initial state: " << _statehist[0].ToString()
                           << flush;
  if (_statehist.size() > 1) {
    XTP_LOG(logDEBUG, *_log)
        << "Last state: " << _statehist.back().ToString() << flush;
  }
  if (_use_oscfilter) {
    XTP_LOG(logDEBUG, *_log) << "Using oscillator strength filter with threshold "
                             << _oscthreshold << flush;
  }
  if (_use_overlapfilter) {
    if (_overlapthreshold == 0.0) {
      XTP_LOG(logDEBUG, *_log)
          << "Using overlap filter with no threshold " << flush;
    } else {
      XTP_LOG(logDEBUG, *_log)
          << "Using overlap filter with threshold " << _overlapthreshold << flush;
    }
  }
  if (_use_overlapfilter_bse) {
    if (_overlapthreshold == 0.0) {
      XTP_LOG(logDEBUG, *_log)
          << "Using overlap filter BSE with no threshold " << flush;
    } else {
      XTP_LOG(logDEBUG, *_log)
          << "Using overlap filter BSE with threshold " << _overlapthreshold << flush;
    }
  }
//  if (_use_variationalfilter) {
//    if (_variationalthreshold == 0.0) {
//      XTP_LOG(logDEBUG, *_log)
//          << "Using Variational filter BSE with no threshold " << flush;
//    } else {
//      XTP_LOG(logDEBUG, *_log)
//          << "Using Variational filter BSE with threshold " << _variationalthreshold << flush;
//    }
//  }
    
  if (_use_densityfilter) {
    if (_dmatthreshold == 0.0) {
      XTP_LOG(logDEBUG, *_log)
          << "Using density filter with no threshold " << flush;
    } else {
      XTP_LOG(logDEBUG, *_log)
          << "Using density filter with threshold " << _dmatthreshold << flush;
    }
  }
  if (_use_localisationfilter) {
    XTP_LOG(logDEBUG, *_log)
        << "Using localisation filter for fragment" << _fragment_loc[0].name()
        << " with cutoff " << _loc_threshold << flush;
  }
  if (_use_dQfilter) {
    XTP_LOG(logDEBUG, *_log)
        << "Using Delta Q filter for fragment" << _fragment_dQ[0].name()
        << "with cutoff  " << _dQ_threshold << flush;
  }
  if (_use_oscfilter && _use_dQfilter) {
    XTP_LOG(logDEBUG, *_log) << "WARNING: filtering for optically active CT "
                                "transition - might not make sense... "
                             << flush;
  }
  if (_use_dQfilter + _use_oscfilter + _use_localisationfilter +
          _use_overlapfilter + _use_densityfilter + _use_overlapfilter_bse + _use_variationalfilter<
      1) {
    XTP_LOG(logDEBUG, *_log) << "WARNING: No filter is used " << flush;
  }
}

std::vector<int> Statefilter::ComparePairofVectors(
    std::vector<int>& vec1, std::vector<int>& vec2) const {
  std::vector<int> result(std::min(vec1, vec2));
  std::vector<int>::iterator it;
  std::sort(vec1.begin(), vec1.end());
  std::sort(vec2.begin(), vec2.end());
  it = std::set_intersection(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(),
                             result.begin());
  result.resize(it - result.begin());
  return result;
}

std::vector<int> Statefilter::CollapseResults(
    std::vector<std::vector<int> >& results) const {
  if (results.size() == 1) {
    return results[0];
  } else {
    std::vector<int> result = results[0];
    for (unsigned i = 1; i < results.size(); i++) {
      result = ComparePairofVectors(result, results[i]);
    }
    return result;
  }
}

QMState Statefilter::CalcState(const Orbitals& orbitals) const {

  if (_use_dQfilter + _use_oscfilter + _use_localisationfilter +
          _use_overlapfilter + _use_densityfilter +_use_overlapfilter_bse 
           <
      1) {
    return _statehist[0];
  }

  std::vector<std::vector<int> > results;
  if (_use_oscfilter) {
    results.push_back(OscFilter(orbitals));
  }
  if (_use_localisationfilter) {
    results.push_back(LocFilter(orbitals));
  }
  if (_use_overlapfilter) {
    results.push_back(OverlapFilter(orbitals));
  }
  if (_use_overlapfilter_bse) {
    results.push_back(OverlapFilterBSE(orbitals));
  }
  //if (_use_variationalfilter) {
  //  results.push_back(VariationalFilter(orbitals));
  //}
  if (_use_densityfilter) {
    results.push_back(DensityFilter(orbitals));
  }
  if (_use_dQfilter) {
    results.push_back(DeltaQFilter(orbitals));
  }

  std::vector<int> result = CollapseResults(results);
  QMState state;
  if (result.size() < 1) {
    state = _statehist.back();
    XTP_LOG(logDEBUG, *_log)
        << "No State found by filter using last state: " << state.ToString()
        << flush;
  } else {
    state = QMState(_statehist.back().Type(), result[0], false);
    XTP_LOG(logDEBUG, *_log) << "Next State is: " << state.ToString() << flush;
  }
  return state;
}

QMState Statefilter::CalcStateAndUpdate(const Orbitals& orbitals) {
  QMState result = CalcState(orbitals);
  _statehist.push_back(result);
  if (_use_overlapfilter) {
    UpdateLastCoeff(orbitals);
  }
  if (_use_overlapfilter_bse) {
    UpdateLastBSE_R(orbitals);
    UpdateLastBSE_AR(orbitals);
  }
  if(_use_densityfilter){
    UpdateLastDmat(orbitals);
  }
//  if(_use_variationalfilter){
//    UpdateLastBSE_R(orbitals);
//    UpdateLastBSE_AR(orbitals);
//    UpdateLastBSE_energies(orbitals);
//  }
  return result;
}

std::vector<int> Statefilter::OscFilter(const Orbitals& orbitals) const {
  Eigen::VectorXd oscs = orbitals.Oscillatorstrengths();
  std::vector<int> indexes;
  for (int i = 0; i < oscs.size(); i++) {
    if (oscs[i] > _oscthreshold) indexes.push_back(i);
  }
  return indexes;
}

std::vector<int> Statefilter::LocFilter(const Orbitals& orbitals) const {
  std::vector<int> indexes;
  Lowdin low;
  std::vector<QMFragment<BSE_Population> > loc = _fragment_loc;
  low.CalcChargeperFragment(loc, orbitals, _statehist[0].Type());
  const Eigen::VectorXd& popE = loc[0].value().E;
  const Eigen::VectorXd& popH = loc[0].value().H;
  for (int i = 0; i < popE.size(); i++) {
    if (popE[i] > _loc_threshold && popH[i] > _loc_threshold) {
      indexes.push_back(i);
    }
  }
  return indexes;
}

std::vector<int> Statefilter::DeltaQFilter(const Orbitals& orbitals) const {
  std::vector<int> indexes;
  Lowdin low;
  std::vector<QMFragment<BSE_Population> > loc = _fragment_dQ;
  low.CalcChargeperFragment(loc, orbitals, _statehist[0].Type());
  Eigen::VectorXd dq = (loc[0].value().H - loc[0].value().E).cwiseAbs();

  for (int i = 0; i < dq.size(); i++) {
    if (dq[i] > _dQ_threshold) {
      indexes.push_back(i);
    }
  }
  return indexes;
}

Eigen::VectorXd Statefilter::CalculateOverlap(const Orbitals& orbitals) const {
  Eigen::MatrixXd coeffs = CalcOrthoCoeffs(orbitals);
  Eigen::VectorXd overlap = (coeffs * _laststatecoeff).cwiseAbs2();
  return overlap;
}


Eigen::VectorXd Statefilter::CalculateOverlapBSE(const Orbitals& orbitals) const {
    
    int nostates=orbitals.NumberofStates(_statehist[0].Type());
    Eigen::VectorXd overlap=Eigen::VectorXd::Zero(nostates);
    for(int i=0;i<nostates;i++){
        QMState state(_statehist[0].Type(),i,false);
        Eigen::VectorXd a = orbitals.BSESinglets().eigenvectors().col(i);
        Eigen::VectorXd b = orbitals.BSESinglets().eigenvectors2().col(i);    
        overlap(i) = std::norm(_lastbse_R.dot(a) - _lastbse_AR.dot(b)) ;
    }
    std::cout << " \n Testing overlap Singlet \n " << overlap << std::endl;
    return overlap;
}

//Eigen::VectorXd Statefilter::CalculateVariationalBSE(const Orbitals& orbitals) const {
//    
//    int nostates=orbitals.NumberofStates(_statehist[0].Type());
//    Eigen::VectorXd overlap= CalculateOverlapBSE(orbitals);
//    Eigen::VectorXd var = Eigen::VectorXd::Zero(nostates);
//    
//    for(int i=0;i<nostates;i++){
//        QMState state(_statehist[0].Type(),i,false);
//        Eigen::VectorXd diff_en = orbitals.BSESinglets().eigenvalues() - _lastbseenergies(i); 
//        var(i) = diff_en(i)*diff_en(i)*overlap(i);
//    }
//    std::cout << " \n Testing Variational Singlet \n " << var << std::endl;
//    return var;
//}



Eigen::VectorXd Statefilter::CalculateDNorm(const Orbitals& orbitals) const{
    
    int nostates=orbitals.NumberofStates(_statehist[0].Type());
    
    Eigen::VectorXd norm=Eigen::VectorXd::Zero(nostates);
    for(int i=0;i<nostates;i++){
        QMState state(_statehist[0].Type(),i,false);
        Eigen::MatrixXd diff = (orbitals.DensityMatrixFull(state)-_lastdmat);
        norm(i) = diff.norm()/(_lastdmat.norm());
    }
    std::cout << " \n || D_new - D_old||/||D_old|| \n" << norm << std::endl;
    return norm;    
}


Eigen::MatrixXd Statefilter::CalcOrthoCoeffs(const Orbitals& orbitals) const {
  QMStateType type = _statehist[0].Type();
  Eigen::MatrixXd coeffs;
  if (type.isSingleParticleState()) {
    if (type == QMStateType::DQPstate) {
      coeffs = orbitals.CalculateQParticleAORepresentation();
    } else {
      coeffs = orbitals.MOs().eigenvectors();
    }
  } else {
    throw std::runtime_error("Overlap for excitons not implemented yet");
  }
  return coeffs;
}

void Statefilter::UpdateLastCoeff(const Orbitals& orbitals) {
  Eigen::MatrixXd ortho_coeffs = CalcOrthoCoeffs(orbitals);
  int offset = 0;
  if (_statehist[0].Type() == QMStateType::DQPstate) {
    offset = orbitals.getGWAmin();
  }
  _laststatecoeff = ortho_coeffs.col(_statehist.back().Index() - offset);
}

void Statefilter::UpdateLastDmat(const Orbitals& orbitals){
    _lastdmat=orbitals.DensityMatrixFull(_statehist.back());
}


void Statefilter::UpdateLastBSE_R(const Orbitals& orbitals){
    _lastbse_R = 
           orbitals.BSESinglets().eigenvectors().col(_statehist.back().Index());
}

void Statefilter::UpdateLastBSE_AR(const Orbitals& orbitals){
    _lastbse_AR = 
          orbitals.BSESinglets().eigenvectors2().col(_statehist.back().Index());
}

void Statefilter::UpdateLastBSE_energies(const Orbitals& orbitals){
    _lastbseenergies = orbitals.BSESinglets().eigenvalues(); 
}

std::vector<int> Statefilter::OverlapFilter(const Orbitals& orbitals) const {
  std::vector<int> indexes;
  if (_statehist.size() <= 1) {
    indexes = std::vector<int>{_statehist[0].Index()};
    return indexes;
  }

  Eigen::VectorXd Overlap = CalculateOverlap(orbitals);
  int validelements = Overlap.size();
  for (int i = 0; i < Overlap.size(); i++) {
    if (Overlap(i) < _overlapthreshold) {
      validelements--;
    }
  }

  std::vector<int> index = std::vector<int>(Overlap.size());
  std::iota(index.begin(), index.end(), 0);
  std::stable_sort(index.begin(), index.end(), [&Overlap](int i1, int i2) {
    return Overlap[i1] > Overlap[i2];
  });

  int offset = 0;
  if (_statehist[0].Type() == QMStateType::DQPstate) {
    offset = orbitals.getGWAmin();
  }

  for (int i : index) {
    if (int(indexes.size()) == validelements) {
      break;
    }
    indexes.push_back(i + offset);
  }
  return indexes;
}

std::vector<int> Statefilter::OverlapFilterBSE(const Orbitals& orbitals) const {
  std::vector<int> indexes;
  if (_statehist.size() <= 1) {
    indexes = std::vector<int>{_statehist[0].Index()};
    return indexes;
  }

  Eigen::VectorXd Overlap = CalculateOverlapBSE(orbitals);
  int validelements = Overlap.size();
  for (int i = 0; i < Overlap.size(); i++) {
    if (Overlap(i) < _overlapthreshold) {
      validelements--;
    }
  }

  std::vector<int> index = std::vector<int>(Overlap.size());
  std::iota(index.begin(), index.end(), 0);
  std::stable_sort(index.begin(), index.end(), [&Overlap](int i1, int i2) {
    return Overlap[i1] > Overlap[i2];
  });

  int offset = 0;
  if (_statehist[0].Type() == QMStateType::DQPstate) {
    offset = orbitals.getGWAmin();
  }

  for (int i : index) {
    if (int(indexes.size()) == validelements) {
      break;
    }
    indexes.push_back(i + offset);
  }
  return indexes;
}


std::vector<int> Statefilter::DensityFilter(const Orbitals& orbitals) const { 
  std::vector<int> indexes;
  if (_statehist.size() <= 1) {
    indexes = std::vector<int>{_statehist[0].Index()};
    return indexes;
  }
  
  Eigen::VectorXd dnorm = CalculateDNorm(orbitals);
  
  int validelements = dnorm.size();
  for (int i = 0; i < dnorm.size(); i++) {
    if (dnorm(i) > _dmatthreshold) {
      validelements--;
    }
  }
  std::vector<int> index = std::vector<int>(dnorm.size());
  std::iota(index.begin(), index.end(), 0);
  std::stable_sort(index.begin(), index.end(), [&dnorm](int i1, int i2) {
    return dnorm[i1] < dnorm[i2];
  });
  int offset = 0;
  if (_statehist[0].Type() == QMStateType::DQPstate) {
    offset = orbitals.getGWAmin();
  }

  for (int i : index) {
    if (int(indexes.size()) == validelements) {
      break;
    }
    indexes.push_back(i + offset);
  }
  return indexes;
}

//std::vector<int> Statefilter::VariationalFilter(const Orbitals& orbitals) const { 
//  std::vector<int> indexes;
//  if (_statehist.size() <= 1) {
//    indexes = std::vector<int>{_statehist[0].Index()};
//    return indexes;
//  }
//  
//  Eigen::VectorXd var = CalculateVariationalBSE(orbitals);
//  
//  int validelements = var.size();
//  for (int i = 0; i < var.size(); i++) {
//    if (var(i) > _variationalthreshold) {
//      validelements--;
//    }
//  }
//  std::vector<int> index = std::vector<int>(var.size());
//  std::iota(index.begin(), index.end(), 0);
//  std::stable_sort(index.begin(), index.end(), [&var](int i1, int i2) {
//    return var[i1] < var[i2];
//  });
//  int offset = 0;
//  if (_statehist[0].Type() == QMStateType::DQPstate) {
//    offset = orbitals.getGWAmin();
//  }
//
//  for (int i : index) {
//    if (int(indexes.size()) == validelements) {
//      break;
//    }
//    indexes.push_back(i + offset);
//  }
//  return indexes;
//}


void Statefilter::WriteToCpt(CheckpointWriter& w) const {
  std::vector<std::string> statehiststring;
  statehiststring.reserve(_statehist.size());
  for (const QMState& s : _statehist) {
    statehiststring.push_back(s.ToString());
  }
  w(statehiststring, "statehist");
  w(_use_oscfilter, "oscfilter");
  w(_oscthreshold, "oscthreshold");

  w(_use_overlapfilter, "overlapfilter");
  w(_use_overlapfilter_bse, "overlapfilterbse");
//  w(_use_variationalfilter, "variationalfilter");
//  w(_variationalthreshold, "variationalthreshold");
  w(_overlapthreshold, "overlapthreshold");
  w(_lastbse_R, "lastbse_R");
  w(_lastbse_AR, "lastbse_AR");
  w(_lastbseenergies, "lastbseenergies");
  
  
  w(_use_densityfilter, "densityfilter");
  w(_dmatthreshold, "dmatthreshold");
  w(_lastdmat, "lastdmat");

  w(_use_localisationfilter, "localisationfilter");
  w(_loc_threshold, "locthreshold");
  w(int(_fragment_loc.size()), "loc_fragments");
  for (unsigned i = 0; i < _fragment_loc.size(); i++) {
    CheckpointWriter ww = w.openChild("fragment_loc_" + std::to_string(i));
    _fragment_loc[i].WriteToCpt(ww);
  }

  w(_use_dQfilter, "dQfilter");
  w(_dQ_threshold, "dQthreshold");
  w(int(_fragment_dQ.size()), "dQ_fragments");
  for (unsigned i = 0; i < _fragment_dQ.size(); i++) {
    CheckpointWriter ww = w.openChild("fragment_dQ_" + std::to_string(i));
    _fragment_dQ[i].WriteToCpt(ww);
  }
}

void Statefilter::ReadFromCpt(CheckpointReader& r) {
  std::vector<std::string> statehiststring;
  r(statehiststring, "statehist");
  _statehist.clear();
  _statehist.reserve(statehiststring.size());
  for (const std::string& s : statehiststring) {
    _statehist.push_back(QMState(s));
  }
  r(_use_oscfilter, "oscfilter");
  r(_oscthreshold, "oscthreshold");
 
//  r(_use_variationalfilter, "variationalfilter");
//  r(_variationalthreshold, "variationalthreshold");
  r(_use_overlapfilter, "overlapfilter");
  r(_overlapthreshold, "overlapthreshold");
  r(_laststatecoeff, "laststatecoeff");
  r(_use_overlapfilter_bse, "overlapfilterbse");
  r(_lastbse_R, "lastbse_R");
  r(_lastbse_AR, "lastbse_AR");
  
  r(_use_densityfilter, "densityfilter");
  r(_dmatthreshold, "dmatthreshold");
  r(_lastdmat, "lastdmat");
  r(_lastbseenergies, "lastbseenergies");
  
  r(_use_localisationfilter, "localisationfilter");
  r(_loc_threshold, "locthreshold");
  _fragment_loc.clear();
  int loc_size = 0;
  r(loc_size, "loc_fragments");
  _fragment_loc.resize(loc_size);
  for (unsigned i = 0; i < _fragment_loc.size(); i++) {
    CheckpointReader rr = r.openChild("fragment_loc_" + std::to_string(i));
    _fragment_loc[i].ReadFromCpt(rr);
  }

  r(_use_dQfilter, "dQfilter");
  r(_dQ_threshold, "dQthreshold");
  _fragment_dQ.clear();
  int dQ_size = 0;
  r(dQ_size, "dQ_fragments");
  _fragment_dQ.resize(dQ_size);
  for (unsigned i = 0; i < _fragment_dQ.size(); i++) {
    CheckpointReader rr = r.openChild("fragment_dQ_" + std::to_string(i));
    _fragment_dQ[i].ReadFromCpt(rr);
  }
}

}  // namespace xtp
}  // namespace votca
