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
/// For an earlier history see ctp repo commit
/// 77795ea591b29e664153f9404c8655ba28dc14e9

#pragma once
#ifndef VOTCA_XTP_QMTOOL_H
#define VOTCA_XTP_QMTOOL_H

// Third party includes
#include <boost/format.hpp>

// VOTCA includes
#include <votca/tools/calculator.h>
#include <votca/tools/property.h>

#include "eigen.h"
namespace votca {
namespace xtp {

class QMTool : public tools::Calculator {
 public:
  QMTool() = default;
  ~QMTool() override = default;

  std::string Identify() override = 0;
  void Initialize(const tools::Property &options) final {
    tools::Property user_options =
        LoadDefaultsAndUpdateWithUserOptions("xtp", options);
    ParseUserOptions(user_options);
  }

  bool Evaluate() {
    OPENMP::setMaxThreads(_nThreads);
    return Run();
  }

  void setJobname(const std::string &name) { _job_name = name; }

 protected:
  virtual bool Run() = 0;
  virtual void ParseUserOptions(const tools::Property &options) = 0;
  const std::string &Jobname() const { return _job_name; }

 private:
  std::string _job_name = "votca";
};

}  // namespace xtp
}  // namespace votca

#endif  // VOTCA_XTP_QMTOOL_H
