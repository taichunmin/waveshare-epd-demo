# 微雪電子 ST25R3911B NFC Board 範例程式

* 來源 1: <https://www.waveshare.net/wiki/ST25R3911B_NFC_Board>
* 來源 2: <https://github.com/waveshare/e-Paper>

## 文件

* [原理圖](https://www.waveshare.net/w/upload/d/da/ST25R3911B_NFC_Board_Schematic.pdf)
* [X-CUBE-NFC5函數說明手冊](https://www.waveshare.net/w/upload/0/01/X-CUBE-NFC5.7z)
* [ST25R3911數據手冊(英文）](https://www.waveshare.net/w/upload/5/52/St25r3911b_EN.pdf)
* [ST25R3911數據手冊(中文）](https://www.waveshare.net/w/upload/b/bb/St25r3911b_ZH.pdf)
* [測試圖片](https://www.waveshare.net/w/upload/a/ad/Picture.zip)

## 無源 NFC 電子墨水屏 刷圖範例

* 原始碼: [`./User/Browser`](https://github.com/taichunmin/waveshare-epd-demo/tree/master/User/Browser)

### 步驟

* 用 USB 先連接讀寫器的 USB 口到電腦或者 5V 電源
* 將做好的單色位圖放置於 Micro SD 卡。將 Micro SD 卡插入 NFC Board 的卡槽中
* 連接 5V 電源到 NFC Board 的 USB 接口
* 屏幕顯示目錄，通過按鍵選擇墨水屏型號和對應的圖片刷新即可
  * 用按鍵移動選擇 Update e-Paper Demo 選項。
  * 根據屏幕的型號選擇要刷新的屏幕型號
  * 選擇圖片(注意需要選擇和屏幕分辨率一致的圖片）
  * 確定圖片
  * 將 NFC Board 的線圈放置於 4.2inch NFC-Powered e-Paper 靠近中心位置
  * 等待刷圖成功
* 如果提示刷新失敗，請拿開讀寫器重新放置感應刷新

![](https://i.imgur.com/Zz9jBxc.png)

> 由於 NFC 屬於通信距離有限， 在刷新的時候為了提高成功率，請不要輕易移動設備。

### 圖片

| ![](https://i.imgur.com/m7yXoyq.png) | ![](https://i.imgur.com/UP5glYV.png) | ![](https://i.imgur.com/t2G0FRt.png) |
| :-----: | :-----: | :-----: |
| 選擇程序 | 選擇屏幕型號 | 選擇圖片 |
| ![](https://i.imgur.com/50EMj0Q.png) | ![](https://i.imgur.com/UOqRFMn.png) | ![](https://i.imgur.com/jQtSJQf.png) |
| 確認選擇 | 從TF卡讀取圖片 | 提示將讀寫器線圈貼到屏幕 |
| ![](https://i.imgur.com/0UZR2pv.png) | ![](https://i.imgur.com/34BZSAD.png) | ![](https://i.imgur.com/IWLhJN0.png) |
| 感應屏幕成功 | 配置屏幕 | 刷新中 |
| ![](https://i.imgur.com/SDcyZaE.png) |  |  |
| 刷新成功，按鍵退出 |  |  |

## 墨水屏刷新原理 (1.54 黑白紅)

資料來源: <https://www.waveshare.net/wiki/1.54inch_e-Paper_Module_(B)>

### 工作原理

本產品使用的電子紙採用“微膠囊電泳顯示”技術進行圖像顯示，其基本原理是懸浮在液體中的帶電納米粒子受到電場作用而產生遷移。電子紙顯示屏是靠反射環境光來顯示圖案的，不需要背光，在環境光下，電子紙顯示屏清晰可視，可視角度幾乎達到了 180°。因此，電子紙顯示屏非常適合閱讀。

### 像素與字節的關系

對於黑白紅三色，可以拆分為黑白圖片、紅白圖片，然後將兩張圖片疊加起來就變成了黑白紅圖片了。

對於黑白圖片，我們可以規定，如果如果是黑色我們定義成0，如果是白色就定義成1，那麼有了表示顏色的方式：

```
白色：□，對應1
黑色：■，對應0
```

一個點在圖形上一般稱之為像素點（pixel）,而顏色不是1就是0，也就是1個位就可以標識顏色：1Pixel = 1bit，那麼一個字節裡面就包含了8個像素點。

以16個像素點為例，我們假設前8個像素點為黑，後8個像素點為白色，那麼可以這麼認為，像素點1-16，對應這0位到15位，0表示黑色，1表示白色：

![](https://i.imgur.com/2kQvqwj.png)

對於計算機而言，它的數據存儲方式是高位在前，低位在後，且一個字節只有8個位，因此會有一點改變：

![](https://i.imgur.com/HYrgYSt.png)

這樣只需要2個字節即可表示16個像素點了。

1.54inch e-paper B 是紅黑白三色，我們需要把圖片拆成2張圖片，一張黑白圖片，一張紅白圖片，在傳輸的時候因為一個寄存器是控制黑白顯示的，一個寄存器是控制紅白顯示。

有一點特殊的是，1.54的黑白部分1個字節裡面需要2個位才能控制1個像素點，而控制紅白1個字節可以控制8個像素點。舉個栗子，假設有8個像素點，前面4個是紅色，後面4個黑色：需要把他們拆成一個黑白圖片，一個紅白圖片，這兩個圖片都是8個像素點，只不過黑白圖片前四個像素為白色，後4個像素點為黑色，而紅白圖片前4個像素點為紅色，後四個像素點為白色。

![](https://i.imgur.com/akReUhf.png)

如果我們規定，白色存儲為1，紅色或者黑色存儲為0，那麼就有了如下的表示：

![](https://i.imgur.com/4NJrQC5.png)

上面說了1.54的黑白部分需要2個位才能控制1個像素點，那麼表示8個像素點需要16個位才能表示，也就是2個字節，規定2個位同時為1表示；控制紅白1個字節可以控制8個像素點，需要1個字節表示；

![](https://i.imgur.com/9KFNAKZ.png)