# ARM Cortex-M3 LPC1768 Bare-Metal C Programs

Comprehensive collection of bare-metal embedded C programs and peripheral drivers for the **NXP LPC1768** microcontroller (ARM Cortex-M3 core). 

Every program is structured with clear, documented header comments covering **peripherals used**, **pin mappings**, **hardware registers**, and **operational logic**.

---

## 📋 Microcontroller Specifications (LPC1768)
- **Core**: ARM® 32-bit Cortex®-M3 CPU
- **Clock Frequency**: Up to 100 MHz
- **Memory**: 512 kB On-chip Flash ROM, 64 kB On-chip SRAM
- **Peripherals Included in this Repository**:
  - General Purpose I/O (GPIO)
  - Controller Area Network (CAN 2.0B)
  - Universal Asynchronous Receiver-Transmitter (UART)
  - Inter-Integrated Circuit (I2C)
  - Analog-to-Digital Converter (12-bit ADC)
  - Digital-to-Analog Converter (10-bit DAC)
  - 32-bit Timers / Match Interrupts (Timer0)
  - External Sensors (HC-SR04 Ultrasonic, ADXL345 3-Axis Accelerometer)
  - Alphanumeric LCDs (4-bit, 8-bit parallel, PCF8574 I2C backpack)
  - RFID Security & Access Control (EM-18 Module)

---

## 📂 Repository Organization

```
ARM-CORTEX-M3-LPC1768-C_PROGRAMS/
├── ADC_DAC/     # Analog-to-Digital & Digital-to-Analog converters, waveform generators
├── CAN/         # CAN Bus multi-node communication, acceptance filters & telemetry
├── LCD/         # 4-bit parallel, 8-bit parallel, and I2C PCF8574 alphanumeric LCD drivers
├── LED/         # Basic GPIO output, binary counters, running lights & switch polling
├── RFID/        # EM-18 RFID card authentication & automated access control systems
├── Sensor/      # ADXL345 I2C accelerometer & HC-SR04 ultrasonic distance sensors
├── Timer/       # Hardware Timer0 periodic interrupts, match registers & prescalers
└── UART/        # Serial communication, terminal echo, GPS NMEA parser & password security
```

---

## 📑 Program Catalog by Category

### 💡 LED / GPIO (`LED/`)
| File | Description | Target Pins |
|---|---|---|
| [`led_blink_single.c`](LED/led_blink_single.c) | Blinks on-board LED using GPIO masking | P1.29 |
| [`led_blink_4leds_simultaneous.c`](LED/led_blink_4leds_simultaneous.c) | Blinks 4 external LEDs simultaneously via Port 0 | P0.22 - P0.25 |
| [`led_4bit_binary_counter.c`](LED/led_4bit_binary_counter.c) | 4-bit binary up-counter (0–15) on Port 1 upper half | P1.21 - P1.24 |
| [`led_8bit_binary_counter.c`](LED/led_8bit_binary_counter.c) | 8-bit binary up-counter (0–255) with half-word register control | P1.22 - P1.29 |
| [`led_4led_sequential_pattern.c`](LED/led_4led_sequential_pattern.c) | Sequential running LED pattern / binary counter | P1.24 - P1.27 |
| [`led_4bit_upcounter_p1_pattern.c`](LED/led_4bit_upcounter_p1_pattern.c) | 4-bit up-counter pattern on Port 1 | P1.24 - P1.27 |
| [`led_arm_cortex_m3_first.c`](LED/led_arm_cortex_m3_first.c) | Introductory bare-metal GPIO toggle program | P0.22 - P0.25 |
| [`led_onboard_input_controlled.c`](LED/led_onboard_input_controlled.c) | Reads push button input and drives on-board LED | P0.0 (In), P1.29 (Out) |
| [`led_switch_button_p0_to_p1_v1.c`](LED/led_switch_button_p0_to_p1_v1.c) | Polled switch input to LED output (Version 1) | P0.0 (In), P1.29 (Out) |
| [`led_switch_button_p0_to_p1_v2.c`](LED/led_switch_button_p0_to_p1_v2.c) | Polled switch input to LED output (Version 2) | P0.0 (In), P1.29 (Out) |

### 🚗 Controller Area Network (`CAN/`)
| File | Description | Target Pins / IDs |
|---|---|---|
| [`can_node22_button_tx.c`](CAN/can_node22_button_tx.c) | CAN Node 22 button transmitter / receiver | P0.0 (RD), P0.1 (TD), ID: 0x22 |
| [`can_node21_button_tx.c`](CAN/can_node21_button_tx.c) | CAN Node 21 button transmitter / receiver | P0.0 (RD), P0.1 (TD), ID: 0x21 |
| [`can_node22_button_tx_v2.c`](CAN/can_node22_button_tx_v2.c) | CAN Node 22 button transceiver (Alternative implementation) | P0.0 (RD), P0.1 (TD), ID: 0x22 |
| [`can_node21_button_tx_v2.c`](CAN/can_node21_button_tx_v2.c) | CAN Node 21 button transceiver (Alternative implementation) | P0.0 (RD), P0.1 (TD), ID: 0x21 |
| [`can_ultrasonic_distance_tx.c`](CAN/can_ultrasonic_distance_tx.c) | HC-SR04 obstacle detector transmitting warnings via CAN | P0.21 (TRIG), P0.22 (ECHO), CAN1 |
| [`can_ultrasonic_alert_rx.c`](CAN/can_ultrasonic_alert_rx.c) | CAN receiver blinking alarm LED when obstacle < 3m | P1.29 (LED), CAN1 (ID: 0x21) |
| [`can_ultrasonic_timer_tx.c`](CAN/can_ultrasonic_timer_tx.c) | Ultrasonic rangefinder broadcasting raw distance byte | P2.11 (TRIG), P2.12 (ECHO), CAN1 |
| [`can_ultrasonic_i2c_lcd_rx.c`](CAN/can_ultrasonic_i2c_lcd_rx.c) | CAN receiver displaying obstacle distance on 16x2 I2C LCD | CAN1, I2C0 (P0.27, P0.28) |

### 📈 ADC & DAC (`ADC_DAC/`)
| File | Description | Target Pins |
|---|---|---|
| [`adc_to_dac_passthrough.c`](ADC_DAC/adc_to_dac_passthrough.c) | Continuous 12-bit ADC sampling replicated to 10-bit DAC output | P1.31 (AD0.5), P0.26 (AOUT) |
| [`adc_threshold_led_trigger_v1.c`](ADC_DAC/adc_threshold_led_trigger_v1.c) | Analog threshold detector using ADSTAT polling (> ~1.0V) | P1.31 (AD0.5), P1.29 (LED) |
| [`adc_threshold_led_trigger_v2.c`](ADC_DAC/adc_threshold_led_trigger_v2.c) | Analog threshold detector using software start (> ~2.0V) | P1.31 (AD0.5), P1.29 (LED) |
| [`dac_ramp_staircase_wave.c`](ADC_DAC/dac_ramp_staircase_wave.c) | Generates staircase / ramp waveform by sweeping DAC register | P0.26 (AOUT) |
| [`dac_timer_sine_wave.c`](ADC_DAC/dac_timer_sine_wave.c) | Synthesizes pure sine wave via 40-point LUT and Timer0 IRQ | P0.26 (AOUT), Timer0 |
| [`adc_dac_with_lcd_display.c`](ADC_DAC/adc_dac_with_lcd_display.c) | Workstation: ADC acquisition, DAC output & real-time LCD readout | P1.31, P0.26, LCD on Port 0 |
| [`dac_square_wave_generation.c`](ADC_DAC/dac_square_wave_generation.c) | Generates square wave on analog output using software delay | P0.26 (AOUT) |

### 📡 UART Serial Communication (`UART/`)
| File | Description | Configuration |
|---|---|---|
| [`uart_gps_nmea_sentence_parser.c`](UART/uart_gps_nmea_sentence_parser.c) | Ring-buffer UART driver parsing GPS `$GPRMC` NMEA sentences | 9600 Baud, Circular Buffer, NVIC |
| [`uart_echo_terminal_115200.c`](UART/uart_echo_terminal_115200.c) | Interactive terminal communication with string echo | 115200 Baud, 8-N-1 (FDR tuning) |
| [`uart_tx_string_9600.c`](UART/uart_tx_string_9600.c) | Serial string transmitter with THRE polling | 9600 Baud, 8-N-1 |
| [`uart_tx_char_sequence.c`](UART/uart_tx_char_sequence.c) | Character-by-character UART transmission with pacing delays | 9600 Baud, 8-N-1 |
| [`uart_rx_led_toggle.c`](UART/uart_rx_led_toggle.c) | Serial remote command decoder ('1' = ON, '0' = OFF) | 9600 Baud, P1.29 Output |
| [`uart_rx_password_auth_led.c`](UART/uart_rx_password_auth_led.c) | Password challenge/response system via serial terminal | 9600 Baud, Secret: `"hiNXP"` |

### 🪪 RFID Systems (`RFID/`)
| File | Description | Target Hardware |
|---|---|---|
| [`rfid_reader_uart_access_control_v1.c`](RFID/rfid_reader_uart_access_control_v1.c) | Reads 12-char RFID tag via UART0 and grants access via LED | EM-18, UART0, P1.29 |
| [`rfid_reader_uart_access_control_v2.c`](RFID/rfid_reader_uart_access_control_v2.c) | Interactive RFID terminal prompt with card lookup | EM-18, UART0, Serial Console |
| [`rfid_access_control_with_i2c_lcd.c`](RFID/rfid_access_control_with_i2c_lcd.c) | Complete security lock: RFID reader, 16x2 I2C LCD & lock relay | EM-18, I2C0 LCD, P1.29 Relay |

### 📟 Alphanumeric LCD Displays (`LCD/`)
| File | Description | Interface Mode |
|---|---|---|
| [`lcd_i2c_welcome_display_v1.c`](LCD/lcd_i2c_welcome_display_v1.c) | I2C 16x2 LCD display ("WELCOME") | I2C0 (100 kHz), PCF8574 (0x27) |
| [`lcd_i2c_welcome_display_v2.c`](LCD/lcd_i2c_welcome_display_v2.c) | I2C 16x2 LCD display (Alternative timing) | I2C0 (100 kHz), PCF8574 (0x27) |
| [`lcd_i2c_welcome_prajwal_display.c`](LCD/lcd_i2c_welcome_prajwal_display.c) | I2C 16x2 LCD personalized greeting | I2C0 (100 kHz), PCF8574 (0x27) |
| [`lcd_8bit_relay_switch_monitor.c`](LCD/lcd_8bit_relay_switch_monitor.c) | 8-bit parallel LCD monitoring switch & relay status | Port 0 (Data), Port 2 (Control) |
| [`lcd_4bit_counter_display.c`](LCD/lcd_4bit_counter_display.c) | 4-bit parallel LCD driving numeric counter 1–16 | P0.4 - P0.7 (Data), P0.0 - P0.2 (Ctrl) |
| [`lcd_4bit_custom_string_display.c`](LCD/lcd_4bit_custom_string_display.c) | 4-bit parallel LCD displaying dual-line custom text | P0.21 - P0.24 (Data), Port 2 (Ctrl) |
| [`lcd_8bit_string_display_v1.c`](LCD/lcd_8bit_string_display_v1.c) | 8-bit parallel HD44780 LCD text banner | P1.19 - P1.26 (Data), P0.4 - P0.6 (Ctrl) |
| [`lcd_8bit_string_display_v2.c`](LCD/lcd_8bit_string_display_v2.c) | 8-bit parallel HD44780 LCD (Alternative variant) | P1.19 - P1.26 (Data), P0.4 - P0.6 (Ctrl) |
| [`lcd_gpio_sensor_status_display.c`](LCD/lcd_gpio_sensor_status_display.c) | 8-bit parallel LCD displaying live sensor HIGH/LOW status | P0.19 (Sensor In), Port 1 (Data) |
| [`lcd_dual_switch_state_monitor.c`](LCD/lcd_dual_switch_state_monitor.c) | Dual-switch state machine with dynamic LCD readout | P1.25 (SW1), P1.26 (SW2), Port 0 LCD |

### 🧭 Sensors (`Sensor/`)
| File | Description | Interface |
|---|---|---|
| [`adxl345_accelerometer_i2c_lcd_v1.c`](Sensor/adxl345_accelerometer_i2c_lcd_v1.c) | 3-axis accelerometer with live X, Y, Z readout on I2C LCD | I2C0 (ADXL345: 0xA6/0xA7, LCD: 0x27) |
| [`adxl345_accelerometer_i2c_lcd_v2.c`](Sensor/adxl345_accelerometer_i2c_lcd_v2.c) | ADXL345 3-axis accelerometer sensor interface (Variant 2) | I2C0 (ADXL345: 0xA6/0xA7, LCD: 0x27) |
| [`ultrasonic_distance_buzzer_alert.c`](Sensor/ultrasonic_distance_buzzer_alert.c) | HC-SR04 ultrasonic rangefinder with buzzer alert (< 50cm) | P2.11 (TRIG), P2.12 (ECHO), P1.29 (Buzzer) |

### ⏱️ Hardware Timers (`Timer/`)
| File | Description | Timing Characteristics |
|---|---|---|
| [`timer0_periodic_led_interrupt.c`](Timer/timer0_periodic_led_interrupt.c) | Timer0 match interrupt with Prescaler & MR0 configuration | Periodic Match IRQ (MCR = 3) |
| [`timer0_5sec_led_toggle_irq.c`](Timer/timer0_5sec_led_toggle_irq.c) | Accurate 5-second periodic LED toggle using Timer0 ISR | PR = 24999 (1ms tick), MR0 = 5000 |

---

## 🛠️ Build and Development Environment
- **IDE**: Keil µVision (MDK-ARM v4 / v5)
- **Target Device**: NXP LPC1768
- **Device Header**: `#include <LPC17xx.h>`
- **CMSIS**: ARM CMSIS Core 3.x+
- **Flasher / Programmer**: Flash Magic (UART ISP via P0.2 / P0.3), J-Link, or ULINK2

---

## 📌 Duplicate Files Handling Note
As per repository guidelines, identical or near-identical implementations have been preserved and differentiated clearly with descriptive suffixes (e.g. `_v1.c` and `_v2.c`), allowing comparison between different register mapping styles, baud configurations, and algorithmic variations.

Empty and incomplete stub files (`lcd_4bit.c`, `ring_twisted.c`, `pshlcd_upgrade.c`, `ip_op.c`) were safely excluded from this distribution.

---

## 👤 Author
- **Prajwal Achari** ([@prajwalachari](https://github.com/prajwalachari))
