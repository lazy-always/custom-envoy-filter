#pragma once

#include "extensions/filters/http/common/factory_base.h"
#include "ingress/awesome_config.pb.h"
#include "ingress/awesome_config.pb.validate.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{

class AwesomeIngressFilterConfig
    : public Extensions::HttpFilters::Common::FactoryBase<
          envoy::awesome::filters::ingress::AwesomeConfig>
{
public:
    AwesomeIngressFilterConfig() : FactoryBase("awesome.ingress") {}

private:
    static constexpr uint64_t DefaultTimeout = 200;
    Http::FilterFactoryCb createFilterFactoryFromProtoTyped(
        const envoy::awesome::filters::ingress::AwesomeConfig &,
        const std::string &, Server::Configuration::FactoryContext &) override;
};

} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy