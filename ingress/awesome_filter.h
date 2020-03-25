#pragma once

#include "ingress/awesome_config.pb.h"
#include "ingress/awesome_config.pb.validate.h"
#include "envoy/local_info/local_info.h"
#include "envoy/stats/scope.h"
#include "envoy/runtime/runtime.h"
#include "envoy/http/filter.h"
#include "envoy/http/context.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{
/**
 * All stats for the Ext Authz filter. @see stats_macros.h
 */

#define ALL_EXT_AUTHZ_FILTER_STATS(COUNTER) \
    COUNTER(ok)                             \
    COUNTER(denied)                         \
    COUNTER(error)                          \
    COUNTER(failure_mode_allowed)

/**
 * Wrapper struct for ext_authz filter stats. @see stats_macros.h
 */
struct ExtAuthzFilterStats
{
    ALL_EXT_AUTHZ_FILTER_STATS(GENERATE_COUNTER_STRUCT)
};
/**
 * Configuration for the Awesome ingress filter.
 */
class AwesomeFilterConfig
{
public:
    AwesomeFilterConfig(const envoy::awesome::filters::ingress::AwesomeConfig &config,
                        const LocalInfo::LocalInfo &local_info, Stats::Scope &scope,
                        Runtime::Loader &runtime, Http::Context &http_context,
                        const std::string &stats_prefix)
        : cluster_(config.cluster()),
          scope_(scope),
          runtime_(runtime),
          http_context_(http_context),
          local_info_(local_info),
          stats_(generateStats(stats_prefix, scope)) {}

    std::string cluster() { return cluster_; }
    Runtime::Loader &runtime() { return runtime_; }
    Stats::Scope &scope() { return scope_; }
    Http::Context &httpContext() { return http_context_; }
    const LocalInfo::LocalInfo &localInfo() const { return local_info_; }
    const ExtAuthzFilterStats &stats() const { return stats_; }

private:
    ExtAuthzFilterStats generateStats(const std::string &prefix, Stats::Scope &scope)
    {
        const std::string final_prefix = prefix + "ext_authz.";
        return {ALL_EXT_AUTHZ_FILTER_STATS(POOL_COUNTER_PREFIX(scope, final_prefix))};
    }

    const std::string cluster_;
    Stats::Scope &scope_;
    Runtime::Loader &runtime_;
    Http::Context &http_context_;
    const LocalInfo::LocalInfo &local_info_;
    // The stats for the filter.
    ExtAuthzFilterStats stats_;
};

using FilterConfigSharedPtr = std::shared_ptr<AwesomeFilterConfig>;
/**
 * HTTP ext_authz filter. Depending on the route configuration, this filter calls the global
 * ext_authz service before allowing further filter iteration.
 */
class AwesomeFilter : public Logger::Loggable<Logger::Id::filter>,
                      public Http::StreamDecoderFilter,
                      public RequestCallbacks
{
public:
    AwesomeFilter(FilterConfigSharedPtr config, ClientPtr &&client)
        : config_(config), client_(std::move(client)), stats_(config->stats()) {}

    // Http::StreamFilterBase
    void onDestroy() override;

    // Http::StreamDecoderFilter
    Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap &headers,
                                            bool end_stream) override;
    Http::FilterDataStatus decodeData(Buffer::Instance &data, bool end_stream) override;
    Http::FilterTrailersStatus decodeTrailers(Http::RequestTrailerMap &trailers) override;
    void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks &callbacks) override;

    // ExtAuthz::RequestCallbacks
    void onComplete(ResponsePtr &&) override;

private:
    void addResponseHeaders(Http::HeaderMap &header_map, const Http::HeaderVector &headers);
    void initiateCall(const Http::HeaderMap &headers);
    void continueDecoding();
    bool isBufferFull();

    // State of this filter's communication with the external authorization service.
    // The filter has either not started calling the external service, in the middle of calling
    // it or has completed.
    enum class State
    {
        NotStarted,
        Calling,
        Complete
    };

    // FilterReturn is used to capture what the return code should be to the filter chain.
    // if this filter is either in the middle of calling the service or the result is denied then
    // the filter chain should stop. Otherwise the filter chain can continue to the next filter.
    enum class FilterReturn
    {
        ContinueDecoding,
        StopDecoding
    };

    Http::HeaderMapPtr getHeaderMap(const ResponsePtr &response);
    FilterConfigSharedPtr config_;
    ClientPtr client_;
    Http::StreamDecoderFilterCallbacks *callbacks_{};
    Http::HeaderMap *request_headers_;
    State state_{State::NotStarted};
    FilterReturn filter_return_{FilterReturn::ContinueDecoding};
    Upstream::ClusterInfoConstSharedPtr cluster_;
    // The stats for the filter.
    ExtAuthzFilterStats stats_;

    // Used to identify if the callback to onComplete() is synchronous (on the stack) or asynchronous.
    bool initiating_call_{};
    bool buffer_data_{};
};

} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy