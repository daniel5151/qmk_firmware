#include "rgb_matrix_user.h"

#include <math.h>

#include "quantum.h"
#include "led_matrix.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#define __ 255
static const uint8_t KEY_TO_LED_MAP[MATRIX_ROWS][MATRIX_COLS] = {
  { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14},
  {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29},
  {30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, __, 42, 43},
  {44, __, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57},
  {58, 59, 60, __, __, __, 61, __, __, __, 62, 63, 64, 65, 66},
};

// shared with default impls (tmk_core/protocol/arm_atsam/led_matrix.c)
extern issi3733_led_t* led_cur;
extern uint8_t         led_per_run;
extern issi3733_led_t* lede;
extern issi3733_led_t  led_map[];

static alt_led_mode_t led_matrix_mode = LED_BUILTIN;

/*----------------------------------  Util  ----------------------------------*/

static uint8_t prng(void) {
  static uint8_t x = 4; // chosen by fair dice roll
  return x += (x * x) | 5;
}

static void set_led_cur_rgb(uint8_t r, uint8_t g, uint8_t b) {
  *led_cur->rgb.r = r;
  *led_cur->rgb.g = g;
  *led_cur->rgb.b = b;
}

/*-------------------------------  Snake Game  -------------------------------*/

typedef enum {
  SNKC_EMPTY_T = 0,
  SNKC_BODY_T,
  SNKC_APPLE_T,
} snk_cell_tag_t;

typedef struct {
  union {
    // Empty
    // Body
    uint8_t seg;
    // Apple
  };
  snk_cell_tag_t tag;
} snk_cell_t;

#define SNK_EMPTY      (snk_cell_t){{0},   SNKC_EMPTY_T}
#define SNK_BODY(seg)  (snk_cell_t){{seg}, SNKC_BODY_T}
#define SNK_APPLE      (snk_cell_t){{0},   SNKC_APPLE_T}

typedef struct {
  uint8_t row, col;
} snk_pos_t;

static struct {
  uint16_t update_timer;

  snk_pos_t delta;
  uint8_t   len;
  snk_pos_t body[67];
  snk_pos_t apple;

  snk_cell_t led_states[ISSI3733_LED_COUNT];
} snk = {0};

static void snk_init(void) {
  snk.update_timer = timer_read();

  snk.delta = (snk_pos_t){0, 1}; // start going right
  snk.len = 1;
  snk.body[0] = (snk_pos_t){2, 2}; // start from the S key ;)
  snk.apple = (snk_pos_t){2, 5}; // spawn apple to the right

  for (int i = 0; i < ISSI3733_LED_COUNT; i++)
    snk.led_states[i] = SNK_EMPTY;
}

static void snk_update_state(void) {
  // make room for new head
  for (int i = snk.len - 1; i >= 0; i--)
    snk.body[i + 1] = snk.body[i];

  // move snake's head
  snk.body[0].row += snk.delta.row;
  snk.body[0].col += snk.delta.col;

  // wrap snake around edges
  if (snk.body[0].row == 255)         snk.body[0].row = MATRIX_ROWS - 1;
  if (snk.body[0].col == 255)         snk.body[0].col = MATRIX_COLS - 1;
  if (snk.body[0].row == MATRIX_ROWS) snk.body[0].row = 0;
  if (snk.body[0].col == MATRIX_COLS) snk.body[0].col = 0;

  // check for self-collision
  for (int i = 1; i < snk.len; i++) {
    if (snk.body[i].row == snk.body[0].row &&
        snk.body[i].col == snk.body[0].col) {
      // TODO: add a nice game-over animation
      rgb_matrix_set_mode(LED_BUILTIN);
      return;
    }
  }

  // check for apple collision
  if (snk.apple.row == snk.body[0].row &&
      snk.apple.col == snk.body[0].col) {
    snk.len++;
    // spawn new apple
    do {
      snk.apple.row = prng() % MATRIX_ROWS;
      snk.apple.col = prng() % MATRIX_COLS;
    } while (KEY_TO_LED_MAP[snk.apple.row][snk.apple.col] == 255);
  }

  // update LEDs
  for (int i = 0; i < ISSI3733_LED_COUNT; i++)
    snk.led_states[i] = SNK_EMPTY;
  for (int i = 0; i < snk.len; i++)
    snk.led_states[KEY_TO_LED_MAP[snk.body[i].row][snk.body[i].col]] = SNK_BODY(i);
  snk.led_states[KEY_TO_LED_MAP[snk.apple.row][snk.apple.col]] = SNK_APPLE;
}

static void snk_run(void) {
  // Denotes start of new processing cycle in the case of chunked processing
  if (led_cur == 0) {
    led_cur = led_map; // return to first led
  }

  // check if it's time to run a game state update
  const uint16_t speed = max(100, 300 - snk.len * 20);
  if (timer_elapsed(snk.update_timer) > speed) {
    snk.update_timer = timer_read();
    snk_update_state();
  }

  // update leds based off game state
  uint8_t led_this_run = 0;
  while (led_cur < lede && led_this_run < led_per_run) {
    const snk_cell_t cell = snk.led_states[led_cur->id - 1];
    switch(cell.tag) {
    case SNKC_EMPTY_T:
      set_led_cur_rgb(
        0x00,
        0x00,
        0x00
      ); break;
    case SNKC_BODY_T:
      set_led_cur_rgb(
        sin(cell.seg * 0.5 + 0) * 127 + 128,
        sin(cell.seg * 0.5 + 2) * 127 + 128,
        sin(cell.seg * 0.5 + 4) * 127 + 128
      ); break;
    case SNKC_APPLE_T:
      set_led_cur_rgb(
        0x00,
        0xFF,
        0x00
      ); break;
    }

    led_cur++;
    led_this_run++;
  }
}

static bool snk_process_record_user(uint16_t keycode, keyrecord_t* record) {
  static uint16_t last_keycode = 0;

  // prevent reversing into yourself
  if (
    (keycode == KC_UP && last_keycode == KC_DOWN) ||
    (keycode == KC_DOWN && last_keycode == KC_UP) ||
    (keycode == KC_LEFT && last_keycode == KC_RGHT) ||
    (keycode == KC_RGHT && last_keycode == KC_LEFT)
  ) return true;

  switch (keycode) {
  // Snake direction
  case KC_UP:   snk.delta = (snk_pos_t){-1,  0}; break;
  case KC_LEFT: snk.delta = (snk_pos_t){ 0, -1}; break;
  case KC_DOWN: snk.delta = (snk_pos_t){ 1,  0}; break;
  case KC_RGHT: snk.delta = (snk_pos_t){ 0,  1}; break;
  // Exit snake mode
  case KC_ESC:  rgb_matrix_set_mode(LED_BUILTIN); break;
  default: break;
  }

  last_keycode = keycode;

  // you probably don't want the keys to "work" while playing snake, but if you
  // do, switch this to `true`
  return true;
}

/*---------------------------  Overriden functions ---------------------------*/
/*               (see tmk_core/protocol/arm_atsam/led_matrix.c)               */

void rgb_matrix_init_user(void) {
  snk_init();
}

bool led_matrix_run_user(void) {
  switch (led_matrix_mode) {
  case LED_BUILTIN:          return true;
  case LED_SNAKE: snk_run(); return false;
  }

  return true; // use default lighting
}

/*-----------------  Exposed functions (for use in keymap.c)  ----------------*/

void rgb_matrix_set_mode(alt_led_mode_t mode) {
  led_matrix_mode = mode;
  switch (led_matrix_mode) {
  case LED_SNAKE: snk_init(); break;
  default: break;
  }
}

bool rgb_matrix_process_record_user(uint16_t keycode, keyrecord_t* record) {
  (void)prng(); // cycle the rng

  switch (led_matrix_mode) {
  case LED_BUILTIN: return true;
  case LED_SNAKE: return snk_process_record_user(keycode, record);
  }

  return true;
}
