package com.grom.engine;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.SocketTimeoutException;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public final class HttpBridge {
    private static final int MAX_RESPONSE_SIZE = 1024 * 1024;
    private static final ExecutorService EXECUTOR = Executors.newCachedThreadPool();

    private HttpBridge() {
    }

    private static native void nativeComplete(
            long requestId, int statusCode, int errorCode, String body, String diagnostic);

    public static void send(long requestId, String method, String url, String[] headers,
                            String body, int timeoutMs) {
        EXECUTOR.execute(() -> execute(requestId, method, url, headers, body, timeoutMs));
    }

    private static void execute(long requestId, String method, String url, String[] headers,
                                String body, int timeoutMs) {
        HttpURLConnection connection = null;
        try {
            connection = (HttpURLConnection) new URL(url).openConnection();
            connection.setRequestMethod(method);
            connection.setConnectTimeout(timeoutMs);
            connection.setReadTimeout(timeoutMs);
            connection.setInstanceFollowRedirects(true);
            for (int i = 0; i + 1 < headers.length; i += 2) {
                connection.setRequestProperty(headers[i], headers[i + 1]);
            }
            if (body != null && !body.isEmpty() && !"GET".equals(method) && !"HEAD".equals(method)) {
                connection.setDoOutput(true);
                byte[] bytes = body.getBytes(StandardCharsets.UTF_8);
                connection.setFixedLengthStreamingMode(bytes.length);
                try (OutputStream output = connection.getOutputStream()) {
                    output.write(bytes);
                }
            }

            int statusCode = connection.getResponseCode();
            InputStream stream = statusCode >= 400
                    ? connection.getErrorStream() : connection.getInputStream();
            nativeComplete(requestId, statusCode, 0, readBody(stream), "");
        } catch (SocketTimeoutException error) {
            nativeComplete(requestId, 0, 2, "", error.toString());
        } catch (Exception error) {
            nativeComplete(requestId, 0, 1, "", error.toString());
        } finally {
            if (connection != null) connection.disconnect();
        }
    }

    private static String readBody(InputStream stream) throws Exception {
        if (stream == null) return "";
        try (InputStream input = stream; ByteArrayOutputStream output = new ByteArrayOutputStream()) {
            byte[] buffer = new byte[4096];
            int total = 0;
            int count;
            while ((count = input.read(buffer)) >= 0) {
                total += count;
                if (total > MAX_RESPONSE_SIZE) throw new IllegalStateException("HTTP response is too large");
                output.write(buffer, 0, count);
            }
            return output.toString(StandardCharsets.UTF_8.name());
        }
    }
}
