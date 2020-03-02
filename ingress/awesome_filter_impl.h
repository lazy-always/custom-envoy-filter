#pragma once

#include <cstdint>

#include "awesome.h"
#include "ingress/awesome_config.pb.h"
#include "ingress/awesome_config.pb.validate.h"
#include "envoy/tracing/http_tracer.h"
#include "envoy/http/async_client.h"
#include "envoy/upstream/cluster_manager.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{

/**
 * HTTP client configuration for the HTTP authorization (ext_authz) filter.
 */
class AwesomeClientConfig
{
public:
    AwesomeClientConfig(const envoy::awesome::filters::ingress::AwesomeConfig &config,
                        std::uint32_t timeout)

        : cluster_(config.cluster()),
          timeout_(timeout),
          uri_(config.uri())
    {
    }

    const std::string &cluster() { return cluster_; }
    const std::string &uri() { return uri_; }
    const std::chrono::milliseconds &timeout() const { return timeout_; }
    const std::string &tracingName() { return tracing_name_; }

private:
    const std::string cluster_;
    const std::chrono::milliseconds timeout_;
    const std::string uri_;
    const std::string tracing_name_;
};

using ClientConfigSharedPtr = std::shared_ptr<AwesomeClientConfig>;

/**
 * This client implementation is used when the Ext_Authz filter needs to communicate with an
 * HTTP authorization server. Unlike the gRPC client that allows the server to define the
 * response object, in the HTTP client, all headers and body provided in the response are
 * dispatched to the downstream, and some headers to the upstream. The HTTP client also allows
 * setting a path prefix witch is not available for gRPC.
 */
class AwesomeHttpClientImpl : public Client,
                              public Http::AsyncClient::Callbacks,
                              Logger::Loggable<Logger::Id::config>
{
public:
    explicit AwesomeHttpClientImpl(Upstream::ClusterManager &cm, ClientConfigSharedPtr config,
                                   TimeSource &time_source);
    ~AwesomeHttpClientImpl() override;

    // ExtAuthz::Client
    void cancel() override;

    // Http::AsyncClient::Callbacks
    void onSuccess(Http::ResponseMessagePtr &&message) override;
    void onFailure(Http::AsyncClient::FailureReason reason) override;

private:
    ResponsePtr toResponse(Http::ResponseMessagePtr message);
    Upstream::ClusterManager &cm_;
    ClientConfigSharedPtr config_;
    Http::AsyncClient::Request *request_{};
    RequestCallbacks *callbacks_{};
    TimeSource &time_source_;
    Tracing::SpanPtr span_;
};

} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy