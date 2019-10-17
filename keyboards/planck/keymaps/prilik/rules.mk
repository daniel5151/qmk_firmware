SRC += muse.c

EXTRAFLAGS += -DRGB_MATRIX_STARTUP_MODE=RGB_MATRIX_CUSTOM_snake

# Set any rules.mk overrides for your specific keymap here.
# See rules at https://docs.qmk.fm/#/config_options?id=the-rulesmk-file
LINK_TIME_OPTIMIZATION_ENABLE = yes
COMMAND_ENABLE = no
MOUSEKEY_ENABLE = no
UNICODEMAP_ENABLE = yes

RGB_MATRIX_CUSTOM_USER = yes