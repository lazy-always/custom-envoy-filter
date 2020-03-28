

#include "awesome_filter.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{

void AwesomeFilter::initiateCall(const Http::HeaderMap &headers)
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
    client_->check(*this, headers, callbacks_->activeSpan());
    initiating_call_ = false;
}

Http::FilterHeadersStatus AwesomeFilter::decodeHeaders(Http::RequestHeaderMap &headers, bool)
{
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
void AwesomeFilter::onDestroy()
{
    if (state_ == State::Calling)
    {
        state_ = State::Complete;
        client_->cancel();
    }
}
void AwesomeFilter::onComplete(ResponsePtr &&)
{
    state_ = State::Complete;
    stats_.ok_.inc();
    continueDecoding();
}

void AwesomeFilter::continueDecoding()
{
    filter_return_ = FilterReturn::ContinueDecoding;
    if (!initiating_call_)
    {
        callbacks_->continueDecoding();
    }
}
} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy