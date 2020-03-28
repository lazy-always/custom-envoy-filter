
#include "awesome_filter_impl.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{

// Static response used for creating authorization ERROR responses.
const Response &errorResponse()
{
    CONSTRUCT_ON_FIRST_USE(Response,
                           Response{CheckStatus::Error, Http::HeaderVector{}, Http::HeaderVector{},
                                    EMPTY_STRING, Http::Code::Forbidden});
}
// SuccessResponse used for creating either DENIED or OK authorization responses.
struct SuccessResponse
{
    SuccessResponse(const Http::HeaderMap &headers, Response &&response)
        : headers_(headers), response_(std::make_unique<Response>(response))
    {
        headers_.iterate(
            [](const Http::HeaderEntry &, void *) -> Http::HeaderMap::Iterate {
                return Http::HeaderMap::Iterate::Continue;
            },
            this);
    }
    const Http::HeaderMap &headers_;
    ResponsePtr response_;
};

AwesomeHttpClientImpl::~AwesomeHttpClientImpl()
{
    ASSERT(callbacks_ == nullptr);
    ASSERT(span_ == nullptr);
}

void AwesomeHttpClientImpl::cancel()
{
    ASSERT(callbacks_ != nullptr);
    ASSERT(span_ != nullptr);
    span_->setTag(Tracing::Tags::get().Status, Tracing::Tags::get().Canceled);
    span_->finishSpan();
    request_->cancel();
    callbacks_ = nullptr;
    span_ = nullptr;
}

// Http::AsyncClient::Callbacks
void AwesomeHttpClientImpl::onSuccess(Http::ResponseMessagePtr &&message)
{
    callbacks_->onComplete(toResponse(std::move(message)));
    span_->finishSpan();
    callbacks_ = nullptr;
    span_ = nullptr;
}

/**
 * Set an error status if parsing status code fails. A Forbidden response is sent to the client
 * if the filter has not been configured with failure_mode_allow.
 */
ResponsePtr AwesomeHttpClientImpl::toResponse(Http::ResponseMessagePtr message)
{
    uint64_t status_code{};
    if (!absl::SimpleAtoi(message->headers().Status()->value().getStringView(), &status_code))
    {
        ENVOY_LOG(warn, "Awesome Filter failed to parse the HTTP status code.");
        span_->setTag(Tracing::Tags::get().Error, Tracing::Tags::get().True);
        return std::make_unique<Response>(errorResponse());
    }
    span_->setTag(TracingConstants::get().HttpStatus,
                  Http::CodeUtility::toString(static_cast<Http::Code>(status_code)));

    // Set an error status if the call to the authorization server returns any of the 5xx HTTP error codes.
    if (Http::CodeUtility::is5xx(status_code))
    {
        return std::make_unique<Response>(errorResponse());
    }
    // Create an Ok authorization response.
    if (status_code == enumToInt(Http::Code::OK))
    {
        SuccessResponse ok{message->headers(),
                           Response{CheckStatus::OK, Http::HeaderVector{}, Http::HeaderVector{},
                                    EMPTY_STRING, Http::Code::OK}};
        span_->setTag(TracingConstants::get().TraceStatus, TracingConstants::get().TraceOk);
        return std::move(ok.response_);
    }
    // Create a Denied authorization response.
    SuccessResponse denied{message->headers(),
                           Response{CheckStatus::Denied, Http::HeaderVector{}, Http::HeaderVector{},
                                    message->bodyAsString(), static_cast<Http::Code>(status_code)}};
    span_->setTag(TracingConstants::get().TraceStatus, TracingConstants::get().TraceUnauthz);
    return std::move(denied.response_);
}

void AwesomeHttpClientImpl::onFailure(Http::AsyncClient::FailureReason reason)
{
    ASSERT(reason == Http::AsyncClient::FailureReason::Reset);
    callbacks_->onComplete(std::make_unique<Response>(errorResponse()));
    span_->setTag(Tracing::Tags::get().Error, Tracing::Tags::get().True);
    span_->finishSpan();
    callbacks_ = nullptr;
    span_ = nullptr;
}

/**
 * Check before starting any request 
 */
void AwesomeHttpClientImpl::check(RequestCallbacks &callbacks,
                                  const Http::HeaderMap &,
                                  Tracing::Span &parent_span)
{
    ASSERT(callbacks_ == nullptr);
    ASSERT(span_ == nullptr);
    callbacks_ = &callbacks;
    span_ = parent_span.spawnChild(Tracing::EgressConfig::get(), config_->tracingName(),
                                   time_source_.systemTime());
    span_->setTag(Tracing::Tags::get().UpstreamCluster, config_->cluster());

    // write a utility here
    //std::string host = std::string(headers.get(Http::Headers::get().Host)->value().getStringView());

    Http::RequestHeaderMapPtr headers = Http::createHeaderMap<Http::RequestHeaderMapImpl>(
        {{Http::Headers::get().Host, "ssss"},
         {Http::Headers::get().Accept, Http::Headers::get().ContentTypeValues.Json},
         {Http::Headers::get().Method, Http::Headers::get().MethodValues.Get},
         {Http::Headers::get().ContentType, Http::Headers::get().ContentTypeValues.Json},
         {Http::Headers::get().Path, "/persons"}});
    Http::RequestMessagePtr message =
        std::make_unique<Http::RequestMessageImpl>(std::move(headers));

    const std::string &cluster = config_->cluster();

    // It's possible that the cluster xspecified in the filter configuration no longer exists due to a CDS removal.
    if (cm_.get(cluster) == nullptr)
    {
        ENVOY_LOG(debug, "Cluster '{}' does not exist", cluster);
        callbacks_->onComplete(std::make_unique<Response>(errorResponse()));
        span_->setTag(Tracing::Tags::get().Error, Tracing::Tags::get().True);
        span_->finishSpan();
        callbacks_ = nullptr;
        span_ = nullptr;
    }
    else
    {
        ENVOY_LOG(debug, "Calling Cluster '{}'", cluster);
        span_->injectContext(message->headers());
        request_ = cm_.httpAsyncClientForCluster(cluster).send(
            std::move(message), *this,
            Http::AsyncClient::RequestOptions().setTimeout(config_->timeout()));
    }
}
} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy