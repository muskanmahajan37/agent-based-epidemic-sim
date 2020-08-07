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

#ifndef AGENT_BASED_EPIDEMIC_SIM_CORE_HAZARD_AGGREGATED_TRANSMISSION_MODEL_H_
#define AGENT_BASED_EPIDEMIC_SIM_CORE_HAZARD_AGGREGATED_TRANSMISSION_MODEL_H_

#include <cmath>

#include "absl/random/random.h"
#include "agent_based_epidemic_sim/core/constants.h"
#include "agent_based_epidemic_sim/core/event.h"
#include "agent_based_epidemic_sim/core/transmission_model.h"

namespace abesim {

// Represents a proximity trace between two hosts made up of a scan distance
// every kScanFrequency and health information associated with the
// contact.
struct ExposureWindow {
  absl::Duration duration;

  std::array<uint8, kMaxScanInstancesPerExposureWindow> scan_distances;
};

// TODO: Summarize the transmission model more concisely when it is
// stable.
// TODO: Currently this TransmissionModel should only be used in
// conjunction with Graph simulator. Extend support to Location event simulator.
// Models transmission between hosts as a sum of doses where each dose computes
// a hazard as a function of (duration, distance, infectivity, symptom_factor,
// location_transmissibility, susceptibility).
class HazardAggregatedTransmissionModel : public TransmissionModel {
 public:
  // TODO: Fix variable naming. For now easiest to just mirror
  // python notebook until stable.
  explicit HazardAggregatedTransmissionModel(const float lambda)
      : lambda_(lambda) {
    risk_at_distance_function_ = [](float proximity) {
      float a = 1.5;
      float b = 6.6;
      return 1 - 1 / (1 + std::exp(-a * proximity + b));
    };
  }

  explicit HazardAggregatedTransmissionModel(
      const float lambda, std::function<float(float)> risk_at_distance_function)
      : lambda_(lambda),
        risk_at_distance_function_(risk_at_distance_function) {}

  // Computes the infection outcome given exposures.
  HealthTransition GetInfectionOutcome(
      absl::Span<const Exposure* const> exposures) override;

 private:
  // Computes a "viral dose" which is used directly in computing the probability
  // of infection for a given Exposure.
  float ComputeDose(float distance, const absl::Duration& duration,
                    const Exposure* exposure);

  // TODO: Link out to actual papers or some other authoritative
  // source.
  // Typical values of lambda_ come from:
  //  Amanda Wilson paper: 2.2x10e-3
  //  Mark Briers paper: 0.6 / 15
  const float lambda_;
  absl::BitGen gen_;

  // Generates a risk dosage for a given distance.
  std::function<float(float)> risk_at_distance_function_;
};

}  // namespace abesim

#endif  // AGENT_BASED_EPIDEMIC_SIM_CORE_HAZARD_AGGREGATED_TRANSMISSION_MODEL_H_
