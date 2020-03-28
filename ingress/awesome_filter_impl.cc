
#include "awesome_filter_impl.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{
AwesomeHttpClientImpl::~AwesomeHttpClientImpl()
{
    ASSERT(callbacks_ == nullptr);
    ASSERT(span_ == nullptr);
}
void AwesomeHttpClientImpl::cancel() {}
// Http::AsyncClient::Callbacks
void AwesomeHttpClientImpl::onSuccess(Http::ResponseMessagePtr &&) {}
void AwesomeHttpClientImpl::onFailure(Http::AsyncClient::FailureReason) {}

/**
 * Check before starting any request 
 */
void AwesomeHttpClientImpl::check(RequestCallbacks &callbacks,
                                  Tracing::Span &parent_span)
{
    ASSERT(callbacks_ == nullptr);
    ASSERT(span_ == nullptr);
    callbacks_ = &callbacks;
    span_ = parent_span.spawnChild(Tracing::EgressConfig::get(), config_->tracingName(),
                                   time_source_.systemTime());
    span_->setTag(Tracing::Tags::get().UpstreamCluster, config_->cluster());
}

} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy