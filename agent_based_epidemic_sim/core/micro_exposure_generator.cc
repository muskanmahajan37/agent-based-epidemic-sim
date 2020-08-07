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

#include "agent_based_epidemic_sim/core/micro_exposure_generator.h"

#include <algorithm>
#include <iostream>
#include <limits>

#include "absl/random/distributions.h"
#include "absl/time/time.h"
#include "agent_based_epidemic_sim/core/constants.h"
#include "agent_based_epidemic_sim/core/event.h"
#include "agent_based_epidemic_sim/core/parameter_distribution.pb.h"

namespace abesim {

Exposure MicroExposureGenerator::Generate(absl::Time start_time,  // unused
                                          absl::Duration duration,
                                          float infectivity,
                                          float symptom_factor) {
  return {
      .duration = duration,
      .infectivity = infectivity,
      .symptom_factor = symptom_factor,
  };
}

std::pair<Exposure, Exposure> MicroExposureGenerator::Generate(
    absl::Time start_time,    // unused
    absl::Duration duration,  // unused
    std::pair<float, float> infectivity,
    std::pair<float, float> symptom_factor) {
  const std::array<float, kMaxTraceLength> proximity_trace =
      (proximity_trace_distribution_.empty()) ? GenerateProximityTrace()
                                              : DrawProximityTrace();

  const uint8 trace_length = std::count_if(
      proximity_trace.begin(), proximity_trace.end(), [](float proximity) {
        return proximity < std::numeric_limits<float>::max();
      });
  const absl::Duration trace_duration = trace_length * kProximityTraceInterval;

  return {{
              .duration = trace_duration,
              .proximity_trace = proximity_trace,
              .infectivity = infectivity.first,
              .symptom_factor = symptom_factor.first,
          },
          {
              .duration = trace_duration,
              .proximity_trace = proximity_trace,
              .infectivity = infectivity.second,
              .symptom_factor = symptom_factor.second,
          }};
}

std::array<float, kMaxTraceLength>
MicroExposureGenerator::GenerateProximityTrace() {
  std::array<float, kMaxTraceLength> full_length_proximity_trace;
  full_length_proximity_trace.fill(std::numeric_limits<float>::max());

  uint8 proximity_trace_length = absl::Uniform<uint8>(gen_, 1, kMaxTraceLength);
  for (uint8 i = 0; i < proximity_trace_length; i++) {
    full_length_proximity_trace[i] = absl::Uniform<float>(gen_, 0.0f, 10.0f);
  }
  return full_length_proximity_trace;
}

std::array<float, kMaxTraceLength>
MicroExposureGenerator::DrawProximityTrace() {
  return proximity_trace_distribution_[absl::Uniform<int>(
      gen_, 0, proximity_trace_distribution_.size() - 1)];
}

}  // namespace abesim
