# Waveshare 無源 NFC 電子墨水屏指令

* [原始碼研究](https://github.com/taichunmin/waveshare-epd-demo/blob/master/User/Browser/Browser.c)
* [黑白紅圖片編碼方法](https://www.waveshare.net/wiki/1.54inch_e-Paper_Module_%28B%29)
* [在 Photoshop 中把圖片處理成黑白](https://www.waveshare.net/wiki/E-Paper-Floyd-Steinberg)
* [waveshare-epd= 1.54B sniff](https://gist.github.com/taichunmin/aeabd2dd8a8aa99c7b22487704dfd40b)
* [ST25R3911B NFC Board 微雪官方開發板](https://www.waveshare.net/wiki/ST25R3911B_NFC_Board)
* [PM3 for Waveshare](https://github.com/RfidResearchGroup/proxmark3/blob/master/client/src/cmdhfwaveshare.c#L662)

## 黑白雙色

![](https://i.imgur.com/sethLTU.png)

```mermaid
flowchart LR
    classDef invisible display:none;
    subgraph sub1 [第1部份]
        direction TB
        cd0d[1. nfc 驅動板供電指令]-->cd00[2. NFCTag 復位及選型指令]
        cd00-->|delay: 50ms|cd01[3. NFCTag 配置1指令]
        cd01-->|delay: 10ms|cd02[4. NFCTag 配置2指令]
        cd02-->|"delay: 10ms<br>(下接第2部份)"|hidden1:::invisible
    end
    subgraph sub2 [第2部份]
        direction TB
        cd03[5. NFCTag 上電指令]-->|delay: 10ms|cd05[6. NFCTag 配置3指令]
        cd05-->|delay: 10ms|cd06[7. NFCTag 配置4指令]
        cd06-->|delay: 10ms|cd07[8. NFCTag 準備發送數據指令]
        cd07-->|"(下接第3部份)"|hidden2:::invisible
    end
    subgraph sub3 [第3部份]
        direction TB
        cd08[9. NFCTag 開始發送數據指令]-->|"循環150次到數據傳送完畢<br>(無需延時)"|cd08
        cd08-->cd18[10. 0xCD18]
        cd18-->cd09[12. NFCTag 開始刷屏指令]
        cd18-->cd19[11. NFCTag 開始發送數據指令]
        cd19-->|"循環150次到數據傳送完畢<br>(無需延時)"|cd19
        cd19-->cd09
        cd09-->cd0a[14. 等待 NFCTag 刷屏成功]
        cd0a-->|"循環至刷屏成功指令返回為止<br>(延時 25ms)"|cd0a
        cd0a-->cd04[15. 發送關閉 NFCTag 電源指令]
    end
    sub1-.-sub2
    sub2-.-sub3
```

```
0xCD0D // 1. nfc 驅動板供電指令, 接收成功返回 0x0000, retry 10
0xCD00 + 型號 // 2. NFCTag 復位及選型指令, 接收成功返回 0x0000, retry 10
0xCD01 // 3. NFCTag 配置1指令, 接收成功返回 0x0000, retry 10
0xCD02 // 4. NFCTag 配置2指令, 接收成功返回 0x0000, retry 10
0xCD03 // 5. NFCTag 上電指令, 接收成功返回 0x0000, retry 10
0xCD05 // 6. NFCTag 配置3指令, 接收成功返回 0x0000, retry 30
0xCD06 // 7. NFCTag 配置4指令, 接收成功返回 0x0000, retry 10
0xCD07 // 8. NFCTag 準備發送數據指令, 接收成功返回 0x0000, retry 10
0xCD08 + 圖像數據長度 + 圖像數據 // 9. NFCTag 開始發送數據指令
0xCD18 // 10. 僅限型號 0,1,2,3,7, 接收成功返回 0x0000, retry 10
0xCD18 // 10. 僅限型號 4,5
0xCD19 + 圖像數據長度 + 圖像數據 // 11. NFCTag 開始發送數據指令, 僅限型號 4,5
0xCD09 // 12. NFCTag 開始刷屏指令, 接收成功返回 0x0000, retry 10
// 13. 等候 10 秒, 僅限型號 5, 6
0xCD0A // 14. 等待 NFCTag 刷屏成功, 接收成功返回 0xFF00, retry 70
0xCD04 // 15. 發送關閉 NFCTag 電源指令
```

* 7.5HD 跟 2.7inch 在 Step 9 時須傳送空資料 `0xFF` * `120 bytes`。

### 型號

![](https://i.imgur.com/rrhreNk.png)

### 圖像數據包大小

1. `0x10`: 2.13 inch (Send 16 data at a time)
2. `0x10`: 2.9 inch (Send 16 data at a time) 
3. `0x64`: 4.2 inch (Send 100 data at a time)
4. `0x78`: 7.5 inch (Send 120 data at a time)

### 程式碼範例

<details>

<summary>Android 程式碼範例</summary>


```java
public boolean canSendPic() {
  byte[] response;
  /*reset eink */
  try {
    response = tntag.transceive(new byte[]{(byte) 0xcd, 0x0d});
    if ((response[0] != (byte) 0x00) || (response[1] != (byte) 0x00)) {
      setStatusBody(getString(R.string.txfail));
      return false;
    }
    response = tntag.transceive(new byte[]{(byte) 0xcd, 0, 10});
    if ((response[0] != (byte) 0x00) || (response[1] != (byte) 0x00)) {
      setStatusBody(getString(R.string.txfail));
      return false;
    }
    SystemClock.sleep(50);
    response = tntag.transceive(new byte[]{(byte) 0xcd, 1});
    if ((response[0] != (byte) 0x00) || (response[1] != (byte) 0x00)) {
      setStatusBody(getString(R.string.txfail));
      return false;
    }
    SystemClock.sleep(10);
    /*eink config*/
    response = tntag.transceive(new byte[]{(byte) 0xcd, 2});
    if ((response[0] != (byte) 0x00) || (response[1] != (byte) 0x00)) {
      setStatusBody(getString(R.string.txfail));
      return false;
    }
    SystemClock.sleep(10);
    response = tntag.transceive(new byte[]{(byte) 0xcd, 3});
    if ((response[0] != (byte) 0x00) || (response[1] != (byte) 0x00)) {
      setStatusBody(getString(R.string.txfail));
      return false;
    }
    SystemClock.sleep(10);
    response = tntag.transceive(new byte[]{(byte) 0xcd, 5});
    if ((response[0] != (byte) 0x00) || (response[1] != (byte) 0x00)) {
      setStatusBody(getString(R.string.txfail));
      return false;
    }
    SystemClock.sleep(10);
    response = tntag.transceive(new byte[]{(byte) 0xcd, 6});
    if ((response[0] != (byte) 0x00) || (response[1] != (byte) 0x00)) {
      setStatusBody(getString(R.string.txfail));
      return false;
    }
    SystemClock.sleep(10);
    /*eink tada tx */
    int[] intArray = new int[bmp_send.getWidth() * bmp_send.getHeight()];
    //copy pixel data from the Bitmap into the 'intArray' array
    bmp_send.getPixels(intArray, 0, bmp_send.getWidth(), 0, 0, bmp_send.getWidth(), bmp_send.getHeight());
    byte packed_pix;
    for (int i = 0; i < 300; i++) {
      for (int j = 0; j < 50; j++) {
        packed_pix = 0;
        for (int k = 0; k < 8; k++)
        {
          packed_pix = (byte)(packed_pix<<1);
          if (((int)(intArray[k+(j*8)+(i*400)])&0x0ff) > 128 ) packed_pix |= 0x01;
        }
        pic_send[i*50+j] = packed_pix;
      }
    }
    //data start command
    response = tntag.transceive(new byte[]{(byte) 0xcd, 7, 0});
    if ((response[0] != (byte) 0x00) || (response[1] != (byte) 0x00)) {
      setStatusBody(getString(R.string.txfail));
      return false;
    }
    //data tx here 图片%
    for (int i = 0; i <150; i++) {
      byte[] cmd = new byte[103];
      System.arraycopy(new byte[]{(byte) 0xcd, 8, 100}, 0, cmd, 0, 3);
      System.arraycopy(pic_send, i*100, cmd, 3, 100);

      //setStatusBody(getString(R.string.txing) + i * 100 / 400+"%");
      response = tntag.transceive(cmd);
      Log.v("pic_send000000000","cmd="+Arrays.toString(cmd));
      if ((response[0] != (byte) 0x00) || (response[1] != (byte) 0x00)) {
        setStatusBody(getString(R.string.txfail));
        return false;
      }
    }
    //Refresh eink
    response = tntag.transceive(new byte[]{(byte) 0xcd, 9});
    if ((response[0] != (byte) 0x00) || (response[1] != (byte) 0x00)) {
      setStatusBody(getString(R.string.txfail));
      Log.v("onnnnn", "Refresh");
      return false;
    }
    SystemClock.sleep(200);
    //wait for ready
    // setStatusBody(getString(R.string.refresh));
    int i=0;
    while (true) {
      i++;
      response = tntag.transceive(new byte[]{(byte) 0xcd, 0x0a});
      if ((response[0] == (byte) 0xff) && (response[1] == (byte) 0x00)) {
        setStatusBody(getString(R.string.txing) + "100%");
        break;
      }
      if(i<100){
        setStatusBody(getString(R.string.txing) + i+"%");
      }
      // Log.v("onnnnn", "wait for ready"+i);
      SystemClock.sleep(25);
    }
    //Eink power off
    response = tntag.transceive(new byte[]{(byte) 0xcd, 4});
    if ((response[0] != (byte) 0x00) || (response[1] != (byte) 0x00)) {
      setStatusBody(getString(R.string.txfail));
      //Log.v("onnnnn", "power off");
      return false;
    }
    return true;
  } catch (IOException e) {
    e.printStackTrace();
  }
  return false;
}
```

</details>

## 黑白紅三色 1.54inch NFC-Powered e-Paper (BW)
  
* `size_flag = 6`

```
0xCD0D // 1. NFC driver board power on, 接收成功返回 0x0000, retry 10
0xCD00 // 2. NFCTag reset and choose e-Paper, 接收成功返回 0x0000, retry 10
0xCD01 // 3. NFCTag configure command 1, 接收成功返回 0x0000, retry 10
0xCD02 // 4. NFCTag configure command 2, 接收成功返回 0x0000, retry 10
0xCD03 // 5. NFCTag Power on, 接收成功返回 0x0000, retry 10
0xCD05 + 圖像數據長度 + 圖像數據 // 6. NFCTag send black data
0xCD04 // 7. NFCTag cofngiure command 3, 接收成功返回 0x0000, retry 10
0xCD05 + 圖像數據長度 + 圖像數據 // 8. NFCTag send red data
0xCD06 // 9. NFCTag cofngiure command 4, 接收成功返回 0x0000, retry 10
0xCD08 // 10. 等待 NFCTag 刷屏成功, 接收成功返回 0xFF00, retry 70
0xCD04 // 11. 發送關閉 NFCTag 電源指令
```
