#include "tm1650.h"
#include "tm1650font.h"

#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace tm1650 {

static const char *const TAG = "display.tm1650";
const uint8_t TM1650_CMD_DATA = 48;  //!< Display data command
const uint8_t TM1650_CMD_CTRL = 0x48;  //!< Display control command
const uint8_t TM1650_CMD_ADDR = 0x48;  //!< Display address command
const uint8_t TM1650_CMD_DATA_READ = 0x49;  //!< Display address command

const uint8_t TM1650_BRT_DEF = 0x40;  //!< Display address command
const uint8_t TM1650_DSP_8S = 0x08;  //!< Display address command
const uint8_t TM1650_DSP_ON = 0x01;  //!< Display address command
const uint8_t TM1650_DSP_OFF = 0x00;  //!< Display address command

const uint8_t TM1650_DATA_WR_CMD = 0x68;  //!< Display address command
const uint8_t TM1650_SW_MSK = 0x44;  //!< Display address command

const uint8_t TM1650_ADDR_MSK = 0x03;  //!< Display address command

void  TM1650Display::set_writer(tm1650_writer_t &&writer) { this->writer_ = writer; }
void  TM1650Display::set_intensity(uint8_t intensity)     { this->intensity_ = intensity; }
void  TM1650Display::set_mode(uint8_t mode)               { this->mode_ = mode; }
void  TM1650Display::set_power(bool power)                { this->power_ = power; }
void  TM1650Display::set_length(uint8_t length)           { this->length_ = length; }
float TM1650Display::get_setup_priority() const           { return setup_priority::PROCESSOR; }


void TM1650Display::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TM1650...");

  for (int i = 0; i < this->length_; i++) {
    this->start_();
    this->send_byte_(TM1650_DATA_WR_CMD | (i+1)<<1);						// address command + address (68,6A,6C,6E)
    this->send_byte_(0);
    this->stop_();
  }

  this->start_();
  this->send_byte_(TM1650_CMD_CTRL);
  this->send_byte_((this->intensity_ << 4) | 0x08 | this->power_ ? TM1650_DSP_ON : TM1650_DSP_OFF);
  this->stop_();

  this->display();
}
void TM1650Display::stop_() {
  this->dio_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->clk_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->bit_delay_();
  
  this->dio_pin_->pin_mode(gpio::FLAG_INPUT);
  this->clk_pin_->pin_mode(gpio::FLAG_INPUT);
  this->bit_delay_();
}

void TM1650Display::display() {
  // Write DATA CMND
  this->start_();


  int counter = this->length_ - 1;

  for (int i = 0; i < this->length_; i++) {
    this->send_byte_(TM1650_DATA_WR_CMD  | + (i+1) << 1 );						// address command + address (68,6A,6C,6E)
    this->send_byte_(this->buffer_[counter]);
    counter--;
  }

/*
  this->send_byte_(TM1650_DATA_WR_CMD | 1 << 1);						// address command + address (68,6A,6C,6E)
  this->send_byte_(0b11101111);
  this->send_byte_(TM1650_DATA_WR_CMD | 2 << 1);						// address command + address (68,6A,6C,6E)
  this->send_byte_(0b11110111);
  */
  this->stop_();
}

bool TM1650Display::send_byte_(uint8_t b) {
  uint8_t data = b;
  for(int i = 0; i < 8; i++) {
      this->clk_pin_->pin_mode(gpio::FLAG_OUTPUT);
      this->bit_delay_();

      this->dio_pin_->digital_write(data & 0x80 ? true : false);
      data = data << 1;

      this->clk_pin_->pin_mode(gpio::FLAG_INPUT);
      this->bit_delay_();
  }

  this->bit_delay_();
  this->bit_delay_();

  this->clk_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->dio_pin_->pin_mode(gpio::FLAG_INPUT);

  this->bit_delay_();
  this->clk_pin_->pin_mode(gpio::FLAG_INPUT);

  this->bit_delay_();

  uint8_t ack = this->dio_pin_->digital_read();

  if( ack == 0 ) {
      this->dio_pin_->pin_mode(gpio::FLAG_OUTPUT);
  }
  this->dio_pin_->pin_mode(gpio::FLAG_OUTPUT);

  return 1;
}

void TM1650Display::bit_delay_() { 
  delayMicroseconds(120); 
}

void TM1650Display::start_() {
  this->dio_pin_->pin_mode(gpio::FLAG_INPUT);
  this->clk_pin_->pin_mode(gpio::FLAG_INPUT);
  this->bit_delay_();

  this->dio_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->clk_pin_->pin_mode(gpio::FLAG_OUTPUT);
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
#ifdef USE_BINARY_SENSOR
void TM1650Display::loop() {
  uint8_t val = this->get_keys();
  for (auto *tm1650_key : this->tm1650_keys_)
    tm1650_key->process(val);
}

uint8_t TM1650Display::get_keys() {
  this->start_();
  this->send_byte_(TM1650_CMD_DATA_READ);

  uint8_t key_code = this->read_byte_();
  this->stop_();

  if ((key_code & TM1650_SW_MSK) == TM1650_SW_MSK) {
        return key_code;
  }

  return 0;
}
#endif

uint8_t TM1650Display::read_byte_() {
  uint8_t retval = 0;

  this->clk_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->dio_pin_->pin_mode(gpio::FLAG_INPUT);

  for (uint8_t bit = 0; bit < 8; bit++) {
    retval <<= 1;
    this->clk_pin_->pin_mode(gpio::FLAG_INPUT);

    this->bit_delay_();

    if (this->dio_pin_->digital_read()) {
      retval |= 0x01;
    }
    this->clk_pin_->pin_mode(gpio::FLAG_OUTPUT);
    this->bit_delay_();
  }
  // Return DIO to output mode
  // Dummy ACK
  this->clk_pin_->pin_mode(gpio::FLAG_OUTPUT); //LOW
  this->dio_pin_->pin_mode(gpio::FLAG_INPUT); //HIGH
  this->bit_delay_();
  this->clk_pin_->pin_mode(gpio::FLAG_INPUT); //LOW
  this->bit_delay_();

  uint8_t ack = this->dio_pin_->digital_read();

  this->dio_pin_->pin_mode(gpio::FLAG_OUTPUT);

  return retval;
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


}

uint8_t TM1650Display::print(uint8_t start_pos, const char *str) {
  uint8_t pos = start_pos;

  for (; *str != '\0'; str++) {
    uint8_t char_data = TM1650_UNKNOWN_CHAR;

    if (*str >= ' ' && *str <= '~') {
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
    uint8_t data = char_data;

    // Save to buffer
    if (pos >= this->length_) {
      ESP_LOGE(TAG, "String is too long for the display!");
      break;
    }
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
