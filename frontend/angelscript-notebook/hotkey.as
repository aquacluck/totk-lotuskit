hotkey::bindButtonsExecFile(KEY_RSTICK|KEY_Y, "sdcard:/totk_lotuskit/idk.as");
hotkey::bindButtonsExecFileNXTas(KEY_RSTICK|KEY_X, "sdcard:/totk_lotuskit/nxtas.txt");
hotkey::bindButtonsEval(KEY_MINUS|KEY_X, "void main() { Player.pos_y += 100; }");

// bindKeyboard variants, using keycode names separated by a single space
hotkey::bindKeyboardExecFile("LeftControl Space", "test_whistle.as");
hotkey::bindKeyboardEval("Q W E R T Y", "void main() { Player.pos_y += 2; }");

hotkey::unbindKeyboard("Q W E R T Y");
hotkey::unbindButtons(KEY_RSTICK|KEY_X);
hotkey::setCooldown(15); // wait before retriggering hotkeys

// All keycodes: https://github.com/aquacluck/totk-lotuskit/blob/main/subrepo/nnheaders/include/nn/hid.h#L108-L239
// Common emulator keyboard issues:
// - keyboard support not enabled
// - emulator window not focused
// - LeftGui (aka win/cmd/super) stuck on by the start menu? Hold a different modifier while pressing LeftGui again to clear it.
// - Escape kills the emulator
// - Tab navigates host toolbar instead of being exposed to game
// Common console keyboard issues:
// - less keys simultaneously/rollover than emulator?

