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
#ifndef VOTCA_XTP_QMSANDBOX_H
#define VOTCA_XTP_QMSANDBOX_H

// Local VOTCA includes
#include "votca/xtp/aobasis.h"
#include "votca/xtp/aomatrix.h"
#include "votca/xtp/logger.h"
#include "votca/xtp/qmpackagefactory.h"
#include "votca/xtp/qmtool.h"

namespace votca {
namespace xtp {

class QMSandbox : public QMTool {
 public:
  QMSandbox() = default;
  ~QMSandbox() final = default;

  std::string Identify() final { return "qmsandbox"; }

  void ParseUserOptions(const tools::Property& user_options) final;
  bool Run() final;

 private:
  std::string _orbfile;
};

void QMSandbox::ParseUserOptions(const tools::Property& user_options) {}

bool QMSandbox::Run() { return true; }

}  // namespace xtp
}  // namespace votca

#endif  // VOTCA_XTP_QMSANDBOX_H
