/*
 * Copyright 2020 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AGENT_BASED_EPIDEMIC_SIM_CORE_MICRO_EXPOSURE_GENERATOR_H_
#define AGENT_BASED_EPIDEMIC_SIM_CORE_MICRO_EXPOSURE_GENERATOR_H_

#include <array>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "absl/random/random.h"
#include "absl/time/time.h"
#include "agent_based_epidemic_sim/core/constants.h"
#include "agent_based_epidemic_sim/core/event.h"
#include "agent_based_epidemic_sim/core/exposure_generator.h"
#include "agent_based_epidemic_sim/port/logging.h"

namespace abesim {

class MicroExposureGenerator : public ExposureGenerator {
 public:
  explicit MicroExposureGenerator(
      const std::vector<std::vector<float>>& proximity_trace_distribution) {
    LoadProximityTraceDistribution(proximity_trace_distribution);
  }
  explicit MicroExposureGenerator() {}

  virtual ~MicroExposureGenerator() = default;
  // TODO: Deprecate this method.
  // Generate microexposures based on a uniform sampling.
  Exposure Generate(const absl::Time start_time, const absl::Duration duration,
                    const float infectivity,
                    const float symptom_factor) override;
  // Generate a pair of Exposure objects representing a single contact between
  // two hosts. The first and second values in the infectivity and
  // symptom_factor pairs will correspond to the first and second Exposures
  // returned, respectively.
  std::pair<Exposure, Exposure> Generate(
      const absl::Time start_time, const absl::Duration duration,
      const std::pair<float, float> infectivity,
      const std::pair<float, float> symptom_factor) override;

 private:
  // Draws a proximity trace from an in-memory, non-parametric distribution.
  // Represents the distances between two hosts at fixed intervals.
  std::array<float, kMaxTraceLength> DrawProximityTrace();

  // Generates a proximity trace by drawing from a uniform distribution.
  // Represents the distances between two hosts at fixed intervals.
  std::array<float, kMaxTraceLength> GenerateProximityTrace();

  // Load fixed length proximity traces from non-parametric distribution.
  void LoadProximityTraceDistribution(
      const std::vector<std::vector<float>>& proximity_trace_distribution) {
    CHECK(!proximity_trace_distribution.empty())
        << "proximity_trace_distribution_ cannot be empty!";
    std::array<float, kMaxTraceLength> fixed_length_proximity_trace;
    uint8 i = 0;
    for (const auto& proximity_trace : proximity_trace_distribution) {
      fixed_length_proximity_trace.fill(std::numeric_limits<float>::max());
      for (i = 0; i < proximity_trace.size(); i++) {
        fixed_length_proximity_trace[i] = proximity_trace[i];
      }

      proximity_trace_distribution_.push_back(fixed_length_proximity_trace);
    }
  }

  std::vector<std::array<float, kMaxTraceLength>> proximity_trace_distribution_;
  absl::BitGen gen_;
};

}  // namespace abesim

#endif  // AGENT_BASED_EPIDEMIC_SIM_CORE_MICRO_EXPOSURE_GENERATOR_H_
