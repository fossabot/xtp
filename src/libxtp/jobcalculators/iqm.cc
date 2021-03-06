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

// Third party includes
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

// VOTCA includes
#include <votca/tools/constants.h>

// Local VOTCA includes
#include "votca/xtp/atom.h"
#include "votca/xtp/logger.h"
#include "votca/xtp/qmpackagefactory.h"
#include "votca/xtp/segmentmapper.h"

// Local private VOTCA includes
#include "iqm.h"

using boost::format;
using namespace boost::filesystem;

namespace votca {
namespace xtp {

void IQM::Initialize(const tools::Property& user_options) {

  tools::Property options =
      LoadDefaultsAndUpdateWithUserOptions("xtp", user_options);
  ParseCommonOptions(options);
  QMPackageFactory::RegisterAll();

  ParseOptionsXML(options);
}

void IQM::ParseOptionsXML(const tools::Property& options) {

  // job tasks
  std::string tasks_string = options.get(".tasks").as<std::string>();
  if (tasks_string.find("input") != std::string::npos) {
    _do_dft_input = true;
  }
  if (tasks_string.find("dft") != std::string::npos) {
    _do_dft_run = true;
  }
  if (tasks_string.find("parse") != std::string::npos) {
    _do_dft_parse = true;
  }
  if (tasks_string.find("dftcoupling") != std::string::npos) {
    _do_dftcoupling = true;
  }
  if (tasks_string.find("gw") != std::string::npos) {
    _do_gwbse = true;
  }
  if (tasks_string.find("bsecoupling") != std::string::npos) {
    _do_bsecoupling = true;
  }

  // storage options
  std::string store_string = options.get(".store").as<std::string>();
  if (store_string.find("dft") != std::string::npos) {
    _store_dft = true;
  }
  if (store_string.find("gw") != std::string::npos) {
    _store_gw = true;
  }

  _dftpackage_options = this->UpdateDFTOptions(options);
  _gwbse_options = this->UpdateGWBSEOptions(options);

  _dftcoupling_options = options.get(".dftcoupling_options");
  _bsecoupling_options.add("bsecoupling", "");
  tools::Property& prop_bsecoupling = _bsecoupling_options.get("bsecoupling");
  prop_bsecoupling = options.get("bsecoupling");

  // read linker groups
  std::string linker =
      options.ifExistsReturnElseReturnDefault<std::string>(".linker_names", "");
  tools::Tokenizer toker(linker, ", \t\n");
  std::vector<std::string> linkers = toker.ToVector();
  for (const std::string& link : linkers) {
    tools::Tokenizer toker2(link, ":");
    std::vector<std::string> link_split = toker2.ToVector();
    if (link_split.size() != 2) {
      throw std::runtime_error(
          "Linker molecule has to be defined NAME:STATEGEO .e.g. DCV5T:n");
    }
    _linkers[link_split[0]] = QMState(link_split[1]);
  }

  // options for parsing data into state file
  std::string key_read = ".readjobfile";
  if (options.exists(key_read + ".singlet")) {
    std::string parse_string_s =
        options.get(key_read + ".singlet").as<std::string>();
    _singlet_levels = FillParseMaps(parse_string_s);
  }
  if (options.exists(key_read + ".triplet")) {
    std::string parse_string_t =
        options.get(key_read + ".triplet").as<std::string>();
    _triplet_levels = FillParseMaps(parse_string_t);
  }

  if (options.exists(key_read + ".hole")) {
    std::string parse_string_h =
        options.get(key_read + ".hole").as<std::string>();
    _hole_levels = FillParseMaps(parse_string_h);
  }
  if (options.exists(key_read + ".electron")) {
    std::string parse_string_e =
        options.get(key_read + ".electron").as<std::string>();
    _electron_levels = FillParseMaps(parse_string_e);
  }
}

std::map<std::string, QMState> IQM::FillParseMaps(
    const std::string& Mapstring) {
  tools::Tokenizer split_options(Mapstring, ", \t\n");
  std::map<std::string, QMState> type2level;
  for (const std::string& substring : split_options) {
    std::vector<std::string> segmentpnumber;
    tools::Tokenizer tok(substring, ":");
    tok.ToVector(segmentpnumber);
    if (segmentpnumber.size() != 2) {
      throw std::runtime_error("Parser iqm: Segment and exciton labels:" +
                               substring + "are not separated properly");
    }
    QMState state = QMState(segmentpnumber[1]);
    std::string segmentname = segmentpnumber[0];
    type2level[segmentname] = state;
  }
  return type2level;
}

void IQM::addLinkers(std::vector<const Segment*>& segments,
                     const Topology& top) {
  std::vector<QMState> result;
  const Segment* seg1 = segments[0];
  const Segment* seg2 = segments[1];
  std::vector<const Segment*> segmentsInMolecule =
      top.FindAllSegmentsOnMolecule(*seg1, *seg2);

  for (const Segment* segment : segmentsInMolecule) {
    Index idIterator = segment->getId();
    if (idIterator != seg1->getId() && idIterator != seg2->getId() &&
        isLinker(segment->getType())) {
      segments.push_back(segment);
    }
  }
}

bool IQM::isLinker(const std::string& name) {
  return _linkers.count(name) == 1;
}

void IQM::SetJobToFailed(Job::JobResult& jres, Logger& pLog,
                         const std::string& errormessage) {
  XTP_LOG(Log::error, pLog) << errormessage << std::flush;
  std::cout << pLog;
  jres.setError(errormessage);
  jres.setStatus(Job::FAILED);
}

void IQM::WriteLoggerToFile(const std::string& logfile, Logger& logger) {
  std::ofstream ofs;
  ofs.open(logfile, std::ofstream::out);
  if (!ofs.is_open()) {
    throw std::runtime_error("Bad file handle: " + logfile);
  }
  ofs << logger << std::endl;
  ofs.close();
}

Job::JobResult IQM::EvalJob(const Topology& top, Job& job, QMThread& opThread) {

  // report back to the progress observer
  Job::JobResult jres = Job::JobResult();

  std::string iqm_work_dir = "OR_FILES";
  std::string eqm_work_dir = "OR_FILES";
  std::string frame_dir =
      "frame_" + boost::lexical_cast<std::string>(top.getStep());

  Logger& pLog = opThread.getLogger();

  QMMapper mapper(pLog);
  mapper.LoadMappingFile(_mapfile);

  // get the information about the job executed by the thread
  Index job_ID = job.getId();
  tools::Property job_input = job.getInput();
  std::vector<tools::Property*> segment_list = job_input.Select("segment");
  Index ID_A = segment_list.front()->getAttribute<Index>("id");
  std::string type_A = segment_list.front()->getAttribute<std::string>("type");
  Index ID_B = segment_list.back()->getAttribute<Index>("id");
  std::string type_B = segment_list.back()->getAttribute<std::string>("type");

  std::string qmgeo_state_A = "n";
  if (segment_list.front()->exists("qm_geometry")) {
    qmgeo_state_A =
        segment_list.front()->getAttribute<std::string>("qm_geometry");
  }

  std::string qmgeo_state_B = "n";
  if (segment_list.back()->exists("qm_geometry")) {
    qmgeo_state_B =
        segment_list.back()->getAttribute<std::string>("qm_geometry");
  }
  QMState stateA(qmgeo_state_A);
  QMState stateB(qmgeo_state_B);

  // set the folders
  std::string pair_dir =
      (format("%1%%2%%3%%4%%5%") % "pair" % "_" % ID_A % "_" % ID_B).str();

  boost::filesystem::path arg_path, arg_pathA, arg_pathB, arg_pathAB;

  std::string orbFileA =
      (arg_pathA / eqm_work_dir / "molecules" / frame_dir /
       (format("%1%_%2%%3%") % "molecule" % ID_A % ".orb").str())
          .generic_string();
  ;
  std::string orbFileB =
      (arg_pathB / eqm_work_dir / "molecules" / frame_dir /
       (format("%1%_%2%%3%") % "molecule" % ID_B % ".orb").str())
          .generic_string();
  ;
  std::string orbFileAB =
      (arg_pathAB / iqm_work_dir / "pairs_iqm" / frame_dir /
       (format("%1%%2%%3%%4%%5%") % "pair_" % ID_A % "_" % ID_B % ".orb").str())
          .generic_string();
  ;
  std::string orb_dir =
      (arg_path / iqm_work_dir / "pairs_iqm" / frame_dir).generic_string();
  ;

  const Segment& seg_A = top.getSegment(ID_A);
  const Segment& seg_B = top.getSegment(ID_B);
  const QMNBList& nblist = top.NBList();
  const QMPair* pair = nblist.FindPair(&seg_A, &seg_B);

  XTP_LOG(Log::error, pLog)
      << TimeStamp() << " Evaluating pair " << job_ID << " [" << ID_A << ":"
      << ID_B << "] out of " << (top.NBList()).size() << std::flush;

  std::string package_append = "workdir_" + Identify();
  std::vector<const Segment*> segments;
  segments.push_back(&seg_A);
  segments.push_back(&seg_B);
  std::string work_dir =
      (arg_path / iqm_work_dir / package_append / frame_dir / pair_dir)
          .generic_string();

  if (_linkers.size() > 0) {
    addLinkers(segments, top);
  }
  Orbitals orbitalsAB;
  // if a pair object is available and is not linked take into account PBC,
  // otherwise write as is
  if (pair == nullptr || segments.size() > 2) {
    if (pair == nullptr) {
      XTP_LOG(Log::warning, pLog)
          << "PBCs are not taken into account when writing the coordinate file!"
          << std::flush;
    }

    orbitalsAB.QMAtoms() = mapper.map(*(segments[0]), stateA);
    orbitalsAB.QMAtoms().AddContainer(mapper.map(*(segments[1]), stateB));

    for (Index i = 2; i < Index(segments.size()); i++) {
      QMState linker_state = _linkers.at(segments[i]->getType());
      orbitalsAB.QMAtoms().AddContainer(
          mapper.map(*(segments[i]), linker_state));
    }

  } else {
    const Segment* seg1 = pair->Seg1();
    orbitalsAB.QMAtoms() = mapper.map(*seg1, stateA);
    Segment seg2 = pair->Seg2PbCopy();
    orbitalsAB.QMAtoms().AddContainer(mapper.map(seg2, stateB));
  }

  if (_do_dft_input || _do_dft_run || _do_dft_parse) {
    std::string qmpackage_work_dir =
        (arg_path / iqm_work_dir / package_append / frame_dir / pair_dir)
            .generic_string();
    ;

    Logger dft_logger(Log::current_level);
    dft_logger.setMultithreading(false);
    dft_logger.setPreface(Log::info, (format("\nDFT INF ...")).str());
    dft_logger.setPreface(Log::error, (format("\nDFT ERR ...")).str());
    dft_logger.setPreface(Log::warning, (format("\nDFT WAR ...")).str());
    dft_logger.setPreface(Log::debug, (format("\nDFT DBG ...")).str());
    std::string dftname = "package.name";
    std::string package = _dftpackage_options.get(dftname).as<std::string>();
    std::unique_ptr<QMPackage> qmpackage =
        std::unique_ptr<QMPackage>(QMPackages().Create(package));
    qmpackage->setLog(&dft_logger);
    qmpackage->setRunDir(qmpackage_work_dir);
    qmpackage->Initialize(_dftpackage_options);

    // if asked, prepare the input files
    if (_do_dft_input) {
      boost::filesystem::create_directories(qmpackage_work_dir);
      if (qmpackage->GuessRequested()) {
        if (_linkers.size() > 0) {
          throw std::runtime_error(
              "Error: You are using a linker and want "
              "to use a monomer guess for the dimer. These are mutually "
              "exclusive.");
        }

        XTP_LOG(Log::error, pLog)
            << "Guess requested, reading molecular orbitals" << std::flush;

        if (qmpackage->getPackageName() == "orca") {
          XTP_LOG(Log::info, pLog)
              << "Copying monomer .gbw files to pair folder" << std::flush;
          std::string gbwFileA =
              (arg_pathA / eqm_work_dir / "molecules" / frame_dir /
               (format("%1%_%2%%3%") % "molecule" % ID_A % ".gbw").str())
                  .generic_string();
          ;
          std::string gbwFileB =
              (arg_pathB / eqm_work_dir / "molecules" / frame_dir /
               (format("%1%_%2%%3%") % "molecule" % ID_B % ".gbw").str())
                  .generic_string();
          ;
          std::string gbwFileA_workdir =
              (boost::filesystem::path(qmpackage_work_dir) / "molA.gbw")
                  .generic_string();
          ;
          std::string gbwFileB_workdir =
              (boost::filesystem::path(qmpackage_work_dir) / "molB.gbw")
                  .generic_string();
          ;
          boost::filesystem::copy_file(
              gbwFileA, gbwFileA_workdir,
              boost::filesystem::copy_option::overwrite_if_exists);
          boost::filesystem::copy_file(
              gbwFileB, gbwFileB_workdir,
              boost::filesystem::copy_option::overwrite_if_exists);
        } else {
          Orbitals orbitalsB;
          Orbitals orbitalsA;

          try {
            XTP_LOG(Log::error, pLog)
                << "Reading MoleculeA from " << orbFileA << std::flush;
            orbitalsA.ReadFromCpt(orbFileA);
          } catch (std::runtime_error&) {
            SetJobToFailed(
                jres, pLog,
                "Do input: failed loading orbitals from " + orbFileA);
            return jres;
          }

          try {
            XTP_LOG(Log::error, pLog)
                << "Reading MoleculeB from " << orbFileB << std::flush;
            orbitalsB.ReadFromCpt(orbFileB);
          } catch (std::runtime_error&) {
            SetJobToFailed(
                jres, pLog,
                "Do input: failed loading orbitals from " + orbFileB);
            return jres;
          }
          XTP_LOG(Log::info, pLog)
              << "Constructing the guess for dimer orbitals" << std::flush;
          orbitalsAB.PrepareDimerGuess(orbitalsA, orbitalsB);
        }
      } else {
        XTP_LOG(Log::info, pLog)
            << "No Guess requested, starting from DFT starting Guess"
            << std::flush;
      }
      qmpackage->WriteInputFile(orbitalsAB);
    }

    if (_do_dft_run) {
      XTP_LOG(Log::error, pLog) << "Running DFT" << std::flush;
      bool _run_dft_status = qmpackage->Run();
      if (!_run_dft_status) {
        SetJobToFailed(jres, pLog, qmpackage->getPackageName() + " run failed");
        WriteLoggerToFile(work_dir + "/dft.log", dft_logger);
        return jres;
      }
    }

    if (_do_dft_parse) {
      bool parse_log_status = qmpackage->ParseLogFile(orbitalsAB);
      if (!parse_log_status) {
        SetJobToFailed(jres, pLog, "LOG parsing failed");
        return jres;
      }

      bool parse_orbitals_status = qmpackage->ParseMOsFile(orbitalsAB);

      if (!parse_orbitals_status) {
        SetJobToFailed(jres, pLog, "Orbitals parsing failed");
        return jres;
      }

    }  // end of the parse orbitals/log
    qmpackage->CleanUp();
    WriteLoggerToFile(work_dir + "/dft.log", dft_logger);
  } else {
    try {
      orbitalsAB.ReadFromCpt(orbFileAB);
    } catch (std::runtime_error&) {
      SetJobToFailed(jres, pLog,
                     "Do input: failed loading orbitals from " + orbFileAB);
      return jres;
    }
  }
  tools::Property job_summary;
  tools::Property& job_output = job_summary.add("output", "");
  if (_do_dftcoupling) {
    DFTcoupling dftcoupling;
    dftcoupling.setLogger(&pLog);
    dftcoupling.Initialize(_dftcoupling_options);
    Orbitals orbitalsB;
    Orbitals orbitalsA;

    try {
      orbitalsA.ReadFromCpt(orbFileA);
    } catch (std::runtime_error&) {
      SetJobToFailed(jres, pLog,
                     "Do input: failed loading orbitals from " + orbFileA);
      return jres;
    }

    try {
      orbitalsB.ReadFromCpt(orbFileB);
    } catch (std::runtime_error&) {
      SetJobToFailed(jres, pLog,
                     "Do input: failed loading orbitals from " + orbFileB);
      return jres;
    }
    try {
      dftcoupling.CalculateCouplings(orbitalsA, orbitalsB, orbitalsAB);
      dftcoupling.Addoutput(job_output, orbitalsA, orbitalsB);
    } catch (std::runtime_error& error) {
      std::string errormessage(error.what());
      SetJobToFailed(jres, pLog, errormessage);
      return jres;
    }
  }

  // do excited states calculation
  if (_do_gwbse) {
    try {
      XTP_LOG(Log::error, pLog) << "Running GWBSE" << std::flush;
      Logger gwbse_logger(Log::current_level);
      gwbse_logger.setMultithreading(false);
      gwbse_logger.setPreface(Log::info, (format("\nGWBSE INF ...")).str());
      gwbse_logger.setPreface(Log::error, (format("\nGWBSE ERR ...")).str());
      gwbse_logger.setPreface(Log::warning, (format("\nGWBSE WAR ...")).str());
      gwbse_logger.setPreface(Log::debug, (format("\nGWBSE DBG ...")).str());
      GWBSE gwbse = GWBSE(orbitalsAB);
      gwbse.setLogger(&gwbse_logger);
      gwbse.Initialize(_gwbse_options);
      gwbse.Evaluate();
      WriteLoggerToFile(work_dir + "/gwbse.log", gwbse_logger);
    } catch (std::runtime_error& error) {
      std::string errormessage(error.what());
      SetJobToFailed(jres, pLog, errormessage);
      return jres;
    }

  }  // end of excited state calculation, exciton data is in _orbitalsAB

  // calculate the coupling

  if (_do_bsecoupling) {
    XTP_LOG(Log::error, pLog) << "Running BSECoupling" << std::flush;
    BSECoupling bsecoupling;
    // orbitals must be loaded from a file
    if (!_do_gwbse) {
      try {
        orbitalsAB.ReadFromCpt(orbFileAB);
      } catch (std::runtime_error&) {
        SetJobToFailed(jres, pLog,
                       "Do input: failed loading orbitals from " + orbFileAB);
        return jres;
      }
    }

    Orbitals orbitalsB;
    Orbitals orbitalsA;

    try {
      orbitalsA.ReadFromCpt(orbFileA);
    } catch (std::runtime_error&) {
      SetJobToFailed(jres, pLog,
                     "Do input: failed loading orbitals from " + orbFileA);
      return jres;
    }

    try {
      orbitalsB.ReadFromCpt(orbFileB);
    } catch (std::runtime_error&) {
      SetJobToFailed(jres, pLog,
                     "Do input: failed loading orbitals from " + orbFileB);
      return jres;
    }

    try {
      Logger bsecoupling_logger(Log::current_level);
      bsecoupling_logger.setMultithreading(false);
      bsecoupling_logger.setPreface(Log::info,
                                    (format("\nBSECOU INF ...")).str());
      bsecoupling_logger.setPreface(Log::error,
                                    (format("\nBSECOU ERR ...")).str());
      bsecoupling_logger.setPreface(Log::warning,
                                    (format("\nBSECOU WAR ...")).str());
      bsecoupling_logger.setPreface(Log::debug,
                                    (format("\nBSECOU DBG ...")).str());
      bsecoupling.setLogger(&bsecoupling_logger);
      bsecoupling.Initialize(_bsecoupling_options);
      bsecoupling.CalculateCouplings(orbitalsA, orbitalsB, orbitalsAB);
      bsecoupling.Addoutput(job_output, orbitalsA, orbitalsB);
      WriteLoggerToFile(work_dir + "/bsecoupling.log", bsecoupling_logger);
    } catch (std::runtime_error& error) {
      std::string errormessage(error.what());
      SetJobToFailed(jres, pLog, errormessage);
      return jres;
    }
  }

  tools::PropertyIOManipulator iomXML(tools::PropertyIOManipulator::XML, 1, "");
  std::stringstream sout;
  sout << iomXML << job_summary;
  XTP_LOG(Log::error, pLog) << TimeStamp() << " Finished evaluating pair "
                            << ID_A << ":" << ID_B << std::flush;
  if (_store_dft || _store_gw) {
    boost::filesystem::create_directories(orb_dir);
    XTP_LOG(Log::error, pLog)
        << "Saving orbitals to " << orbFileAB << std::flush;
    if (!_store_dft) {
      orbitalsAB.MOs().clear();
    }
    if (!_store_gw) {
      orbitalsAB.QPdiag().clear();
      orbitalsAB.QPpertEnergies().resize(0);
    }
    orbitalsAB.WriteToCpt(orbFileAB);
  } else {
    XTP_LOG(Log::error, pLog)
        << "Orb file is not saved according to options " << std::flush;
  }

  jres.setOutput(job_summary);
  jres.setStatus(Job::COMPLETE);

  return jres;
}

void IQM::WriteJobFile(const Topology& top) {

  std::cout << std::endl
            << "... ... Writing job file " << _jobfile << std::flush;
  std::ofstream ofs;
  ofs.open(_jobfile, std::ofstream::out);
  if (!ofs.is_open()) {
    throw std::runtime_error("\nERROR: bad file handle: " + _jobfile);
  }

  const QMNBList& nblist = top.NBList();

  Index jobCount = 0;
  if (nblist.size() == 0) {
    std::cout << std::endl
              << "... ... No pairs in neighbor list, skip." << std::flush;
    return;
  }

  ofs << "<jobs>" << std::endl;
  std::string tag = "";

  for (const QMPair* pair : nblist) {
    if (pair->getType() == QMPair::Excitoncl) {
      continue;
    }
    Index id1 = pair->Seg1()->getId();
    std::string name1 = pair->Seg1()->getType();
    Index id2 = pair->Seg2()->getId();
    std::string name2 = pair->Seg2()->getType();
    Index id = jobCount;
    tools::Property Input;
    tools::Property& pInput = Input.add("input", "");
    tools::Property& pSegmentA =
        pInput.add("segment", boost::lexical_cast<std::string>(id1));
    pSegmentA.setAttribute<std::string>("type", name1);
    pSegmentA.setAttribute<Index>("id", id1);
    tools::Property& pSegmentB =
        pInput.add("segment", boost::lexical_cast<std::string>(id2));
    pSegmentB.setAttribute<std::string>("type", name2);
    pSegmentB.setAttribute<Index>("id", id2);
    Job job(id, tag, Input, Job::AVAILABLE);
    job.ToStream(ofs);
    jobCount++;
  }
  // CLOSE STREAM
  ofs << "</jobs>" << std::endl;
  ofs.close();
  std::cout << std::endl
            << "... ... In total " << jobCount << " jobs" << std::flush;
  return;
}

double IQM::GetDFTCouplingFromProp(tools::Property& dftprop, Index stateA,
                                   Index stateB) {
  double J = 0;
  bool found = false;
  for (tools::Property* state : dftprop.Select("coupling")) {
    Index state1 = state->getAttribute<Index>("levelA");
    Index state2 = state->getAttribute<Index>("levelB");
    if (state1 == stateA && state2 == stateB) {
      J = state->getAttribute<double>("j") * tools::conv::ev2hrt;
      found = true;
      break;
    }
  }
  if (found) {
    return J * J;
  } else {
    return -1;
  }
}

double IQM::GetBSECouplingFromProp(tools::Property& bseprop,
                                   const QMState& stateA,
                                   const QMState& stateB) {
  double J = 0;
  std::string algorithm = bseprop.getAttribute<std::string>("algorithm");
  bool found = false;
  for (tools::Property* state : bseprop.Select("coupling")) {
    QMState state1;
    state1.FromString(state->getAttribute<std::string>("stateA"));
    QMState state2;
    state2.FromString(state->getAttribute<std::string>("stateB"));
    if (state1 == stateA && state2 == stateB) {
      J = state->getAttribute<double>(algorithm) * tools::conv::ev2hrt;
      found = true;
      break;
    }
  }
  if (found) {
    return J * J;
  } else {
    return -1;
  }
}

QMState IQM::GetElementFromMap(const std::map<std::string, QMState>& elementmap,
                               const std::string& elementname) const {
  QMState state;
  try {
    state = elementmap.at(elementname);
  } catch (std::out_of_range&) {
    std::string errormessage =
        "Map does not have segment of type: " + elementname;
    errormessage += "\n segments in map are:";
    for (const auto& s : elementmap) {
      errormessage += "\n\t" + s.first;
    }
    throw std::runtime_error(errormessage);
  }
  return state;
}

void IQM::ReadJobFile(Topology& top) {
  // gets the neighborlist from the topology
  QMNBList& nblist = top.NBList();
  Index number_of_pairs = nblist.size();
  Index dft_h = 0;
  Index dft_e = 0;
  Index bse_s = 0;
  Index bse_t = 0;
  Index incomplete_jobs = 0;
  Logger log;
  log.setReportLevel(Log::current_level);

  tools::Property xml;
  // load the QC results in a vector indexed by the pair ID
  xml.LoadFromXML(_jobfile);
  std::vector<tools::Property*> jobProps = xml.Select("jobs.job");
  std::vector<tools::Property*> records =
      std::vector<tools::Property*>(nblist.size() + 1, nullptr);

  // loop over all jobs = pair records in the job file
  for (tools::Property* job : jobProps) {
    if (!job->exists("status")) {
      throw std::runtime_error(
          "Jobfile is malformed. <status> tag missing on job.");
    }
    if (job->get("status").as<std::string>() != "COMPLETE" ||
        !job->exists("output")) {
      incomplete_jobs++;
      continue;
    }

    // get the output records
    tools::Property poutput = job->get("output");
    // job file is stupid, because segment ids are only in input have to get
    // them out l
    std::vector<tools::Property*> segmentprobs = job->Select("input.segment");
    std::vector<Index> id;
    for (tools::Property* segment : segmentprobs) {
      id.push_back(segment->getAttribute<Index>("id"));
    }
    if (id.size() != 2) {
      throw std::runtime_error(
          "Getting pair ids from jobfile failed, check jobfile.");
    }

    Index idA = id[0];
    Index idB = id[1];

    // segments which correspond to these ids
    Segment& segA = top.getSegment(idA);
    Segment& segB = top.getSegment(idB);
    // pair that corresponds to the two segments
    QMPair* qmp = nblist.FindPair(&segA, &segB);
    // output using logger

    if (qmp == nullptr) {  // there is no pair in the neighbor list with this
                           // name
      XTP_LOG(Log::error, log)
          << "No pair " << idA << ":" << idB
          << " found in the neighbor list. Ignoring" << std::flush;
    } else {
      records[qmp->getId()] = &(job->get("output"));
    }

  }  // finished loading from the file

  for (QMPair* pair : top.NBList()) {

    if (records[pair->getId()] == nullptr) {
      continue;  // skip pairs which are not in the jobfile
    }

    const Segment* segmentA = pair->Seg1();
    const Segment* segmentB = pair->Seg2();

    QMPair::PairType ptype = pair->getType();
    if (ptype != QMPair::PairType::Hopping) {
      XTP_LOG(Log::error, log) << "WARNING Pair " << pair->getId()
                               << " is not of any of the "
                                  "Hopping type. Skipping pair"
                               << std::flush;
      continue;
    }

    tools::Property* pair_property = records[pair->getId()];

    if (pair_property->exists("dftcoupling")) {
      tools::Property& dftprop = pair_property->get("dftcoupling");
      Index homoA = dftprop.getAttribute<Index>("homoA");
      Index homoB = dftprop.getAttribute<Index>("homoB");
      QMStateType hole = QMStateType(QMStateType::Hole);
      if (dftprop.exists(hole.ToLongString())) {
        tools::Property& holes = dftprop.get(hole.ToLongString());
        QMState stateA = GetElementFromMap(_hole_levels, segmentA->getType());
        QMState stateB = GetElementFromMap(_hole_levels, segmentB->getType());
        Index levelA = homoA - stateA.StateIdx();  // h1 is is homo;
        Index levelB = homoB - stateB.StateIdx();
        double J2 = GetDFTCouplingFromProp(holes, levelA, levelB);
        if (J2 >= 0) {
          pair->setJeff2(J2, hole);
          dft_h++;
        }
      }
      QMStateType electron = QMStateType(QMStateType::Electron);
      if (dftprop.exists(electron.ToLongString())) {
        tools::Property& electrons = dftprop.get(electron.ToLongString());
        QMState stateA =
            GetElementFromMap(_electron_levels, segmentA->getType());
        QMState stateB =
            GetElementFromMap(_electron_levels, segmentB->getType());
        Index levelA = homoA + 1 + stateA.StateIdx();  // e1 is lumo;
        Index levelB = homoB + 1 + stateB.StateIdx();
        double J2 = GetDFTCouplingFromProp(electrons, levelA, levelB);
        if (J2 >= 0) {
          pair->setJeff2(J2, electron);
          dft_e++;
        }
      }
    }
    if (pair_property->exists("bsecoupling")) {
      tools::Property& bseprop = pair_property->get("bsecoupling");
      QMStateType singlet = QMStateType(QMStateType::Singlet);
      if (bseprop.exists(singlet.ToLongString())) {
        tools::Property& singlets = bseprop.get(singlet.ToLongString());
        QMState stateA =
            GetElementFromMap(_singlet_levels, segmentA->getType());
        QMState stateB =
            GetElementFromMap(_singlet_levels, segmentB->getType());
        double J2 = GetBSECouplingFromProp(singlets, stateA, stateB);
        if (J2 >= 0) {
          pair->setJeff2(J2, singlet);
          bse_s++;
        }
      }
      QMStateType triplet = QMStateType(QMStateType::Triplet);
      if (bseprop.exists(triplet.ToLongString())) {
        tools::Property& triplets = bseprop.get(triplet.ToLongString());
        QMState stateA =
            GetElementFromMap(_triplet_levels, segmentA->getType());
        QMState stateB =
            GetElementFromMap(_triplet_levels, segmentB->getType());
        double J2 = GetBSECouplingFromProp(triplets, stateA, stateB);
        if (J2 >= 0) {
          pair->setJeff2(J2, triplet);
          bse_t++;
        }
      }
    }
  }

  XTP_LOG(Log::error, log) << "Pairs [total:updated(e,h,s,t)] "
                           << number_of_pairs << ":(" << dft_e << "," << dft_h
                           << "," << bse_s << "," << bse_t
                           << ") Incomplete jobs: " << incomplete_jobs << "\n"
                           << std::flush;
  return;
}
}  // namespace xtp
}  // namespace votca
