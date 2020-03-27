

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
/*

AwesomeFilterConfig::AwesomeFilterConfig(const envoy::awesome::filters::ingress::AwesomeConfig &config,
                                         const LocalInfo::LocalInfo &local_info, Stats::Scope &scope,
                                         Runtime::Loader &runtime, Http::Context &http_context,
                                         const std::string &stats_prefix)
{
}
*/

AwesomeFilter::AwesomeFilter(FilterConfigSharedPtr config, ClientPtr &&client)
    : config_(config), client_(std::move(client)), stats_(config->stats()) {}

AwesomeFilter::~AwesomeFilter() {}

void AwesomeFilter::onDestroy() {}

const Http::LowerCaseString AwesomeFilter::headerKey() const
{
    return Http::LowerCaseString("abc");
}
const std::string AwesomeFilter::headerValue() const
{
    return "value";
}

Http::FilterHeadersStatus AwesomeFilter::decodeHeaders(Http::RequestHeaderMap &headers,
                                                       bool)
{
    headers.addCopy(headerKey(), headerValue());
    return Http::FilterHeadersStatus::Continue;
}

Http::FilterDataStatus AwesomeFilter::decodeData(Buffer::Instance &, bool)
{
    return Http::FilterDataStatus::Continue;
}

Http::FilterTrailersStatus AwesomeFilter::decodeTrailers(Http::RequestTrailerMap &)
{
    return Http::FilterTrailersStatus::Continue;
}

void AwesomeFilter::setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks &)
{
}

void AwesomeFilter::onComplete(ResponsePtr &&)
{
}

} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy