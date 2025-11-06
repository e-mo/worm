
#ifndef ANSI_COLORS_H
#define ANSI_COLORS_H
// ============================================================================
// TEXT FORMATTING
// ============================================================================
#define ANSI_RESET           "\033[0m"       // Reset all attributes
#define ANSI_BOLD            "\033[1m"       // Bold/Bright
#define ANSI_DIM             "\033[2m"       // Dim/Faint
#define ANSI_ITALIC          "\033[3m"       // Italic
#define ANSI_UNDERLINE       "\033[4m"       // Underline
#define ANSI_BLINK           "\033[5m"       // Blink (slow)
#define ANSI_BLINK_FAST      "\033[6m"       // Blink (fast) - rare support
#define ANSI_REVERSE         "\033[7m"       // Reverse/Inverse colors
#define ANSI_HIDDEN          "\033[8m"       // Hidden/Concealed
#define ANSI_STRIKETHROUGH   "\033[9m"       // Strikethrough/Crossed out

// RESET SPECIFIC ATTRIBUTES
#define ANSI_RESET_BOLD      "\033[21m"      // or "\033[22m"
#define ANSI_RESET_DIM       "\033[22m"
#define ANSI_RESET_ITALIC    "\033[23m"
#define ANSI_RESET_UNDERLINE "\033[24m"
#define ANSI_RESET_BLINK     "\033[25m"
#define ANSI_RESET_REVERSE   "\033[27m"
#define ANSI_RESET_HIDDEN    "\033[28m"
#define ANSI_RESET_STRIKE    "\033[29m"

// ============================================================================
// FOREGROUND COLORS (30-37: Standard, 90-97: Bright)
// ============================================================================
#define ANSI_FG_BLACK        "\033[30m"
#define ANSI_FG_RED          "\033[31m"
#define ANSI_FG_GREEN        "\033[32m"
#define ANSI_FG_YELLOW       "\033[33m"
#define ANSI_FG_BLUE         "\033[34m"
#define ANSI_FG_MAGENTA      "\033[35m"
#define ANSI_FG_CYAN         "\033[36m"
#define ANSI_FG_WHITE        "\033[37m"
#define ANSI_FG_DEFAULT      "\033[39m"      // Default foreground color

// BRIGHT FOREGROUND COLORS
#define ANSI_FG_BRIGHT_BLACK   "\033[90m"    // Gray
#define ANSI_FG_BRIGHT_RED     "\033[91m"
#define ANSI_FG_BRIGHT_GREEN   "\033[92m"
#define ANSI_FG_BRIGHT_YELLOW  "\033[93m"
#define ANSI_FG_BRIGHT_BLUE    "\033[94m"
#define ANSI_FG_BRIGHT_MAGENTA "\033[95m"
#define ANSI_FG_BRIGHT_CYAN    "\033[96m"
#define ANSI_FG_BRIGHT_WHITE   "\033[97m"

// ============================================================================
// BACKGROUND COLORS (40-47: Standard, 100-107: Bright)
// ============================================================================
#define ANSI_BG_BLACK        "\033[40m"
#define ANSI_BG_RED          "\033[41m"
#define ANSI_BG_GREEN        "\033[42m"
#define ANSI_BG_YELLOW       "\033[43m"
#define ANSI_BG_BLUE         "\033[44m"
#define ANSI_BG_MAGENTA      "\033[45m"
#define ANSI_BG_CYAN         "\033[46m"
#define ANSI_BG_WHITE        "\033[47m"
#define ANSI_BG_DEFAULT      "\033[49m"      // Default background color

// BRIGHT BACKGROUND COLORS
#define ANSI_BG_BRIGHT_BLACK   "\033[100m"   // Gray background
#define ANSI_BG_BRIGHT_RED     "\033[101m"
#define ANSI_BG_BRIGHT_GREEN   "\033[102m"
#define ANSI_BG_BRIGHT_YELLOW  "\033[103m"
#define ANSI_BG_BRIGHT_BLUE    "\033[104m"
#define ANSI_BG_BRIGHT_MAGENTA "\033[105m"
#define ANSI_BG_BRIGHT_CYAN    "\033[106m"
#define ANSI_BG_BRIGHT_WHITE   "\033[107m"

// ============================================================================
// CURSOR MOVEMENT
// ============================================================================
#define ANSI_CURSOR_HOME         "\033[H"        // Move to 0,0

// ============================================================================
// CURSOR VISIBILITY
// ============================================================================
#define ANSI_CURSOR_HIDE         "\033[?25l"     // Hide cursor
#define ANSI_CURSOR_SHOW         "\033[?25h"     // Show cursor

// ============================================================================
// SCREEN MANIPULATION
// ============================================================================
#define ANSI_CLEAR_SCREEN        "\033[2J"       // Clear entire screen
#define ANSI_CLEAR_LINE          "\033[2K"       // Clear entire line
#define ANSI_CLEAR_TO_EOL        "\033[K"        // Clear from cursor to end of line
#define ANSI_CLEAR_TO_BOL        "\033[1K"       // Clear from cursor to beginning of line
#define ANSI_CLEAR_TO_EOS        "\033[J"        // Clear from cursor to end of screen
#define ANSI_CLEAR_TO_BOS        "\033[1J"       // Clear from cursor to beginning of screen

// ============================================================================
// SCROLLING
// ============================================================================
#define ANSI_SCROLL_UP           "\033[S"        // Scroll up one line
#define ANSI_SCROLL_DOWN         "\033[T"        // Scroll down one line

// ============================================================================
// SAVE/RESTORE CURSOR POSITION
// ============================================================================
#define ANSI_CURSOR_SAVE         "\033[s"        // Save cursor position
#define ANSI_CURSOR_RESTORE      "\033[u"        // Restore cursor position
// Alternative (more widely supported):
#define ANSI_CURSOR_SAVE_ALT     "\0337"
#define ANSI_CURSOR_RESTORE_ALT  "\0338"

// ============================================================================
// TERMINAL MODES
// ============================================================================
#define ANSI_ALT_SCREEN_ENABLE   "\033[?1049h"   // Enable alternative screen buffer
#define ANSI_ALT_SCREEN_DISABLE  "\033[?1049l"   // Disable alternative screen buffer

// // ============================================================================
// // 256 COLOR MODE FUNCTIONS
// // ============================================================================
// static inline void set_fg_256(int color) {
//     printf("\033[38;5;%dm", color);
// }

// static inline void set_bg_256(int color) {
//     printf("\033[48;5;%dm", color);
// }

// // ============================================================================
// // RGB/TRUE COLOR MODE (24-bit) FUNCTIONS
// // ============================================================================
// static inline void set_fg_rgb(int r, int g, int b) {
//     printf("\033[38;2;%d;%d;%dm", r, g, b);
// }

// static inline void set_bg_rgb(int r, int g, int b) {
//     printf("\033[48;2;%d;%d;%dm", r, g, b);
// }

// // ============================================================================
// // CURSOR MOVEMENT FUNCTIONS
// // ============================================================================
// static inline void cursor_move_to(int row, int col) {
//     printf("\033[%d;%dH", row, col);
// }

// static inline void cursor_up(int n) {
//     printf("\033[%dA", n);
// }

// static inline void cursor_down(int n) {
//     printf("\033[%dB", n);
// }

// static inline void cursor_right(int n) {
//     printf("\033[%dC", n);
// }

// static inline void cursor_left(int n) {
//     printf("\033[%dD", n);
// }

// // ============================================================================
// // UTILITY FUNCTIONS
// // ============================================================================

// // Check if output is a terminal (to avoid escape codes in files)
// static inline int is_tty(FILE *stream) {
//     return isatty(fileno(stream));
// }

// // Conditional formatting
// static inline void print_error(const char *msg) {
//     if (is_tty(stderr)) {
//         fprintf(stderr, BOLD FG_RED "ERROR: " RESET FG_RED "%s" RESET "\n", msg);
//     } else {
//         fprintf(stderr, "ERROR: %s\n", msg);
//     }
// }

// static inline void print_warning(const char *msg) {
//     if (is_tty(stderr)) {
//         fprintf(stderr, BOLD FG_YELLOW "WARNING: " RESET FG_YELLOW "%s" RESET "\n", msg);
//     } else {
//         fprintf(stderr, "WARNING: %s\n", msg);
//     }
// }

// static inline void print_success(const char *msg) {
//     if (is_tty(stdout)) {
//         fprintf(stdout, BOLD FG_GREEN "✓ " RESET "%s" RESET "\n", msg);
//     } else {
//         fprintf(stdout, "✓ %s\n", msg);
//     }
// }

// // ============================================================================
// // DEMO FUNCTIONS
// // ============================================================================

// static inline void demo_basic_formatting(void) {
//     printf(BOLD "Bold text" RESET "\n");
//     printf(DIM "Dim text" RESET "\n");
//     printf(ITALIC "Italic text" RESET "\n");
//     printf(UNDERLINE "Underlined text" RESET "\n");
//     printf(BLINK "Blinking text" RESET "\n");
//     printf(REVERSE "Reversed text" RESET "\n");
//     printf(STRIKETHROUGH "Strikethrough text" RESET "\n");
// }

// static inline void demo_colors(void) {
//     printf(FG_RED "Red text" RESET "\n");
//     printf(FG_GREEN BG_BLUE "Green on blue" RESET "\n");
//     printf(BOLD FG_YELLOW "Bold yellow" RESET "\n");
//     printf(FG_BRIGHT_CYAN "Bright cyan" RESET "\n");
// }

// static inline void demo_combined(void) {
//     printf(BOLD UNDERLINE FG_RED BG_WHITE "Bold, underlined, red on white" RESET "\n");
//     printf(ITALIC FG_BRIGHT_GREEN "Italic bright green" RESET "\n");
// }

// static inline void demo_256_colors(void) {
//     printf("256 color palette:\n");
//     for (int i = 0; i < 256; i++) {
//         printf("\033[38;5;%dm█" RESET, i);
//         if ((i + 1) % 16 == 0) printf("\n");
//     }
// }

// static inline void demo_rgb_colors(void) {
//     // RGB gradient
//     for (int i = 0; i < 256; i += 8) {
//         printf("\033[38;2;%d;%d;%dm█" RESET, 255-i, i, 128);
//     }
//     printf("\n");
// }

// static inline void demo_cursor_movement(void) {
//     printf("Starting position\n");
//     printf(CURSOR_SAVE);
//     printf("This will disappear...\n");
//     printf(CURSOR_RESTORE);
//     printf("Back to saved position!\n");
// }

#endif // ANSI_COLORS_H
