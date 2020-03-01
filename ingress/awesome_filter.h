

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{
/**
 * Configuration for the Awesome ingress filter.
 */
class AwesomeFilterConfig
{
public:
    AwesomeFilterConfig(const envoy::extensions::filters::http::ext_authz::v3::ExtAuthz &config,
                        const LocalInfo::LocalInfo &local_info, Stats::Scope &scope,
                        Runtime::Loader &runtime, Http::Context &http_context,
                        const std::string &stats_prefix)
        : allow_partial_message_(config.with_request_body().allow_partial_message()),
          failure_mode_allow_(config.failure_mode_allow()),
          clear_route_cache_(config.clear_route_cache()),
          max_request_bytes_(config.with_request_body().max_request_bytes()),
          status_on_error_(toErrorCode(config.status_on_error().code())), local_info_(local_info),
          scope_(scope), runtime_(runtime), http_context_(http_context),
          filter_enabled_(config.has_filter_enabled()
                              ? absl::optional<Runtime::FractionalPercent>(
                                    Runtime::FractionalPercent(config.filter_enabled(), runtime_))
                              : absl::nullopt),
          pool_(scope_.symbolTable()),
          metadata_context_namespaces_(config.metadata_context_namespaces().begin(),
                                       config.metadata_context_namespaces().end()),
          include_peer_certificate_(config.include_peer_certificate()),
          stats_(generateStats(stats_prefix, scope)), ext_authz_ok_(pool_.add("ext_authz.ok")),
          ext_authz_denied_(pool_.add("ext_authz.denied")),
          ext_authz_error_(pool_.add("ext_authz.error")),
          ext_authz_failure_mode_allowed_(pool_.add("ext_authz.failure_mode_allowed")) {}

    bool allowPartialMessage() const { return allow_partial_message_; }

    bool withRequestBody() const { return max_request_bytes_ > 0; }

    bool failureModeAllow() const { return failure_mode_allow_; }

    bool clearRouteCache() const { return clear_route_cache_; }

    uint32_t maxRequestBytes() const { return max_request_bytes_; }

    const LocalInfo::LocalInfo &localInfo() const { return local_info_; }

    Http::Code statusOnError() const { return status_on_error_; }

    bool filterEnabled() { return filter_enabled_.has_value() ? filter_enabled_->enabled() : true; }

    Runtime::Loader &runtime() { return runtime_; }

    Stats::Scope &scope() { return scope_; }

    Http::Context &httpContext() { return http_context_; }

    const std::vector<std::string> &metadataContextNamespaces()
    {
        return metadata_context_namespaces_;
    }

    const ExtAuthzFilterStats &stats() const { return stats_; }

    void incCounter(Stats::Scope &scope, Stats::StatName name)
    {
        scope.counterFromStatName(name).inc();
    }

    bool includePeerCertificate() const { return include_peer_certificate_; }

private:
    static Http::Code toErrorCode(uint64_t status)
    {
        const auto code = static_cast<Http::Code>(status);
        if (code >= Http::Code::Continue && code <= Http::Code::NetworkAuthenticationRequired)
        {
            return code;
        }
        return Http::Code::Forbidden;
    }

    ExtAuthzFilterStats generateStats(const std::string &prefix, Stats::Scope &scope)
    {
        const std::string final_prefix = prefix + "ext_authz.";
        return {ALL_EXT_AUTHZ_FILTER_STATS(POOL_COUNTER_PREFIX(scope, final_prefix))};
    }

    const bool allow_partial_message_;
    const bool failure_mode_allow_;
    const bool clear_route_cache_;
    const uint32_t max_request_bytes_;
    const Http::Code status_on_error_;
    const LocalInfo::LocalInfo &local_info_;
    Stats::Scope &scope_;
    Runtime::Loader &runtime_;
    Http::Context &http_context_;

    const absl::optional<Runtime::FractionalPercent> filter_enabled_;

    // TODO(nezdolik): stop using pool as part of deprecating cluster scope stats.
    Stats::StatNamePool pool_;

    const std::vector<std::string> metadata_context_namespaces_;

    const bool include_peer_certificate_;

    // The stats for the filter.
    ExtAuthzFilterStats stats_;

public:
    // TODO(nezdolik): deprecate cluster scope stats counters in favor of filter scope stats
    // (ExtAuthzFilterStats stats_).
    const Stats::StatName ext_authz_ok_;
    const Stats::StatName ext_authz_denied_;
    const Stats::StatName ext_authz_error_;
    const Stats::StatName ext_authz_failure_mode_allowed_;
};

} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy