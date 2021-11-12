
#ifndef _RTC_H
#define _RTC_H

// functions in header file to allow for calling of rtc functions
void init_rtc();
void handle_rtc();
// int rtc_open();
// int rtc_read();
// int rtc_write(int16_t freq);
// int rtc_close();

int32_t rtc_open(const uint8_t *filename);
int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, const void *buf, int32_t nbytes);
int32_t rtc_close(int32_t fd);

#endif

