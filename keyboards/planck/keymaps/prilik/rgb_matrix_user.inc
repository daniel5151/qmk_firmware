RGB_MATRIX_EFFECT(snake, process_snake(record))

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS


/*----------------------------------  Snake  ---------------------------------*/

#define TRUE_MATRIX_ROWS 4
#define TRUE_MATRIX_COLS 12

// a hacky workaround for the ez's really funky MATRIX_ROWS and MATRIX_COLS vals
static uint8_t (*ez_rgb_frame_buffer)[TRUE_MATRIX_COLS] = (uint8_t (*)[TRUE_MATRIX_COLS]) rgb_frame_buffer;

typedef struct { uint8_t row, col; } snk_pos_t;

// Snake game state
static struct {
    uint32_t last_tick;

    snk_pos_t delta;
    uint8_t   len;
    snk_pos_t body[47];
    snk_pos_t apple;
} snk = {0};

static void snk_init(void) {
    snk.last_tick = 0;

    snk.delta   = (snk_pos_t){0, 1}; // start going right
    snk.len     = 1;
    snk.body[0] = (snk_pos_t){1, 2}; // start from the S key lol
    snk.apple   = (snk_pos_t){1, 5}; // spawn apple to the right of the body
}

static void snk_update_state(void) {
    // make room for new head
    for (int i = snk.len - 1; i >= 0; i--)
        snk.body[i + 1] = snk.body[i];

    // move snake's head
    snk.body[0].row += snk.delta.row;
    snk.body[0].col += snk.delta.col;

    // wrap snake around edges
    if (snk.body[0].row == 255)               snk.body[0].row = TRUE_MATRIX_ROWS - 1;
    if (snk.body[0].col == 255)               snk.body[0].col = TRUE_MATRIX_COLS - 1;
    if (snk.body[0].row ==  TRUE_MATRIX_ROWS) snk.body[0].row = 0;
    if (snk.body[0].col ==  TRUE_MATRIX_COLS) snk.body[0].col = 0;

    // check for self-collision
    for (int i = 1; i < snk.len; i++) {
        if (snk.body[i].row == snk.body[0].row &&
            snk.body[i].col == snk.body[0].col) {
            // TODO: add a nice game-over animation
            rgb_matrix_step();
            return;
        }
    }

    // check for apple collision
    if (snk.apple.row == snk.body[0].row &&
        snk.apple.col == snk.body[0].col) {
        snk.len++;
        // spawn new apple
        snk.apple.row = rand() % TRUE_MATRIX_ROWS;
        snk.apple.col = rand() % TRUE_MATRIX_COLS;
    }

    // update the frambuffer

    // set empty
    memset(ez_rgb_frame_buffer, 0, sizeof rgb_frame_buffer);
    // set body
    for (int i = 0; i < snk.len; i++)
        ez_rgb_frame_buffer[snk.body[i].row][snk.body[i].col] = i + 1;
    // set apple
    ez_rgb_frame_buffer[snk.apple.row][snk.apple.col] = 255;
}

static void process_snake(keyrecord_t* record) {
    uint8_t row = record->event.key.row;
    uint8_t col = record->event.key.col;

    // char bruh [16];
    // sprintf(bruh, "%u %u", row, col);
    // send_string(bruh);

    // the ez has a funky wiring...  
    if (row == 7 && col == 1) snk.delta = (snk_pos_t){  0, -1 }; // left
    if (row == 7 && col == 2) snk.delta = (snk_pos_t){  1,  0 }; // down
    if (row == 3 && col == 3) snk.delta = (snk_pos_t){ -1,  0 }; // up
    if (row == 3 && col == 4) snk.delta = (snk_pos_t){  0,  1 }; // right
}

static bool snake(effect_params_t* params) {
    // Modified version of RGB_MATRIX_USE_LIMITS to work off of matrix row / col size
    uint8_t led_min = RGB_MATRIX_LED_PROCESS_LIMIT * params->iter;
    uint8_t led_max = led_min + RGB_MATRIX_LED_PROCESS_LIMIT;
    if (led_max > sizeof(rgb_frame_buffer)) led_max = sizeof(rgb_frame_buffer);

    if (params->init) {
        rgb_matrix_set_color_all(0, 0, 0);
        memset(ez_rgb_frame_buffer, 0, sizeof rgb_frame_buffer);
        snk_init();
    }

    if (g_rgb_counters.tick - snk.last_tick > (rgb_matrix_config.speed * 2)) {
        snk.last_tick = g_rgb_counters.tick;
        snk_update_state();
    }

    for (int i = led_min; i < led_max; i++) {
        // on the ez it's _easy_ to fit the grid points onto a grid, since the 
        // keyboard is ortholinear.
        uint8_t row = (((float)g_led_config.point[i].y +                0) / (float) 64) * TRUE_MATRIX_ROWS;
        uint8_t col = (((float)g_led_config.point[i].x + TRUE_MATRIX_COLS) / (float)255) * TRUE_MATRIX_COLS;
        uint8_t val = ez_rgb_frame_buffer[row][col];

        RGB rgb;
        switch (ez_rgb_frame_buffer[row][col]) {
            case 0:   rgb = (RGB){ 0,   0, 0 }; break;
            case 255: rgb = (RGB){ 0, 255, 0 }; break;
            default: 
                rgb = (RGB){
                    sin(val * 0.5 + 0) * 127 + 128,
                    sin(val * 0.5 + 2) * 127 + 128,
                    sin(val * 0.5 + 4) * 127 + 128
                };
                break;
        }

        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    return led_max < sizeof(rgb_frame_buffer);
}


#endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
