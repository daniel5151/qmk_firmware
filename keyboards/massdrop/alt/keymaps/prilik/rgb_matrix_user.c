#include "quantum.h"
#include "led_matrix.h"

#define __ 255
static const uint8_t KEY_TO_LED_MAP[MATRIX_ROWS][MATRIX_COLS] = {
  { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14},
  {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29},
  {30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, __, 42, 43},
  {44, __, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57},
  {58, 59, 60, __, __, __, 61, __, __, __, 62, 63, 64, 65, 66},
                        /* 74 */ // somehow bridge to bottom bar?
};

// shared with default impls (tmk_core/protocol/arm_atsam/led_matrix.c)
extern issi3733_led_t* led_cur;
extern uint8_t         led_per_run;
extern issi3733_led_t* lede;
extern issi3733_led_t  led_map[];

// Snake game state

typedef enum {
  SNK_EMPTY = 0,
  SNK_BODY,
  SNK_APPLE,
} snk_cell_t;

typedef enum {
  DIR_LEFT,
  DIR_RIGHT,
  DIR_UP,
  DIR_DOWN
} snk_dir_t;

typedef struct {
  uint8_t row, col;
} snkpos_t;

static struct {
  uint16_t update_timer;
  uint8_t  rng;

  uint16_t  speed; // ms between updates
  snk_dir_t dir;
  uint8_t   len;
  snkpos_t  body[67];
  snkpos_t  apple;

  uint8_t led_states[ISSI3733_LED_COUNT];

} snk;

static uint8_t snkrng(void) {
  uint8_t out = snk.rng >> 1;
  if (snk.rng & 1) snk.rng = out ^ 0xB8;
  return snk.rng;
}

// overrides default impl (tmk_core/protocol/arm_atsam/led_matrix.c)
void rgb_matrix_init_user(void) {
  // Init Snake game state
  snk.update_timer = timer_read();

  snk.speed = 1000; // ms
  snk.dir = DIR_RIGHT;
  snk.len = 1;
  snk.body[0].row = 3; snk.body[0].col = 3; // start from the S key ;)
  snk.apple.row   = 3; snk.apple.col   = 6; // spawn apple nearby & to the right

  // Set initial LED states
  for (int i = 0; i < ISSI3733_LED_COUNT; i++)
    snk.led_states[i] = SNK_EMPTY;
  snk.led_states[KEY_TO_LED_MAP[3][3]] = SNK_BODY;
  snk.led_states[KEY_TO_LED_MAP[3][6]] = SNK_APPLE;
}

static void snk_update_state(void) {
  // TODO

  // Don't forget to update snk.led_states!
}

// overrides default impl (tmk_core/protocol/arm_atsam/led_matrix.c)
bool led_matrix_run_user(void) {
  // Denotes start of new processing cycle in the case of chunked processing
  if (led_cur == 0) {
    led_cur = led_map; // return to first led
  }

  // check if it's time to run a game state update
  if (timer_elapsed(snk.update_timer) > snk.speed) {
    snk.update_timer = timer_read();
    snk_update_state();
  }

  // update leds based off game state
  uint8_t led_this_run = 0;
  while (led_cur < lede && led_this_run < led_per_run) {
    #define set_rgb(rr,gg,bb) do { \
        *led_cur->rgb.r = rr; \
        *led_cur->rgb.g = gg; \
        *led_cur->rgb.b = bb; \
      } while (0);

    switch(snk.led_states[led_cur->id - 1]) {
    case SNK_EMPTY: set_rgb(0x00, 0x00, 0x00); break;
    case SNK_BODY:  set_rgb(0x00, 0x00, 0xFF); break;
    case SNK_APPLE: set_rgb(0x00, 0xFF, 0x00); break;
    }

    #undef set_rgb

    led_cur++;
    led_this_run++;
  }

  return false;
}

void rgb_matrix_record_key_press(uint16_t keycode, keyrecord_t* record) {
  (void)snkrng(); // cycle the rng some more

  // update snake direction
  switch (keycode) {
  case KC_UP:   snk.dir = DIR_UP;    break;
  case KC_LEFT: snk.dir = DIR_LEFT;  break;
  case KC_DOWN: snk.dir = DIR_DOWN;  break;
  case KC_RGHT: snk.dir = DIR_RIGHT; break;
  default: break;
  }
}
