
#include "awesome_filter_impl.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{
/*
AwesomeClientConfig::AwesomeClientConfig(const envoy::awesome::filters::ingress::AwesomeConfig &config,
                                         uint32_t timeout){

};
*/

AwesomeHttpClientImpl::AwesomeHttpClientImpl(Upstream::ClusterManager &cm, ClientConfigSharedPtr config,
                                             TimeSource &time_source)
    : cm_(cm), config_(config), time_source_(time_source) {}

AwesomeHttpClientImpl::~AwesomeHttpClientImpl()
{
}

void AwesomeHttpClientImpl::cancel() {}

// Http::AsyncClient::Callbacks
void AwesomeHttpClientImpl::onSuccess(Http::ResponseMessagePtr &&) {}
void AwesomeHttpClientImpl::onFailure(Http::AsyncClient::FailureReason) {}

} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy