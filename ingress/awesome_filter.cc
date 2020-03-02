

#include "awesome_filter.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{
/**
 * Configuration for the Awesome ingress filter.
 */

AwesomeFilterConfig::AwesomeFilterConfig(const envoy::awesome::filters::ingress::AwesomeConfig &config,
                                         const LocalInfo::LocalInfo &local_info, Stats::Scope &scope,
                                         Runtime::Loader &runtime, Http::Context &http_context,
                                         const std::string &stats_prefix)
{
}

} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy