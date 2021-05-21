# BLE beacon header

## Header

|             |                   |                                                                               |
| ----------- | ----------------- | ----------------------------------------------------------------------------- |
| MSG         | 2                 | https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers |
| Packet Flag | 2                 |                                                                               |
| Name        | 未定              |                                                                               |
| UUID        | 未定              | 16                                                                            |
| 任意データ  | 31byte の余った分 |                                                                               |
| TX pwr      | 2                 |                                                                               |

## Central filter

void filterRssi(int8_t min_rssi);
void filterMSD(uint16_t manuf_id);
void filterUuid(BLEUuid ble_uuid);

|            |                        |     |
| ---------- | ---------------------- | --- |
| filterRssi | 電波強度フィルター     |     |
| filterMSD  | 使用するため登録は必要 |     |
| filterUuid | デバイス認識           |     |

- デバイスを見分けるが必要ため、UUID は必須。Beacon の残りスペースは 3byte しかない
- ScanResponse を使う場合+29byte 使用可能
