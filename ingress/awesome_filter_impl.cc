
#include "awesome_filter_impl.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{
AwesomeHttpClientImpl::~AwesomeHttpClientImpl() {}
void AwesomeHttpClientImpl::cancel() {}
// Http::AsyncClient::Callbacks
void AwesomeHttpClientImpl::onSuccess(Http::ResponseMessagePtr &&) {}
void AwesomeHttpClientImpl::onFailure(Http::AsyncClient::FailureReason) {}

} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy