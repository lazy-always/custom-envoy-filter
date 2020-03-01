/**
 * HTTP client configuration for the HTTP authorization (ext_authz) filter.
 */
class AwesomeClientConfig
{
public:
    AwesomeClientConfig(const envoy::extensions::filters::http::ext_authz::v3::ExtAuthz &config,
                        uint32_t timeout, absl::string_view path_prefix);

    /**
   * Returns the name of the authorization cluster.
   */
    const std::string &cluster() { return cluster_name_; }

    /**
   * Returns the authorization request path prefix.
   */
    const std::string &pathPrefix() { return path_prefix_; }

    /**
   * Returns authorization request timeout.
   */
    const std::chrono::milliseconds &timeout() const { return timeout_; }

    /**
   * Returns a list of matchers used for selecting the request headers that should be sent to the
   * authorization server.
   */
    const MatcherSharedPtr &requestHeaderMatchers() const { return request_header_matchers_; }

    /**
   * Returns a list of matchers used for selecting the authorization response headers that
   * should be send back to the client.
   */
    const MatcherSharedPtr &clientHeaderMatchers() const { return client_header_matchers_; }

    /**
   * Returns a list of matchers used for selecting the authorization response headers that
   * should be send to an the upstream server.
   */
    const MatcherSharedPtr &upstreamHeaderMatchers() const { return upstream_header_matchers_; }

    /**
   * Returns a list of headers that will be add to the authorization request.
   */
    const Http::LowerCaseStrPairVector &headersToAdd() const { return authorization_headers_to_add_; }

    /**
   * Returns the name used for tracing.
   */
    const std::string &tracingName() { return tracing_name_; }

private:
    static MatcherSharedPtr
    toRequestMatchers(const envoy::type::matcher::v3::ListStringMatcher &matcher);
    static MatcherSharedPtr
    toClientMatchers(const envoy::type::matcher::v3::ListStringMatcher &matcher);
    static MatcherSharedPtr
    toUpstreamMatchers(const envoy::type::matcher::v3::ListStringMatcher &matcher);
    static Http::LowerCaseStrPairVector
    toHeadersAdd(const Protobuf::RepeatedPtrField<envoy::config::core::v3::HeaderValue> &);

    const MatcherSharedPtr request_header_matchers_;
    const MatcherSharedPtr client_header_matchers_;
    const MatcherSharedPtr upstream_header_matchers_;
    const Http::LowerCaseStrPairVector authorization_headers_to_add_;
    const std::string cluster_name_;
    const std::chrono::milliseconds timeout_;
    const std::string path_prefix_;
    const std::string tracing_name_;
};