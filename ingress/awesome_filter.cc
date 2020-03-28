

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

void AwesomeFilter::initiateCall(const Http::HeaderMap &)
{
    if (filter_return_ == FilterReturn::StopDecoding)
    {
        return;
    }

    Router::RouteConstSharedPtr route = callbacks_->route();
    if (route == nullptr || route->routeEntry() == nullptr)
    {
        return;
    }
    cluster_ = callbacks_->clusterInfo();
    ENVOY_STREAM_LOG(trace, "Awesome filter calling the cluster", *callbacks_);
    state_ = State::Calling;
    filter_return_ = FilterReturn::StopDecoding; // Don't let the filter chain continue as we are
                                                 // going to invoke check call.
    initiating_call_ = true;
    client_->check(*this, callbacks_->activeSpan());
    initiating_call_ = false;
}

Http::FilterHeadersStatus AwesomeFilter::decodeHeaders(Http::RequestHeaderMap &headers, bool)
{
    headers.addCopy(headerKey(), headerValue());
    request_headers_ = &headers;
    ENVOY_STREAM_LOG(debug, "Awesome filter is starting the request", *callbacks_);
    initiateCall(headers);
    return filter_return_ == FilterReturn::StopDecoding
               ? Http::FilterHeadersStatus::StopAllIterationAndWatermark
               : Http::FilterHeadersStatus::Continue;
}

Http::FilterDataStatus AwesomeFilter::decodeData(Buffer::Instance &, bool)
{
    return Http::FilterDataStatus::Continue;
}

Http::FilterTrailersStatus AwesomeFilter::decodeTrailers(Http::RequestTrailerMap &)
{
    return Http::FilterTrailersStatus::Continue;
}

void AwesomeFilter::setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks &callbacks)
{
    callbacks_ = &callbacks;
}

void AwesomeFilter::onComplete(ResponsePtr &&)
{
}
} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy