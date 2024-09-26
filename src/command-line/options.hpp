// this file uses the "X" pattern.

// Arguments to FOUND_CLI_OPTION:
// 1. String used as the command line option.
// 2. Type of the option value.
// 3. Property name
// 4. Default value
// 5. Code to convert optarg into the value.
// 6. The default value if the option is specified with no argument, or kNoDefaultArgument

// To properly align these fields, I recommend using an editor plugin. In Vim, try `vim-lion`; in
// Emacs, try `evil-lion`. With your cursor inside any of the blocks, type `glip,` to aLign the
// Inside of the current Paragraph to comma.

#include <string>

FOUND_CLI_OPTION("png", std::string, png, "", optarg, kNoDefaultArgument)
