# Vendored curl

- Version: curl 8.22.0 (released 2026-09-02)
- Source: `https://curl.se/download/curl-8.22.0.tar.xz`
- SHA-256: `f7ef3ae8a22e521f289803fe93543eb64c329b58aa73a9e224dfd915a2a5f4f7`
- License: curl license, see `COPYING`

The release signature was verified against curl release manager Daniel
Stenberg's key `27EDEAF22F3ABCEB50DB9A125CC908FDB71E12C2` before the source was
added to GROmEngine.

GROmEngine builds this copy as a static HTTP(S)-focused library on Windows and
Linux/Unix. Windows uses Schannel. Linux/Unix uses the system OpenSSL development
package. macOS uses `NSURLSession`, Android uses `HttpURLConnection`, and Web
uses `fetch`, because those platforms already provide a native HTTP and TLS
stack.

When updating curl, download a signed release from curl.se, verify its signature
using the documented curl release key, replace this directory, update the
version and checksum above, then re-run the Windows and Linux CMake checks.
