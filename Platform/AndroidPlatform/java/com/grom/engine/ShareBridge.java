package com.grom.engine;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.net.Uri;

import androidx.core.content.FileProvider;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public final class ShareBridge {
    private static final int STATUS_PRESENTED = 0;
    private static final int STATUS_ERROR = 5;

    private ShareBridge() {
    }

    private static native void nativeComplete(long requestId, int status, String diagnostic);

    public static boolean handle(Activity activity, String[] params) {
        if (activity == null || params == null || params.length != 5) return false;

        final long requestId;
        try {
            requestId = Long.parseLong(params[0]);
        } catch (NumberFormatException exception) {
            return false;
        }

        final String title = params[1];
        final String text = params[2];
        final String url = params[3];
        final String imageAssetPath = params[4];
        activity.runOnUiThread(() -> openChooser(
                activity, requestId, title, text, url, imageAssetPath));
        return true;
    }

    private static void openChooser(Activity activity, long requestId, String title,
                                    String text, String url, String imageAssetPath) {
        final String combinedText = text.isEmpty() || url.isEmpty()
                ? text + url : text + "\n" + url;

        try {
            Intent sendIntent = createImageIntent(
                    activity, title, combinedText, imageAssetPath);
            if (sendIntent == null || !canHandle(activity, sendIntent)) {
                sendIntent = createTextIntent(title, combinedText);
            }
            if (!canHandle(activity, sendIntent)) {
                nativeComplete(requestId, STATUS_ERROR, "No application can share this content");
                return;
            }

            Intent chooser = Intent.createChooser(sendIntent, title);
            activity.startActivity(chooser);
            nativeComplete(requestId, STATUS_PRESENTED, "");
        } catch (ActivityNotFoundException exception) {
            nativeComplete(requestId, STATUS_ERROR, "No sharing application is available");
        } catch (Exception exception) {
            nativeComplete(requestId, STATUS_ERROR,
                    exception.getMessage() == null ? exception.toString() : exception.getMessage());
        }
    }

    private static Intent createImageIntent(Activity activity, String title,
                                            String text, String imageAssetPath) {
        if (imageAssetPath == null || imageAssetPath.isEmpty()) return null;

        File shareDirectory = new File(activity.getCacheDir(), "shared");
        if (!shareDirectory.exists() && !shareDirectory.mkdirs()) return null;
        File imageFile = new File(shareDirectory, "share.png");

        try (InputStream input = activity.getAssets().open(imageAssetPath);
             FileOutputStream output = new FileOutputStream(imageFile, false)) {
            byte[] buffer = new byte[16 * 1024];
            int count;
            while ((count = input.read(buffer)) != -1) {
                if (count > 0) output.write(buffer, 0, count);
            }
        } catch (Exception exception) {
            return null;
        }

        Uri uri = FileProvider.getUriForFile(
                activity, activity.getPackageName() + ".share-files", imageFile);
        return new Intent(Intent.ACTION_SEND)
                .setType("image/png")
                .putExtra(Intent.EXTRA_SUBJECT, title)
                .putExtra(Intent.EXTRA_TEXT, text)
                .putExtra(Intent.EXTRA_STREAM, uri)
                .addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
    }

    private static Intent createTextIntent(String title, String text) {
        return new Intent(Intent.ACTION_SEND)
                .setType("text/plain")
                .putExtra(Intent.EXTRA_SUBJECT, title)
                .putExtra(Intent.EXTRA_TEXT, text);
    }

    private static boolean canHandle(Activity activity, Intent intent) {
        return intent != null && intent.resolveActivity(activity.getPackageManager()) != null;
    }
}
