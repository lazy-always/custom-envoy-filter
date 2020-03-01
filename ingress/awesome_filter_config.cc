

#include "awesome_filter_config.h"
#include "common/protobuf/utility.h"
#include "extensions/filters/common/ext_authz/ext_authz_http_impl.h"
#include "extensions/filters/http/ext_authz/ext_authz.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{

Envoy::Http::FilterFactoryCb createFilterFactoryFromProtoTyped(
    const envoy::awesome::filters::ingress::AwesomeConfig &proto_config,
    const std::string &stats_prefix, Server::Configuration::FactoryContext &context)
{
    const auto filter_config =
        std::make_shared<Extensions::HttpFilters::ExtAuthz::FilterConfig>(proto_config, context.localInfo(), context.scope(),
                                                                          context.runtime(), context.httpContext(), stats_prefix);

    const uint32_t timeout_ms = PROTOBUF_GET_MS_OR_DEFAULT(proto_config.key(),
                                                           timeout, DefaultTimeout);
    const auto client_config =
        std::make_shared<Extensions::Filters::Common::ExtAuthz::ClientConfig>(
            proto_config, timeout_ms, proto_config.key());

    Http::FilterFactoryCb callback = [filter_config, client_config,
                                      &context](Http::FilterChainFactoryCallbacks &callbacks) {
        auto client = std::make_unique<Extensions::Filters::Common::ExtAuthz::RawHttpClientImpl>(
            context.clusterManager(), client_config, context.timeSource());
        callbacks.addStreamDecoderFilter(Http::StreamDecoderFilterSharedPtr{
            std::make_shared<Extensions::HttpFilters::ExtAuthz::Filter>(filter_config, std::move(client))});
    };
    return callback;
}

} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy