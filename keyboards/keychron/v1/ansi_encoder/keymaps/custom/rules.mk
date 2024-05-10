TAP_DANCE_ENABLE   = yes
LTO_ENABLE         = yes
VIA_ENABLE         = yes
BOOTMAGIC_ENABLE   = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE    = yes      # Mouse keys
EXTRAKEY_ENABLE    = yes      # Audio control and System control
NKRO_ENABLE        = yes      # Enable USB N-key Rollover
ENCODER_ENABLE     = yes      # Enable Encoder
ENCODER_MAP_ENABLE = yes
DIP_SWITCH_ENABLE  = yes
RGB_MATRIX_ENABLE  = yes
BACKLIGHT_ENABLE   = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE    = no       # Enable keyboard RGB underglow
AUDIO_ENABLE       = no       # Audio output
CONSOLE_ENABLE     = no       # Console for debug
COMMAND_ENABLE     = no       # Commands for debug and configuration

# custom matrix setup
CUSTOM_MATRIX = lite
SRC += matrix.c
