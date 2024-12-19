## You can store script files here for easy reference in the frontend
- All you need to do is add it to a `index-user.json` file. Create the file if you want it. It's just a list of filenames, see the builtins file for reference
- `index-builtin.json` is a separate file so later updates won't overwrite the user's changes

## TODO
- how to separate frontend snippets from utility AS on the backend? eventuallly we want the mod to be able to load some common routines on demand, not have them stuck on the frontend
    - similar to shipping+rotating long tas script issues
    - this should stay as pretty dumb kv notes then. any complicated script orchestration should be on the backend, where we'll eventually need to be reading+writing+loading from sdcard anyways

