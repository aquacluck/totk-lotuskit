hotkey::bindButtonsExecFile(KEY_RSTICK|KEY_Y, "sdcard:/totk_lotuskit/idk.as");
hotkey::bindButtonsExecFileNXTas(KEY_RSTICK|KEY_X, "sdcard:/totk_lotuskit/nxtas.txt");
hotkey::bindButtonsEval(KEY_MINUS|KEY_X, "void main() { Player.pos += 100; }");

hotkey::unbindButtons(KEY_RSTICK|KEY_X); // remove hotkey
hotkey::setCooldown(15); // wait before retriggering hotkeys

