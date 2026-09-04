#import <Foundation/Foundation.h>

#include "Http/HttpBackend.h"

namespace {
constexpr NSUInteger MAX_RESPONSE_SIZE = 1024 * 1024;

NSString *ToString(const std::string &value) {
    return [NSString stringWithUTF8String:value.c_str()];
}

class AppleHttpBackend final : public nsHttpClient::IBackend {
public:
    using RequestId = nsHttpClient::RequestId;

    AppleHttpBackend() {
        auto *session = [NSURLSession sessionWithConfiguration:
                NSURLSessionConfiguration.ephemeralSessionConfiguration];
#if __has_feature(objc_arc)
        _session = session;
#else
        _session = [session retain];
#endif
    }

    ~AppleHttpBackend() override {
        CancelAll();
        [_session invalidateAndCancel];
#if !__has_feature(objc_arc)
        [_session release];
#endif
    }

    bool Send(RequestId requestId, const nsHttpRequest &request) override {
        @autoreleasepool {
            auto *url = [NSURL URLWithString:ToString(request.url)];
            if (!url) return false;
            auto *nativeRequest = [NSMutableURLRequest requestWithURL:url];
            nativeRequest.HTTPMethod = ToString(request.method);
            nativeRequest.timeoutInterval = request.timeoutMs / 1000.0;
            if (!request.body.empty()) {
                nativeRequest.HTTPBody = [NSData dataWithBytes:request.body.data()
                                                   length:request.body.size()];
            }
            for (const auto &[name, value] : request.headers) {
                [nativeRequest setValue:ToString(value) forHTTPHeaderField:ToString(name)];
            }

            auto *task = [_session dataTaskWithRequest:nativeRequest
                    completionHandler:^(NSData *data, NSURLResponse *urlResponse, NSError *error) {
                nsHttpResponse response;
                if (auto *http = [urlResponse isKindOfClass:NSHTTPURLResponse.class]
                        ? (NSHTTPURLResponse *)urlResponse : nil) {
                    response.statusCode = static_cast<int>(http.statusCode);
                }
                if (error) {
                    response.error = error.code == NSURLErrorTimedOut
                            ? nsHttpError::Timeout : nsHttpError::Network;
                    response.diagnostic = error.localizedDescription.UTF8String ?: "NSURLSession error";
                } else if (data.length > MAX_RESPONSE_SIZE) {
                    response.error = nsHttpError::Network;
                    response.diagnostic = "HTTP response is too large";
                } else if (data.length) {
                    response.body.assign(static_cast<const char *>(data.bytes), data.length);
                }
                nsHttpClient::Complete(requestId, std::move(response));
            }];
            if (!task) return false;
            [task resume];
            return true;
        }
    }

    void CancelAll() override {
        [_session getAllTasksWithCompletionHandler:^(NSArray<__kindof NSURLSessionTask *> *tasks) {
            for (NSURLSessionTask *task in tasks) [task cancel];
        }];
    }

private:
    __strong NSURLSession *_session = nil;
};
}

nsHttpClient::IBackend *CreateHttpBackend() {
    return new AppleHttpBackend();
}
