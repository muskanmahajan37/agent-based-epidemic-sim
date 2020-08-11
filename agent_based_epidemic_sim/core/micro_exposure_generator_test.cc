#include "agent_based_epidemic_sim/core/micro_exposure_generator.h"

#include "agent_based_epidemic_sim/core/micro_exposure_generator_builder.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace abesim {
namespace {

const std::pair<float, float> kInfectivityPair = {1.0, 0.0};
const std::pair<float, float> kSymptomFactorPair = {1.0, 0.0};
const std::vector<std::vector<float>> kDistribution = {{1.0f}};
const std::vector<std::vector<float>> kEmptyDistribution = {};

TEST(MicroExposureGeneratorTest, NoMicroExposuresWithNoDistribution) {
  MicroExposureGeneratorBuilder meg_builder;
  std::unique_ptr<ExposureGenerator> generator = meg_builder.Build();
  const float kInfectivity = 1.0;
  const float kSymptomFactor = 1.0;

  Exposure exposure = generator->Generate(absl::UnixEpoch(), absl::Minutes(100),
                                          kInfectivity, kSymptomFactor);

  EXPECT_EQ(exposure.infectivity, kInfectivity);
  EXPECT_EQ(exposure.symptom_factor, kSymptomFactor);
}

TEST(MicroExposureGeneratorTest, DiesWithEmptyDistribution) {
  MicroExposureGeneratorBuilder meg_builder;
  EXPECT_DEATH(meg_builder.Build(kEmptyDistribution), "cannot be empty!");
}

TEST(MicroExposureGeneratorTest, CorrectlyDrawsFromDistribution) {
  MicroExposureGeneratorBuilder meg_builder;
  std::unique_ptr<ExposureGenerator> generator =
      meg_builder.Build(kDistribution);

  std::pair<Exposure, Exposure> exposures =
      generator->Generate(absl::UnixEpoch(), absl::ZeroDuration(),
                          kInfectivityPair, kSymptomFactorPair);

  std::array<float, kMaxTraceLength> proximity_trace =
      exposures.first.proximity_trace;
  EXPECT_EQ(proximity_trace[0], 1.0f);
  for (int i = 1; i < proximity_trace.size(); i++) {
    EXPECT_EQ(proximity_trace[i], std::numeric_limits<float>::max());
  }
}

TEST(MicroExposureGeneratorTest, ProximityTracesEqual) {
  MicroExposureGeneratorBuilder meg_builder;
  std::unique_ptr<ExposureGenerator> generator =
      meg_builder.Build(kDistribution);

  std::pair<Exposure, Exposure> exposures =
      generator->Generate(absl::UnixEpoch(), absl::ZeroDuration(),
                          kInfectivityPair, kSymptomFactorPair);

  EXPECT_EQ(exposures.first.proximity_trace, exposures.second.proximity_trace);
}

TEST(MicroExposureGeneratorTest, CorrectOrderingOfExposures) {
  MicroExposureGeneratorBuilder meg_builder;
  std::unique_ptr<ExposureGenerator> generator =
      meg_builder.Build(kDistribution);

  std::pair<Exposure, Exposure> exposures =
      generator->Generate(absl::UnixEpoch(), absl::ZeroDuration(),
                          kInfectivityPair, kSymptomFactorPair);

  EXPECT_EQ(exposures.first.infectivity, kInfectivityPair.first);
  EXPECT_EQ(exposures.second.infectivity, kInfectivityPair.second);

  EXPECT_EQ(exposures.first.symptom_factor, kSymptomFactorPair.first);
  EXPECT_EQ(exposures.second.symptom_factor, kSymptomFactorPair.second);
}

}  // namespace
}  // namespace abesim
