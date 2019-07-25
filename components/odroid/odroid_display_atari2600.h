#ifndef ODROID_DISPLAY_EMU_IMPL
void ili9341_write_frame_atari2600(uint8_t* buffer, uint16_t* palette, uint8_t isPal);
#else

void ili9341_write_frame_atari2600(uint8_t* buffer, uint16_t* palette, uint8_t isPal)
{
    static const int STELLA_WIDTH = 160;
    static const int STELLA_HEIGHT_NTSC = 210;
    static const int STELLA_HEIGHT_PAL = 250;

    odroid_display_lock();

    if (buffer == NULL)
    {
        // clear the buffer
        for (int i = 0; i < LINE_BUFFERS; ++i)
        {
            memset(line[i], 0, 320 * sizeof(uint16_t) * LINE_COUNT);
        }

        // clear the screen
        send_reset_drawing(0, 0, 320, 240);

        for (int y = 0; y < 240; y += LINE_COUNT)
        {
            uint16_t* line_buffer = line_buffer_get();
            send_continue_line(line_buffer, 320, LINE_COUNT);
        }
    }
    else
    {
        int top;
        int start_line;
        int end_line;

        if (isPal)
        {
            // LCD cant display entire PAL frame
            top = 0;
            start_line = (STELLA_HEIGHT_PAL - 240) / 2;
            end_line = start_line + 240;
        }
        else
        {
            top = 120 - (STELLA_HEIGHT_NTSC / 2);
            start_line = 0;
            end_line = STELLA_HEIGHT_NTSC;
        }

        send_reset_drawing(0, top, STELLA_WIDTH * 2, end_line - start_line);

        int src_idx = 0;
        for (int y = start_line; y < end_line; y += 5)
        {
            uint16_t* line_buffer = line_buffer_get();
            int dst_idx = 0; //i * 320 * sizeof(uint16_t);

            for (int i = 0; i < 5; ++i)
            {
                for (int x = 0; x < STELLA_WIDTH; ++x)
                {
                    uint16_t pixel= palette[buffer[src_idx++]];
                    pixel = pixel << 8 | pixel >> 8;

                    line_buffer[dst_idx++] = pixel;
                    line_buffer[dst_idx++] = pixel;
                }
            }

            send_continue_line(line_buffer, 320, 5);
        }
    }
    odroid_display_unlock();
}
#endif
