#include "tm1650.h"
#include "tm1650font.h"

#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace tm1650 {

static const char *const TAG = "display.tm1650";
const uint8_t TM1650_CMD_DATA = 0x48;  //!< Display data command
const uint8_t TM1650_CMD_CTRL = 0x48;  //!< Display control command
const uint8_t TM1650_CMD_ADDR = 0x24;  //!< Display address command

const uint8_t TM1650_BRT_DEF = 0x40;  //!< Display address command
const uint8_t TM1650_DSP_8S = 0x08;  //!< Display address command
const uint8_t TM1650_DSP_ON = 0x01;  //!< Display address command
const uint8_t TM1650_DATA_WR_CMD = 0x68;  //!< Display address command

const uint8_t TM1650_ADDR_MSK = 0x03;  //!< Display address command

void  TM1650Display::set_writer(tm1650_writer_t &&writer) { this->writer_ = writer; }
void  TM1650Display::set_intensity(uint8_t intensity)     { this->intensity_ = intensity; }
void  TM1650Display::set_mode(uint8_t mode)               { this->mode_ = mode; }
void  TM1650Display::set_power(bool power)                { this->power_ = power; }
void  TM1650Display::set_length(uint8_t length)           { this->length_ = length; }
float TM1650Display::get_setup_priority() const           { return setup_priority::PROCESSOR; }

void TM1650Display::set_segment_map(const char *segment_map) {
  auto map_len = strlen(segment_map);

  if (map_len > TM1650_MAX_SEGMENTS) {
    map_len = TM1650_MAX_SEGMENTS;
  }

  for (uint8_t i = 0; i < map_len; ++i) {
    char seg_map = segment_map[map_len - 1 - i];

    // 7 <- PABCDEFG <- 0
    if (seg_map <= 'G') {
      this->segment_map_[i] = 1 << ('G' - seg_map);
    } else {
      this->segment_map_[i] = TM1650_DOT_SEGMENT;
    }
  }
}

void TM1650Display::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TM1650...");

  this->clk_pin_->setup();               // OUTPUT
  this->dio_pin_->setup();               // OUTPUT
  this->clk_pin_->digital_write(true);  // LOW
  this->dio_pin_->digital_write(true);  // LOW

  this->send_byte_(TM1650_CMD_CTRL);
  this->send_byte_(TM1650_BRT_DEF | TM1650_DSP_8S | TM1650_DSP_ON);

  this->display();
}
void TM1650Display::stop_() {
  this->dio_pin_->digital_write(false);
  bit_delay_();
  this->clk_pin_->digital_write(true);
  bit_delay_();
  this->dio_pin_->digital_write(true);
  bit_delay_();
}

void TM1650Display::display() {
  ESP_LOGD(TAG, "Display %02X%02X%02X%02X", this->buffer_[0], this->buffer_[1], this->buffer_[2], this->buffer_[3]);

  // Write DATA CMND
  this->start_();
  this->send_byte_(TM1650_DATA_WR_CMD | ((0 & TM1650_ADDR_MSK) << 1));


this->send_byte_(0x32);

this->send_byte_(0x32);

  // Write the data bytes
 // for (auto b : this->buffer_) {
  //  ESP_LOGD(TAG, "Display %s", b);
   // this->send_byte_(b);
 // }
  this->stop_();
}

bool TM1650Display::send_byte_(uint8_t b) {
  uint8_t data = b;
 for (int bit=7; bit >= 0; bit--) {    
    //The TM1650 expects MSB first
    if (((data >> bit) & 0x01) == 0x01) {
      this->dio_pin_->digital_write(true);
    }
    else {    
      this->dio_pin_->digital_write(false);
    }  
    bit_delay_();
    this->clk_pin_->digital_write(true);
    bit_delay_();
    this->clk_pin_->digital_write(false);
    bit_delay_();
  }  

  this->dio_pin_->digital_write(true);
  
  // Prepare DIO to read data
  this->dio_pin_->pin_mode(gpio::FLAG_INPUT);
  bit_delay_();
  bit_delay_();
  bit_delay_();
      
  // dummy Ack
  this->clk_pin_->digital_write(true);
  bit_delay_();
//  _ack = _dio;  
  this->clk_pin_->digital_write(false);
  bit_delay_();
  
  // Return DIO to output mode
  this->dio_pin_->pin_mode(gpio::FLAG_OUTPUT);
  bit_delay_();

  this->clk_pin_->digital_write(true);

  return 1;
}

void TM1650Display::bit_delay_() { delayMicroseconds(100); }
void TM1650Display::start_() {
  this->dio_pin_->digital_write(false);
  this->bit_delay_();
  this->clk_pin_->digital_write(false);
  this->bit_delay_();
}
void TM1650Display::update() {
  for (uint8_t &i : this->buffer_) {
    i = 0;
  }

  if (this->writer_.has_value()) {
    (*this->writer_)(*this);
  }
  this->display();

}

void TM1650Display::dump_config() {
  ESP_LOGCONFIG(TAG, "TM1650:");
  ESP_LOGCONFIG(TAG, "  Intensity: %d", this->intensity_);
  ESP_LOGCONFIG(TAG, "  Mode: %d", this->mode_);
  ESP_LOGCONFIG(TAG, "  Power: %d", this->power_);
  ESP_LOGCONFIG(TAG, "  Length: %d", this->length_);
  LOG_PIN("  Data Pin: ", this->dio_pin_);
  LOG_PIN("  Clock Pin: ", this->clk_pin_);

  LOG_UPDATE_INTERVAL(this);

  if (this->error_code_ == COMMUNICATION_FAILED) {
    ESP_LOGE(TAG, "Communication with TM1650 failed!");
  }
}

uint8_t TM1650Display::print(uint8_t start_pos, const char *str) {
  uint8_t pos = start_pos;

  for (; *str != '\0'; str++) {
    uint8_t char_data = TM1650_UNKNOWN_CHAR;

    if (*str >= ' ' && *str <= '~') {
        ESP_LOGD(TAG, "PRINT %s", str);

      char_data = progmem_read_byte(&TM1650_ASCII_TO_RAW[*str - ' ']);
    }

    if (char_data == TM1650_UNKNOWN_CHAR) {
      ESP_LOGW(TAG, "Encountered character '%c' with no TM1650 representation while translating string!", *str);
    }

    // Activate dot if it is next character
    if (*(str + 1) == '.') {
      char_data |= TM1650_DOT_SEGMENT;
      str++;
    }

    // Remap character segments
    uint8_t data = 0;

    for (auto s = 0; s < TM1650_MAX_SEGMENTS; ++s) {
      data |= (char_data & (1 << s)) ? this->segment_map_[s] : 0;
    }

    // Save to buffer
    if (pos >= this->length_) {
      ESP_LOGE(TAG, "String is too long for the display!");
      break;
    }

      ESP_LOGW(TAG, "E %s", data);

    this->buffer_[pos] = data;

    pos++;
  }

  return pos - start_pos;
}

uint8_t TM1650Display::print(const char *str) {
  return this->print(0, str);
}

uint8_t TM1650Display::printf(uint8_t pos, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}

uint8_t TM1650Display::printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(buffer);
  return 0;
}

uint8_t TM1650Display::strftime(uint8_t pos, const char *format, ESPTime time) {
  char buffer[64];
  size_t ret = time.strftime(buffer, sizeof(buffer), format);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}

uint8_t TM1650Display::strftime(const char *format, ESPTime time) {
  return this->strftime(0, format, time);
}

}  // namespace tm1650
}  // namespace esphome
