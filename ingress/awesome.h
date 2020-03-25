#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "envoy/common/pure.h"
#include "envoy/http/codes.h"
#include "envoy/tracing/http_tracer.h"

#include "common/singleton/const_singleton.h"

namespace Envoy
{
namespace Awesome
{
namespace Filters
{
namespace Ingress
{

/**
 * Constant values used for tracing metadata.
 */
struct TracingConstantValues
{
    const std::string TraceStatus = "ext_authz_status";
    const std::string TraceUnauthz = "ext_authz_unauthorized";
    const std::string TraceOk = "ext_authz_ok";
    const std::string HttpStatus = "ext_authz_http_status";
};

using TracingConstants = ConstSingleton<TracingConstantValues>;

/**
 * Possible async results for a check call.
 */
enum class CheckStatus
{
    // The request is authorized.
    OK,
    // The authz service could not be queried.
    Error,
    // The request is denied.
    Denied
};

/**
 * Authorization response object for a RequestCallback.
 */
struct Response
{
    // Call status.
    CheckStatus status;
    // Optional http headers used on either denied or ok responses.
    Http::HeaderVector headers_to_append;
    // Optional http headers used on either denied or ok responses.
    Http::HeaderVector headers_to_add;
    // Optional http body used only on denied response.
    std::string body;
    // Optional http status used only on denied response.
    Http::Code status_code{};
};

using ResponsePtr = std::unique_ptr<Response>;

/**
 * Async callbacks used during check() calls.
 */
class RequestCallbacks
{
public:
    virtual ~RequestCallbacks() = default;

    /**
   * Called when a check request is complete. The resulting ResponsePtr is supplied.
   */
    virtual void onComplete(ResponsePtr &&response) PURE;
};

class Client
{
public:
    // Destructor
    virtual ~Client() = default;

    /**
   * Cancel an inflight Check request.
   */
    virtual void cancel() PURE;
};

using ClientPtr = std::unique_ptr<Client>;

} // namespace Ingress
} // namespace Filters
} // namespace Awesome
} // namespace Envoy