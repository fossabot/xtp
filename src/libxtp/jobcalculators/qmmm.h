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
#ifndef VOTCA_XTP_QMMM_H
#define VOTCA_XTP_QMMM_H

// Local VOTCA includes
#include "votca/xtp/parallelxjobcalc.h"

namespace votca {
namespace xtp {

/**
 * \brief QM/MM with different regions around
 *
 * Calculates properties of different regions inside a multiregion calculation
 *
 * Callname: qmmm
 */

class QMMM : public ParallelXJobCalc<std::vector<Job> > {
 public:
  std::string Identify() final { return "qmmm"; }
  void WriteJobFile(const Topology &top) final;
  void ReadJobFile(Topology &top) final;

 protected:
  void ParseUserOptions(const tools::Property &user_options) final;
  Job::JobResult EvalJob(const Topology &top, Job &job,
                         QMThread &opThread) final;

 private:
  bool hasQMRegion() const;
  tools::Property _regions_def;

  Index _max_iterations;
  bool _print_regions_pdb = false;

  bool _write_parse = true;
  std::vector<QMState> _states;
};

}  // namespace xtp
}  // namespace votca
#endif  // VOTCA_XTP_QMMM_H
