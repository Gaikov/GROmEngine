package com.grom.engine;

import android.content.Context;
import android.hardware.input.InputManager;
import android.os.Handler;
import android.os.Looper;
import android.view.InputDevice;
import android.view.KeyEvent;

import java.util.TreeMap;

// Process-owned, like AndroidPlatform. An Activity/EGL recreation must not
// register another listener. No Activity is retained by this bridge.
public final class GamepadBridge implements InputManager.InputDeviceListener {
    private static GamepadBridge _instance;

    private static native void nativeDeviceChanged(int id, String name, int[] axes,
            float[] minima, float[] maxima, int[] buttons);
    private static native void nativeDeviceRemoved(int id);

    public static synchronized void Start(Context context) {
        if (_instance == null) _instance = new GamepadBridge(context.getApplicationContext());
    }

    private GamepadBridge(Context context) {
        InputManager manager = context.getSystemService(InputManager.class);
        manager.registerInputDeviceListener(this, new Handler(Looper.getMainLooper()));
        for (int id : InputDevice.getDeviceIds()) Publish(id);
    }

    private static boolean IsButton(int key) {
        return KeyEvent.isGamepadButton(key)
                || (key >= KeyEvent.KEYCODE_DPAD_UP && key <= KeyEvent.KEYCODE_DPAD_CENTER)
                || (key >= KeyEvent.KEYCODE_DPAD_UP_LEFT && key <= KeyEvent.KEYCODE_DPAD_DOWN_RIGHT);
    }

    private void Publish(int id) {
        InputDevice device = InputDevice.getDevice(id);
        if (device == null || (!device.supportsSource(InputDevice.SOURCE_GAMEPAD)
                && !device.supportsSource(InputDevice.SOURCE_JOYSTICK))) {
            onInputDeviceRemoved(id);
            return;
        }
        TreeMap<Integer, InputDevice.MotionRange> ranges = new TreeMap<>();
        for (InputDevice.MotionRange range : device.getMotionRanges()) {
            if ((range.getSource() & InputDevice.SOURCE_CLASS_MASK) == InputDevice.SOURCE_CLASS_JOYSTICK
                    && range.getAxis() >= 0) {
                ranges.put(range.getAxis(), range);
            }
        }
        int[] axes = new int[ranges.size()];
        float[] minima = new float[ranges.size()];
        float[] maxima = new float[ranges.size()];
        int index = 0;
        for (InputDevice.MotionRange range : ranges.values()) {
            axes[index] = range.getAxis();
            minima[index] = range.getMin();
            maxima[index++] = range.getMax();
        }
        int count = 0;
        for (int key = 0; key <= KeyEvent.getMaxKeyCode(); key++) if (IsButton(key)) count++;
        int[] candidates = new int[count];
        index = 0;
        for (int key = 0; key <= KeyEvent.getMaxKeyCode(); key++) {
            if (IsButton(key)) candidates[index++] = key;
        }
        boolean[] supported = device.hasKeys(candidates);
        count = 0;
        for (boolean present : supported) if (present) count++;
        int[] buttons = new int[count];
        index = 0;
        for (int i = 0; i < candidates.length; i++) {
            if (supported[i]) buttons[index++] = candidates[i];
        }
        nativeDeviceChanged(id, device.getName(), axes, minima, maxima, buttons);
    }

    @Override public void onInputDeviceAdded(int id) { Publish(id); }
    @Override public void onInputDeviceChanged(int id) { Publish(id); }
    @Override public void onInputDeviceRemoved(int id) {
        nativeDeviceRemoved(id);
    }
}
