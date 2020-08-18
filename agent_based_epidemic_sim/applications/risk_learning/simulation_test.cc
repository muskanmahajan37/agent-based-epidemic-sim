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

#include "absl/flags/flag.h"
#include "absl/status/status.h"
#include "absl/strings/str_split.h"
#include "absl/time/time.h"
#include "agent_based_epidemic_sim/agent_synthesis/population_profile.pb.h"
#include "agent_based_epidemic_sim/applications/risk_learning/config.pb.h"
#include "agent_based_epidemic_sim/core/parse_text_proto.h"
#include "agent_based_epidemic_sim/core/risk_score.h"
#include "agent_based_epidemic_sim/port/file_utils.h"
#include "agent_based_epidemic_sim/port/status_matchers.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "riegeli/bytes/file_writer.h"
#include "riegeli/records/record_writer.h"

namespace abesim {
namespace {
using RiegeliBytesSink = riegeli::FdWriter<>;

constexpr char kConfigPath[] =
    "agent_based_epidemic_sim/applications/risk_learning/"
    "testdata/config.pbtxt";

void FillLocation(LocationProto& location, int64 uuid,
                  LocationReference::Type type) {
  LocationReference* ref = location.mutable_reference();
  ref->set_uuid(uuid);
  ref->set_type(type);
  GraphParamLocation* params = location.mutable_graph_params();
  params->set_degree(10);
  params->set_nodes(100);
}
void FillAgent(AgentProto& agent, int64 uuid, HealthState::State initial,
               const std::vector<LocationProto>& locations) {
  agent.set_uuid(uuid);
  agent.set_population_profile_id(1);
  agent.set_initial_health_state(initial);
  for (const LocationProto& location : locations) {
    *agent.add_locations() = location.reference();
  }
}

TEST(SimulationTest, RunsSimulation) {
  const std::string config_path = absl::StrCat("./", "/", kConfigPath);
  std::string contents;
  PANDEMIC_ASSERT_OK(file::GetContents(config_path, &contents));
  RiskLearningSimulationConfig config =
      ParseTextProtoOrDie<RiskLearningSimulationConfig>(contents);

  // Write some locations to a file.
  std::vector<LocationProto> locations(2);
  FillLocation(locations[0], 1000, LocationReference::HOUSEHOLD);
  FillLocation(locations[1], 1001, LocationReference::BUSINESS);
  config.set_location_file(
      absl::StrCat(getenv("TEST_TMPDIR"), "/", "locations"));
  {
    riegeli::RecordWriter<RiegeliBytesSink> writer(
        std::forward_as_tuple(config.location_file(), "w"));
    for (const LocationProto location : locations) {
      CHECK(writer.WriteRecord(location));
    }
    CHECK(writer.Close());
  }

  // Write some agents to a file.
  std::vector<AgentProto> agents(100);
  for (int i = 0; i < agents.size(); ++i) {
    FillAgent(agents[i], i + 1,
              i == 0 ? HealthState::INFECTIOUS : HealthState::SUSCEPTIBLE,
              locations);
  }
  config.set_agent_file(absl::StrCat(getenv("TEST_TMPDIR"), "/", "agents"));
  {
    riegeli::RecordWriter<RiegeliBytesSink> writer(
        std::forward_as_tuple(config.agent_file(), "w"));
    for (const AgentProto agent : agents) {
      CHECK(writer.WriteRecord(agent));
    }
    CHECK(writer.Close());
  }

  absl::Status status = RunSimulation(config, /*num_workers=*/1);
  EXPECT_TRUE(status.ok()) << status;
}

}  // namespace
}  // namespace abesim
