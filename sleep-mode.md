# Attiny sleep mode


|   | clk CPU  | clk FLASH  | clk IO | clk ADC | Oscillators Main clock | wake INT0 | Wake SPM/EEPROM ready | wake ADC | Wake other I/O | wake watchdog | Note |
|---|---|---|---|---|---|---|---|---|---|---|---|
| Idle  |   |   | x | x | x | x | x | x | x | x |   |
| ADC noise reduction  |   |   |   | x | x | x | x | x | x | x |   |
| Power-down  |   |   |   |   |   | x |   |   |   | x |   |
| Stand-by* |   |   |   |   | x | x | x |   |   |   |   |

## 消費電力

|   | 稼働 | Sleep |   |   |   |   |   |   |   |   |   |
|---|---|---|---|---|---|---|---|---|---|---|---|
| MCU | 1mA | 2μA |   |   |   |   |   |   |   |   |   |
| SD | ~30mA | ? |   |   |   |   |   |   |   |   |   |
| RTC* | 1.5mA | 200μA | 110% |   |   |   |   |   |   |   |   |
| ACCEL | 10μA | 0.1μA |   |   |   |   |   |   |   |   |   |
| MOSFET | ? | ? |   |   |   |   |   |   |   |   |   |
| DC-DC | 90% | 90% |   |   |   |   |   |   |   |   |   |

Panasonic 単3電池 定電流連続放電1000mAで0.9時間 -> ~1.0V 
約 0.9(h)*1(A)*((1+1.5)/2(v)) = 1.125Wh

| 電流 | 10mA | 20mA | 30mA | 40mA | 50mA |   |   |   |   |   |   |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 時間 | 34 | 17 | 11.3 | 8.5 | 6.8 |   |   |   |   |   |   |
