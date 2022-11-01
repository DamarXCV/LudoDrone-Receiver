Import("env")

# In-line command with arguments
env.Replace(
    UPLOADCMD="$PYTHONEXE $UPLOADER --chip esp32s3 --port \"COM4\" --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 8MB 0x0000 custom_boot/bootloader.bin 0x8000 .pio/build/adafruit_feather_esp32s3_nopsram/partitions.bin 0xe000 custom_boot/boot_app0.bin 0x2d0000 custom_boot/tinyuf2.bin 0x10000 .pio/build/adafruit_feather_esp32s3_nopsram/firmware.bin",
)
