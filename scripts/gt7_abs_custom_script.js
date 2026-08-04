// --- 參數設定 ---
var tireRadius = 0.30;   
var triggerDiff = 7.0;    
var minSpeed = 15;
var sustainMs = 200;      // 震動延長時間 (毫秒)，若覺得不夠長可以改為 300

// 初始化計時器 (存放在 root 確保跨格紀錄)
if (root.lastAbsTrigger == null) { root.lastAbsTrigger = 0; }

var vSpeed = $prop('SpeedKmh'); 
var brake = $prop('Brake');
var now = Date.now();

// 讀取四輪數據
var w01 = $prop('DataCorePlugin.GameRawData.Wheel_Speed01'); 
var w02 = $prop('DataCorePlugin.GameRawData.Wheel_Speed02'); 
var w03 = $prop('DataCorePlugin.GameRawData.Wheel_Speed03'); 
var w04 = $prop('DataCorePlugin.GameRawData.Wheel_Speed04'); 

if (w01 === null || vSpeed < minSpeed) return 0;

// 計算最大速度差
function getDiff(wheelRad) {
    return vSpeed - (wheelRad * tireRadius * 3.6);
}
var maxDiff = Math.max(getDiff(w01), getDiff(w02), getDiff(w03), getDiff(w04));

// --- 觸發與延長邏輯 ---
// 只要「正在打滑」或是「距離上次打滑不到 sustainMs 毫秒」
if ((maxDiff > triggerDiff && brake > 10) || (now - root.lastAbsTrigger < sustainMs)) {
    
    // 如果是新的打滑，更新計時器
    if (maxDiff > triggerDiff && brake > 10) {
        root.lastAbsTrigger = now;
    }

    // 輸出震動：加入正弦波 Math.sin 讓震動有起伏感，比單純隨機更像機械跳動
    var wave = Math.sin(now / 20) * 20; // 產生 -20 到 20 的波動
    return 70 + wave + (Math.random() * 10); 
}

return 0;