//
// Created by Roman on", 3/26/2024.
//

#include "DesktopKeyNames.h"

const char *DesktopKeyNames::GetName(int key) {
    return _names[key];
}

DesktopKeyNames::DesktopKeyNames() {
    SetName("SPACE", 32);
    SetName("APOSTROPHE", 39);
    SetName("COMMA", 44);
    SetName("MINUS", 45);
    SetName("PERIOD", 46);
    SetName("SLASH", 47);
    SetName("0", 48);
    SetName("1", 49);
    SetName("2", 50);
    SetName("3", 51);
    SetName("4", 52);
    SetName("5", 53);
    SetName("6", 54);
    SetName("7", 55);
    SetName("8", 56);
    SetName("9", 57);
    SetName("SEMICOLON", 59);
    SetName("EQUAL", 61);
    SetName("A", 65);
    SetName("B", 66);
    SetName("C", 67);
    SetName("D", 68);
    SetName("E", 69);
    SetName("F", 70);
    SetName("G", 71);
    SetName("H", 72);
    SetName("I", 73);
    SetName("J", 74);
    SetName("K", 75);
    SetName("L", 76);
    SetName("M", 77);
    SetName("N", 78);
    SetName("O", 79);
    SetName("P", 80);
    SetName("Q", 81);
    SetName("R", 82);
    SetName("S", 83);
    SetName("T", 84);
    SetName("U", 85);
    SetName("V", 86);
    SetName("W", 87);
    SetName("X", 88);
    SetName("Y", 89);
    SetName("Z", 90);
    SetName("LEFT_BRACKET", 91);
    SetName("BACKSLASH", 92);
    SetName("RIGHT_BRACKET", 93);
    SetName("GRAVE_ACCENT", 96);
    SetName("WORLD_1", 161);
    SetName("WORLD_2", 162);
    SetName("ESCAPE", 256);
    SetName("ENTER", 257);
    SetName("TAB", 258);
    SetName("BACKSPACE", 259);
    SetName("INSERT", 260);
    SetName("DELETE", 261);
    SetName("RIGHT", 262);
    SetName("LEFT", 263);
    SetName("DOWN", 264);
    SetName("UP", 265);
    SetName("PAGE_UP", 266);
    SetName("PAGE_DOWN", 267);
    SetName("HOME", 268);
    SetName("END", 269);
    SetName("CAPS_LOCK", 280);
    SetName("SCROLL_LOCK", 281);
    SetName("NUM_LOCK", 282);
    SetName("PRINT_SCREEN", 283);
    SetName("PAUSE", 284);
    SetName("F1", 290);
    SetName("F2", 291);
    SetName("F3", 292);
    SetName("F4", 293);
    SetName("F5", 294);
    SetName("F6", 295);
    SetName("F7", 296);
    SetName("F8", 297);
    SetName("F9", 298);
    SetName("F10", 299);
    SetName("F11", 300);
    SetName("F12", 301);
    SetName("F13", 302);
    SetName("F14", 303);
    SetName("F15", 304);
    SetName("F16", 305);
    SetName("F17", 306);
    SetName("F18", 307);
    SetName("F19", 308);
    SetName("F20", 309);
    SetName("F21", 310);
    SetName("F22", 311);
    SetName("F23", 312);
    SetName("F24", 313);
    SetName("F25", 314);
    SetName("KP_0", 320);
    SetName("KP_1", 321);
    SetName("KP_2", 322);
    SetName("KP_3", 323);
    SetName("KP_4", 324);
    SetName("KP_5", 325);
    SetName("KP_6", 326);
    SetName("KP_7", 327);
    SetName("KP_8", 328);
    SetName("KP_9", 329);
    SetName("KP_DECIMAL", 330);
    SetName("KP_DIVIDE", 331);
    SetName("KP_MULTIPLY", 332);
    SetName("-", 333);
    SetName("+", 334);
    SetName("KP_ENTER", 335);
    SetName("KP_EQUAL", 336);
    SetName("LEFT_SHIFT", 340);
    SetName("LCTRL", 341);
    SetName("LEFT_ALT", 342);
    SetName("LEFT_SUPER", 343);
    SetName("RIGHT_SHIFT", 344);
    SetName("RCTRL", 345);
    SetName("RIGHT_ALT", 346);
    SetName("RIGHT_SUPER", 347);
    SetName("MENU", 348);
}

void DesktopKeyNames::SetName(const char *name, int key) {
    _names[key] = name;
    _names[key].ToLower();
}
