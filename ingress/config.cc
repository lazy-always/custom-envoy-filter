

#include "config.h"
#include "awesome_filter_impl.h"
#include "awesome_filter.h"
#include "common/protobuf/utility.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{

Envoy::Http::FilterFactoryCb AwesomeIngressFilterConfig::createFilterFactoryFromProtoTyped(
    const envoy::awesome::filters::ingress::AwesomeConfig &proto_config,
    const std::string &stats_prefix, Server::Configuration::FactoryContext &context)
{

    const auto filter_config =
        std::make_shared<AwesomeFilterConfig>(proto_config, context.localInfo(), context.scope(),
                                              context.runtime(), context.httpContext(), stats_prefix);

    const uint32_t timeout_ms = PROTOBUF_GET_MS_OR_DEFAULT(proto_config, timeout, DefaultTimeout);
    const auto client_config =
        std::make_shared<AwesomeClientConfig>(proto_config, timeout_ms);

    Http::FilterFactoryCb callback = [filter_config, client_config, &context](Http::FilterChainFactoryCallbacks &callbacks) {
        auto client = std::make_unique<AwesomeHttpClientImpl>(context.clusterManager(), client_config, context.timeSource());

        callbacks.addStreamDecoderFilter(Http::StreamDecoderFilterSharedPtr{
            std::make_shared<AwesomeFilter>(filter_config, std::move(client))});
    };

    return callback;
}

} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy