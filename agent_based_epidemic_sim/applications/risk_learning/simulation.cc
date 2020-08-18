// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "agent_based_epidemic_sim/applications/risk_learning/simulation.h"

#include <memory>
#include <vector>

#include "absl/container/fixed_array.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "agent_based_epidemic_sim/agent_synthesis/population_profile.pb.h"
#include "agent_based_epidemic_sim/applications/risk_learning/config.pb.h"
#include "agent_based_epidemic_sim/applications/risk_learning/risk_score.h"
#include "agent_based_epidemic_sim/core/agent.h"
#include "agent_based_epidemic_sim/core/risk_score.h"
#include "agent_based_epidemic_sim/core/simulation.h"
#include "agent_based_epidemic_sim/port/time_proto_util.h"
#include "riegeli/bytes/fd_reader.h"
#include "riegeli/bytes/file_reader.h"
#include "riegeli/records/record_reader.h"

namespace abesim {
namespace {

using RiegeliBytesSource = riegeli::FdReader<>;

class LearningRiskScoreGenerator : public RiskScoreGenerator {
 public:
  LearningRiskScoreGenerator(const TracingPolicyProto& policy,
                             LocationTypeFn location)
      : policy_(policy), location_(std::move(location)) {}
  std::unique_ptr<RiskScore> NextRiskScore() override {
    return *CreateLearningRiskScore(policy_, location_);
  }

 private:
  const TracingPolicyProto policy_;
  const LocationTypeFn location_;
};

}  // namespace

util::Status RunSimulation(const RiskLearningSimulationConfig& config,
                           int num_workers) {
  // Read in locations.
  std::vector<std::unique_ptr<Location>> locations;
  {
    riegeli::RecordReader<RiegeliBytesSource> reader(
        std::forward_as_tuple(config.location_file(), "r"));
    LocationProto proto;
    while (reader.ReadRecord(proto)) {
      // TODO: Build locations from proto.
    }
    absl::Status status = reader.status();
    if (!status.ok()) return status;
    reader.Close();
  }

  // Read in agents.
  std::vector<std::unique_ptr<Agent>> agents;
  {
    riegeli::RecordReader<RiegeliBytesSource> reader(
        std::forward_as_tuple(config.agent_file(), "r"));
    AgentProto proto;
    while (reader.ReadRecord(proto)) {
      // TODO: Build agents from proto.
    }
    absl::Status status = reader.status();
    if (!status.ok()) return status;
    reader.Close();
  }

  auto time_or = DecodeGoogleApiProto(config.init_time());
  CHECK(time_or.ok());
  const auto init_time = time_or.value();
  auto step_size_or = DecodeGoogleApiProto(config.step_size());
  CHECK(step_size_or.ok());
  const auto step_size = step_size_or.value();

  auto sim = num_workers > 1
                 ? ParallelSimulation(init_time, std::move(agents),
                                      std::move(locations), num_workers)
                 : SerialSimulation(init_time, std::move(agents),
                                    std::move(locations));
  sim->Step(config.steps(), step_size);
  return absl::OkStatus();
}

}  // namespace abesim
