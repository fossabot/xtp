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

// Standard includes
#include <string>

// Local VOTCA includes
#include "votca/xtp/calculatorfactory.h"
#include "votca/xtp/stateapplication.h"

using namespace votca;

class XtpRun : public xtp::StateApplication {
 public:
  std::string ProgramName() final { return "xtp_run"; }

  void HelpText(std::ostream& out) final {
    out << "Runs excitation/charge transport calculators\n";
  }

  void HelpText(){};

  void Initialize() final;
  bool EvaluateOptions() final;

 private:
  // void    PrintDescription(string name, HelpOutputType _help_output_type);
};

namespace propt = boost::program_options;

void XtpRun::Initialize() {
  xtp::Calculatorfactory::RegisterAll();
  xtp::StateApplication::Initialize();

  AddProgramOptions("Calculator")("execute,e", propt::value<std::string>(),
                                  "Name of calculator to run");
  AddProgramOptions("Calculator")("list,l", "Lists all available calculators");
  AddProgramOptions("Calculator")("description,d", propt::value<std::string>(),
                                  "Short description of a calculator");
}

bool XtpRun::EvaluateOptions() {

  std::string helpdir = "xtp/xml";
  if (OptionsMap().count("list")) {
    std::cout << "Available XTP calculators:\n";
    for (const auto& calc : xtp::Calculators().getObjects()) {
      PrintDescription(std::cout, calc.first, helpdir, Application::HelpShort);
    }
    StopExecution();
    return true;
  }

  if (OptionsMap().count("description")) {
    CheckRequired("description", "no calculator is given");
    tools::Tokenizer tok(OptionsMap()["description"].as<std::string>(),
                         " ,\n\t");
    // loop over the names in the description string
    for (const std::string& n : tok) {
      // loop over calculators
      bool printerror = true;
      for (const auto& calc : xtp::Calculators().getObjects()) {

        if (n.compare(calc.first) == 0) {
          PrintDescription(std::cout, calc.first, helpdir,
                           Application::HelpLong);
          printerror = false;
          break;
        }
      }
      if (printerror) {
        std::cout << "Calculator " << n << " does not exist\n";
      }
    }
    StopExecution();
    return true;
  }

  xtp::StateApplication::EvaluateOptions();
  CheckRequired("execute", "Nothing to do here: Abort.");

  tools::Tokenizer calcs(OptionsMap()["execute"].as<std::string>(), " ,\n\t");
  std::vector<std::string> calc_string = calcs.ToVector();
  if (calc_string.size() != 1) {
    throw std::runtime_error(
        "You can only run one calculator at the same time.");
  }
  bool found_calc = false;
  for (const auto& calc : xtp::Calculators().getObjects()) {

    if (calc_string[0].compare(calc.first) == 0) {
      xtp::StateApplication::SetCalculator(
          xtp::Calculators().Create(calc_string[0]));
      found_calc = true;
      break;
    }
  }
  if (!found_calc) {
    std::cout << "Calculator " << calc_string[0] << " does not exist\n";
    StopExecution();
  } else {
    _options.LoadFromXML(_op_vm["options"].as<std::string>());
  }
  return true;
}

int main(int argc, char** argv) {

  XtpRun xtprun;
  return xtprun.Exec(argc, argv);
}
