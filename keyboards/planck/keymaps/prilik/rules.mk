SRC += muse.c

# TODO: integrate better with makefile system
EXTRALDFLAGS += -Lkeyboards/planck/keymaps/prilik/rust/target/thumbv7em-none-eabi/release/ -lrust -specs=nosys.specs

# Set any rules.mk overrides for your specific keymap here.
# See rules at https://docs.qmk.fm/#/config_options?id=the-rulesmk-file
LINK_TIME_OPTIMIZATION_ENABLE = yes
COMMAND_ENABLE = no
MOUSEKEY_ENABLE = no
UNICODEMAP_ENABLE = yes
