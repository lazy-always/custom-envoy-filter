

#include "awesome_filter.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{

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