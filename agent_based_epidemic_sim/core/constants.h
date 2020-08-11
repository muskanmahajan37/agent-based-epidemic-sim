#ifndef AGENT_BASED_EPIDEMIC_SIM_CORE_CONSTANTS_H_
#define AGENT_BASED_EPIDEMIC_SIM_CORE_CONSTANTS_H_

#include <array>
#include <vector>

#include "absl/time/time.h"
#include "agent_based_epidemic_sim/core/integral_types.h"

namespace abesim {
// Maximum numnber of proximity "scans" for a given proximity trace. This
// effectively caps the maximum length of a given exposure. If a longer exposure
// takes place, multiple Exposure objects should be created.
inline const uint8 kMaxTraceLength = 20;

// Number of days after initial infection that the host is considered to still
// be infectious.
inline const uint8 kMaxDaysAfterInfection = 14;

inline const absl::Duration kProximityTraceInterval = absl::Minutes(5);

// TODO: Dummy data for now. Load this into memory from file on the
// ExposureGenerator.
inline const std::vector<std::vector<float>> kNonParametricTraceDistribution = {
    {9.06893969},
    {2.17069263, 1.34896927, 1.25818902},
    {0.82138789, 6.22004292},
    {2.05025381},
    {3.24773771},
    {4.47790356},
    {8.76846823},
    {2.46735085, 1.18301727, 2.31253068, 0.6587179},
    {5.40369733, 5.72922553, 4.00336149},
    {3.90483315, 1.13448638, 1.52669001, 4.54364751, 0.00735043, 1.91172425,
     0.29878502, 1.03358558, 3.7044072, 1.87280156, 1.3717239, 2.72108035,
     0.81446531, 3.54814224, 1.90291171, 0.38753284, 1.42542508}};

// This table maps days since infection to an infectivity factor. It is computed
// using a Gamma CDF distribution and the parameters are defined in
// (broken link).
inline const std::array<float, kMaxDaysAfterInfection + 1> kInfectivityArray = {
    0.9706490058950823,    1.7351078930710577,    1.2019044766404108,
    0.6227475415319733,    0.2804915619270876,    0.1163222813140352,
    0.04568182435881363,   0.017259874839000156,  0.006335824965724157,
    0.002274361283270409,  0.0008019921659041529, 0.00027871327592632333,
    0.0000956941785834608, 0.0000325214311858168, 0};

}  // namespace abesim

#endif  // AGENT_BASED_EPIDEMIC_SIM_CORE_CONSTANTS_H_
