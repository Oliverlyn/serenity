#ifndef SERENITY_OVERLOAD_DETECTOR_HPP
#define SERENITY_OVERLOAD_DETECTOR_HPP

#include <list>
#include <string>

#include "glog/logging.h"

#include "messages/serenity.hpp"

#include "serenity/config.hpp"
#include "serenity/data_utils.hpp"
#include "serenity/serenity.hpp"
#include "serenity/wid.hpp"

#include "stout/lambda.hpp"
#include "stout/nothing.hpp"
#include "stout/option.hpp"

namespace mesos {
namespace serenity {

class OverloadDetectorConfig : public SerenityConfig {
 public:
  OverloadDetectorConfig() { }

  explicit OverloadDetectorConfig(const SerenityConfig& customCfg) {
    this->initDefaults();
    this->applyConfig(customCfg);
  }

  void initDefaults() {
    //! double_t
    //! Detector threshold.
    this->fields[detector::THRESHOLD] =
      detector::DEFAULT_UTILIZATION_THRESHOLD;
  }
};

/**
 * OverloadDetector is able to create contention if utilization is above
 * given thresholds.
 */
class OverloadDetector :
    public Consumer<ResourceUsage>,
    public Producer<Contentions> {
 public:
  OverloadDetector(
      Consumer<Contentions>* _consumer,
      const lambda::function<usage::GetterFunction>& _cpuUsageGetFunction,
      SerenityConfig _conf,
      const Tag& _tag = Tag(QOS_CONTROLLER, NAME))
    : tag(_tag),
      cpuUsageGetFunction(_cpuUsageGetFunction),
      Producer<Contentions>(_consumer) {
    SerenityConfig config = OverloadDetectorConfig(_conf);
    this->cfgUtilizationThreshold =
      config.getD(detector::THRESHOLD);
  }

  ~OverloadDetector() {}

  Try<Nothing> consume(const ResourceUsage& in) override;

  static const constexpr char* NAME = "OverloadDetector";

 protected:
  const Tag tag;
  const lambda::function<usage::GetterFunction> cpuUsageGetFunction;

  // cfg parameters.
  double_t cfgUtilizationThreshold;
};

}  // namespace serenity
}  // namespace mesos

#endif  // SERENITY_OVERLOAD_DETECTOR_HPP
