# Android用ショートカット

BT3（4番目の接続先）を選択するとAndroidレイヤー1が有効になります。
変換先は、提供されたSamsung端末のショートカット設定画面に合わせています。
他のAndroid機種に共通する仕様ではありません。履歴操作は実機で正常に動作している通常のAlt + Tabを使います。

## 操作

| roBaでの操作 | Androidへ送る操作 | 目的 |
|---|---|---|
| Alt + Tab | Alt + Tab（変換なし） | 履歴・アプリ切替 |
| Win + Tab | Cmd + Tab（変換なし） | エッジパネルの割当は保留 |
| 右Win + D | Cmd + Enter | ホーム（Windowsのデスクトップ表示に対応） |
| 右Win + A | Cmd + N | 通知 |
| 右Win + L | Cmd + L | 画面OFF |
| 右Win + Shift + L | Cmd + Shift + L | ロック画面 |
| 左Win + O | Cmd + O | Gemini（端末側の割当） |
| Win + Space | Ctrl + Space | 言語切替 |
| Win + ↑ | Cmd + Ctrl + ↑ | ウィンドウ最大化 |
| Win + ↓ | Cmd + Ctrl + ↓ | ポップアップ表示（最小化とは異なる） |
| Win + ← / → | Cmd + Ctrl + ← / → | 分割画面 |
| Alt + ← | Cmd + Backspace | 戻る |
| Alt + F4 | Alt + F4 | 現在のアプリを終了 |

CmdはWinと同じGUI修飾キーです。左Winは数字レイヤー6も開きます。右Winは文字配置を維持します。
Win + SpaceではGUIを取り除いてCtrl + Spaceを送ります。

履歴はAltを保持しながらTabで候補を切り替え、Altを離して確定します。
WinとTabの押下状態を管理する独自処理は撤去しました。
Win + Tabには独自の動作を割り当てず、そのままCmd + Tabを端末へ送ります。
エッジパネルを開く標準キーコンボは確認できていないため、割当は保留です。
Samsung公式案内はハンドルのスワイプ操作を説明しています。
https://www.samsung.com/uk/support/mobile-devices/what-is-the-edge-panel-and-how-do-i-use-it/

## 矢印とレイヤー

- 左Winを押すと、Windows用配列と同じく数字レイヤー6＋Winが有効になります。
- Enter長押しで数字レイヤーを開き、I/J/K/Lの位置を↑/←/↓/→として使います。
- 例：左Win + Lの位置 → Cmd + Ctrl + →（右へ分割）。Enter長押しは不要です。
- 右Win + Lは画面OFFです。右Win + D/Aもホーム／通知として使えます。左Win側の同じ物理位置は数字・矢印になります。
- 左Win + OはCmd + Oを送ります。WinなしのEnter長押し + OはPage Upを維持します。
- O長押しのARROWと手動マウスのM_MOUSEはWindowsと共通です。Android専用の矢印変換は適用しません。
- Bluetooth切替はSpace長押し + Enter長押しで開きます。Win + Spaceは言語切替なので、Winを離して操作してください。

通常の文字入力、Ctrl・Alt・Shift、Tab長押しのマウスレイヤー、Space長押しのFunctionレイヤーは維持しています。
トラックボールの自動マウスレイヤーが有効な間は、そのマウスボタン配置が優先されます。
Windows側（レイヤー0）の配列とWinキーの数字レイヤー動作は変更していません。

## 変換範囲

上表は機能が近い操作を対応させたものです。ホームとデスクトップ、ポップアップ表示と最小化は厳密には異なります。
スクリーンショット、DeX、カメラ・マイク切替など、Windowsと一対一に対応しない操作は新たに変換していません。
既存のスクリーンショットキー（Win + Shift + S）はそのままです。
マウスボタン4は既存配置のまま、ボタン5は未配置です。

## 確認項目

1. BT3へ切り替えて文字・Tab・Spaceと各長押しが使えること。
2. 通常のAlt + Tabで候補を切り替え、Altを離すと確定できること。Win + Tabは独自変換を行わないこと。
3. 右Win + D/A/LとWin + Spaceが上表の操作になること。
4. 左Win + I/J/K/Lでウィンドウ操作ができ、右Win + Lは画面OFFになること。Enter長押しの通常数字・矢印も使えること。
5. NUMBER上のAlt + ←が戻る操作になり、ARROW・M_MOUSEでは共通のキー入力が維持されること。
6. Androidから5つの接続先を選べて、他の接続先では通常配列へ戻ること。

ZMK Studioで保存した配列がある場合、書き込んだソースの配列と異なる可能性があります。
動作が一致しない場合は、保存内容を確認してからStudioでキーマップを初期状態へ戻してください。

## Android専用の補助レイヤー

基本配列のANDROID（1）に加えて、ANDROID_NUMBER（9）だけを自動で重ねます。
Android（1）と数字（6）が同時に有効なとき、I/J/K/Lの矢印とO位置をAndroid向けに変換します。残りのキーは共通の数字レイヤーを使用します。

ARROW（7）とM_MOUSE（3）はWindowsと共通です。専用の補助レイヤーはありません。
Bluetooth・音量も共通のMEDIA_BT（10）を使います。Function（5）＋数字（6）で有効になり、ANDROID_NUMBERより優先されるため、Bluetooth選択が矢印で隠れません。

キー操作はZMK標準のマクロ、レイヤー、mod-morphだけで構成しています。独自のsrc・動作定義・専用テストは不要です。
