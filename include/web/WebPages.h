/**
 * @file WebPages.h
 * @brief Web页面HTML内容
 */

#ifndef WEB_PAGES_H
#define WEB_PAGES_H

const char HTML_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>BottleBoy</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }

        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 12px;
        }

        .container { max-width: 600px; margin: 0 auto; }

        /* ── Header ── */
        .header {
            text-align: center;
            color: white;
            margin-bottom: 14px;
        }
        .header h1 { font-size: 1.6em; margin-bottom: 6px; }
        .status {
            display: inline-block;
            padding: 4px 14px;
            background: rgba(255,255,255,0.2);
            border-radius: 20px;
            font-size: 0.85em;
            color: white;
        }
        .status.connected  { background: rgba(76,175,80,0.85); }
        .status.disconnected { background: rgba(244,67,54,0.85); }

        /* ── Section title ── */
        .section-title {
            color: rgba(255,255,255,0.9);
            font-size: 1em;
            font-weight: 600;
            margin: 14px 0 8px 2px;
            letter-spacing: 0.02em;
        }

        /* ── Card ── */
        .card {
            background: white;
            border-radius: 14px;
            padding: 18px;
            box-shadow: 0 6px 20px rgba(0,0,0,0.12);
            margin-bottom: 12px;
        }
        .card h3 {
            color: #667eea;
            font-size: 1em;
            margin-bottom: 14px;
            display: flex;
            align-items: center;
            gap: 8px;
        }

        /* ── DHT22 ── */
        .sensor-dual {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 12px;
            margin-top: 4px;
        }
        .sensor-item { text-align: center; }
        .sensor-item-label { color: #999; font-size: 0.8em; margin-bottom: 4px; }
        .sensor-item-value { font-size: 2em; font-weight: 700; color: #333; }
        .sensor-unit { color: #bbb; font-size: 0.8em; }

        /* ── Toggle row ── */
        .toggle-row {
            display: flex;
            align-items: center;
            justify-content: space-between;
            padding: 6px 0;
        }
        .toggle-label { color: #444; font-size: 0.95em; }
        .toggle-switch { position: relative; width: 52px; height: 28px; flex-shrink: 0; }
        .toggle-switch input { opacity: 0; width: 0; height: 0; }
        .toggle-track {
            position: absolute; inset: 0;
            background: #ccc;
            border-radius: 28px;
            cursor: pointer;
            transition: background 0.25s;
        }
        .toggle-track:before {
            content: '';
            position: absolute;
            width: 22px; height: 22px;
            left: 3px; top: 3px;
            background: white;
            border-radius: 50%;
            transition: transform 0.25s;
        }
        .toggle-switch input:checked + .toggle-track { background: #667eea; }
        .toggle-switch input:checked + .toggle-track:before { transform: translateX(24px); }

        /* ── Slider ── */
        .control-group { margin: 10px 0; }
        .control-label { display: block; color: #666; font-size: 0.85em; margin-bottom: 6px; }
        .slider-container { display: flex; align-items: center; gap: 10px; }
        .slider {
            flex: 1; height: 8px; border-radius: 5px;
            background: #e0e0e0; outline: none;
            -webkit-appearance: none;
        }
        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 24px; height: 24px;
            border-radius: 50%; background: #667eea; cursor: pointer;
        }
        .slider::-moz-range-thumb {
            width: 24px; height: 24px;
            border-radius: 50%; background: #667eea;
            cursor: pointer; border: none;
        }
        .slider-value { min-width: 36px; text-align: right; color: #555; font-size: 0.9em; font-weight: 500; }

        /* ── Buttons ── */
        .btn-group { display: flex; gap: 10px; margin-top: 14px; }
        .btn {
            flex: 1; padding: 14px 8px;
            border: none; border-radius: 10px;
            font-size: 0.95em; font-weight: 600;
            cursor: pointer; transition: all 0.15s;
            min-height: 48px;
        }
        .btn-primary  { background: #667eea; color: white; }
        .btn-primary:active  { background: #4f63d2; }
        .btn-success  { background: #4CAF50; color: white; }
        .btn-success:active  { background: #3d9142; }
        .btn-danger   { background: #f44336; color: white; }
        .btn-danger:active   { background: #d32f2f; }
        .btn-gray     { background: #f0f0f0; color: #555; }
        .btn-gray:active     { background: #e0e0e0; }

        /* ── LED status bar ── */
        .led-status {
            display: flex;
            gap: 8px;
            flex-wrap: wrap;
            margin-top: 12px;
        }
        .led-badge {
            display: inline-flex;
            align-items: center;
            gap: 4px;
            padding: 4px 10px;
            border-radius: 20px;
            font-size: 0.8em;
            font-weight: 500;
        }
        .badge-night  { background: #e8eaf6; color: #3949ab; }
        .badge-day    { background: #fff8e1; color: #f57f17; }
        .badge-pir-on { background: #e8f5e9; color: #2e7d32; }
        .badge-pir-off{ background: #f5f5f5; color: #9e9e9e; }
        .badge-btn-on { background: #fce4ec; color: #c62828; }
        .badge-btn-off{ background: #f5f5f5; color: #9e9e9e; }

        /* ── Light sensor bar ── */
        .light-bar-wrap { margin-top: 8px; }
        .light-bar-bg {
            width: 100%; height: 10px;
            background: #eee; border-radius: 5px;
            overflow: hidden;
        }
        .light-bar-fill {
            height: 100%; border-radius: 5px;
            background: linear-gradient(90deg, #ffd54f, #fff176);
            transition: width 0.4s;
        }
        .light-bar-label {
            display: flex; justify-content: space-between;
            margin-top: 6px; font-size: 0.82em; color: #888;
        }

        /* ── RF quick panel ── */
        .rf-quick-grid {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(120px, 1fr));
            gap: 8px;
        }
        .rf-quick-btn {
            padding: 12px 8px;
            background: #f0f2ff;
            border: 1.5px solid #c5cae9;
            border-radius: 10px;
            color: #3949ab;
            font-size: 0.9em;
            font-weight: 600;
            cursor: pointer;
            text-align: center;
            transition: all 0.15s;
        }
        .rf-quick-btn:active { background: #c5cae9; }
        .rf-empty { color: #bbb; font-size: 0.88em; padding: 8px 0; }

        /* ── RF 学习器 ── */
        .rf-learn-bar {
            display: flex; gap: 8px; margin-bottom: 12px; flex-wrap: wrap; align-items: center;
        }
        .rf-learn-bar input[type=text] {
            flex: 1; min-width: 110px;
            padding: 10px 12px;
            border: 1.5px solid #ddd; border-radius: 8px;
            font-size: 0.95em; outline: none;
        }
        .rf-learn-bar input[type=text]:focus { border-color: #667eea; }
        .rf-status {
            margin: 0 0 12px; padding: 8px 12px;
            border-radius: 8px; font-size: 0.88em;
            background: #f5f5f5; color: #555;
            display: flex; align-items: center; gap: 8px;
        }
        .rf-status.waiting { background: #fff8e1; color: #e65100; }
        .rf-status.success { background: #e8f5e9; color: #2e7d32; }
        .rf-status.timeout { background: #ffebee; color: #c62828; }
        .rf-device-list { list-style: none; padding: 0; margin: 0; }
        .rf-device-item {
            display: flex; align-items: center;
            justify-content: space-between;
            padding: 10px 10px;
            border-radius: 10px; background: #f7f8fc;
            margin-bottom: 6px; gap: 8px;
        }
        .rf-device-name {
            font-weight: 600; color: #333; flex: 1;
            font-size: 0.9em; overflow: hidden;
            text-overflow: ellipsis; white-space: nowrap;
        }
        .rf-device-meta { font-size: 0.76em; color: #bbb; white-space: nowrap; }
        .rf-device-btns { display: flex; gap: 6px; flex-shrink: 0; }
        .btn-rf-send {
            padding: 6px 14px; background: #667eea; color: white;
            border: none; border-radius: 7px; cursor: pointer;
            font-size: 0.85em; font-weight: 600;
        }
        .btn-rf-send:active { background: #4f63d2; }
        .btn-rf-del {
            padding: 6px 10px; background: #fff0f0; color: #e53935;
            border: 1px solid #ffcdd2; border-radius: 7px; cursor: pointer;
            font-size: 0.85em;
        }
        .btn-rf-del:active { background: #ffcdd2; }

        /* ── TFT ── */
        .tft-card h3 { margin-bottom: 14px; }

        /* ── System info ── */
        .info-row {
            display: flex; justify-content: space-between;
            padding: 9px 0; border-bottom: 1px solid #f0f0f0;
            font-size: 0.9em;
        }
        .info-row:last-child { border-bottom: none; }
        .info-label { color: #888; }
        .info-value { color: #333; font-weight: 500; }

        .divider {
            height: 1px; background: #f0f0f0; margin: 12px 0;
        }
    </style>
</head>
<body>
<div class="container">

    <!-- Header -->
    <div class="header">
        <h1>🍼 BottleBoy</h1>
        <div id="status" class="status disconnected">连接中...</div>
    </div>

    <!-- 1. 温湿度 -->
    <h2 class="section-title">🌡️ 环境</h2>
    <div id="card-dht" class="card" style="display:none">
        <h3>🌡️ 温湿度</h3>
        <div class="sensor-dual">
            <div class="sensor-item">
                <div class="sensor-item-label">温度</div>
                <div class="sensor-item-value" id="dht-temp">--</div>
                <div class="sensor-unit">°C</div>
            </div>
            <div class="sensor-item">
                <div class="sensor-item-label">湿度</div>
                <div class="sensor-item-value" id="dht-humi">--</div>
                <div class="sensor-unit">%</div>
            </div>
        </div>
    </div>

    <!-- 2. RF 快捷操作 -->
    <h2 class="section-title">📡 RF 快捷</h2>
    <div class="card">
        <h3>📡 快捷发射</h3>
        <div class="rf-quick-grid" id="rf-quick-grid">
            <span class="rf-empty">暂无已学设备</span>
        </div>
    </div>

    <!-- 3. LED 控制 -->
    <h2 class="section-title">💡 灯光</h2>
    <div id="card-led" class="card" style="display:none">
        <h3>💡 LED 控制</h3>

        <!-- 感应模式开关 -->
        <div class="toggle-row">
            <span class="toggle-label">感应自动模式</span>
            <label class="toggle-switch">
                <input type="checkbox" id="pir-mode-toggle" checked
                       onchange="sendControl({cmd:'pirMode',enabled:this.checked})">
                <span class="toggle-track"></span>
            </label>
        </div>

        <div class="divider"></div>

        <!-- 亮度 -->
        <div class="control-group">
            <label class="control-label">亮度 <span id="led-brightness-val">--</span></label>
            <div class="slider-container">
                <input type="range" min="0" max="255" value="180"
                       class="slider" id="led-brightness"
                       oninput="previewBrightness('DualLED', this.value)"
                       onchange="sendBrightness('DualLED', this.value)">
                <span class="slider-value" id="led-brightness-display">--</span>
            </div>
        </div>

        <!-- 色温 -->
        <div class="control-group">
            <label class="control-label">色温 <span id="led-colortemp-val">--</span></label>
            <div class="slider-container">
                <span style="font-size:1.1em">🔥</span>
                <input type="range" min="0" max="100" value="20"
                       class="slider" id="led-colortemp"
                       oninput="previewColorTemp('DualLED', this.value)"
                       onchange="sendColorTemp('DualLED', this.value)">
                <span style="font-size:1.1em">❄️</span>
            </div>
        </div>

        <!-- 开关按钮 -->
        <div class="btn-group">
            <button id="led-toggle-btn" class="btn btn-success" onclick="toggleLED('DualLED')">开灯</button>
        </div>

        <!-- 状态栏 -->
        <div class="led-status" id="led-status-bar">
            <span class="led-badge badge-day" id="badge-light">☀️ 白天</span>
            <span class="led-badge badge-pir-off" id="badge-pir">👤 无人</span>
            <span class="led-badge badge-btn-off" id="badge-btn">🔘 自动</span>
        </div>
    </div>

    <!-- 5. 光敏 -->
    <h2 class="section-title">🌤️ 光线</h2>
    <div id="card-light" class="card" style="display:none">
        <h3>🌤️ 环境光线</h3>
        <div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:8px">
            <span style="font-size:1.4em;font-weight:700;color:#333" id="light-percent">--%</span>
            <span id="light-state-badge" class="led-badge badge-day">☀️ 白天</span>
        </div>
        <div class="light-bar-wrap">
            <div class="light-bar-bg">
                <div class="light-bar-fill" id="light-bar" style="width:0%"></div>
            </div>
            <div class="light-bar-label">
                <span>🌙 夜晚</span><span>☀️ 明亮</span>
            </div>
        </div>
    </div>

    <!-- 6. TFT 屏幕 -->
    <h2 class="section-title">🖥️ 屏幕</h2>
    <div class="card tft-card">
        <h3>📺 TFT 显示屏</h3>

        <div class="toggle-row">
            <span class="toggle-label">屏幕开关</span>
            <label class="toggle-switch">
                <input type="checkbox" id="tft-power" onchange="sendTft({state: this.checked ? 'on' : 'off'})">
                <span class="toggle-track"></span>
            </label>
        </div>

        <div class="control-group" style="margin-top:10px">
            <label class="control-label">背光亮度 <span id="tft-brightness-val">255</span></label>
            <div class="slider-container">
                <input type="range" min="0" max="255" value="255"
                       class="slider" id="tft-brightness"
                       oninput="document.getElementById('tft-brightness-val').textContent=this.value"
                       onchange="sendTft({brightness: parseInt(this.value)})">
                <span class="slider-value" id="tft-brightness-display">255</span>
            </div>
        </div>

        <div class="toggle-row" style="margin-top:4px">
            <span class="toggle-label">夜间模式 <small style="color:#aaa">（黑底琥珀橙）</small></span>
            <label class="toggle-switch">
                <input type="checkbox" id="tft-night" onchange="sendTft({nightMode: this.checked})">
                <span class="toggle-track"></span>
            </label>
        </div>

        <div style="margin-top:12px">
            <button class="btn btn-gray" style="width:auto;padding:8px 16px;font-size:0.85em;border-radius:8px;min-height:0"
                    onclick="sendTft({autoMode:true})">
                🔄 跟随光线自动
            </button>
        </div>
    </div>

    <!-- 7. RF 学习器 -->
    <h2 class="section-title">📡 RF 学习</h2>
    <div class="card">
        <h3>📡 RF 遥控学习器</h3>
        <div class="rf-learn-bar">
            <input type="text" id="rf-name-input" placeholder="设备名称，如：灯1" maxlength="30">
            <button class="btn btn-success" style="flex:0;white-space:nowrap;padding:10px 14px;min-height:0;border-radius:8px;font-size:0.9em" onclick="rfStartLearn()">🎯 学习</button>
            <button class="btn btn-gray"    style="flex:0;white-space:nowrap;padding:10px 14px;min-height:0;border-radius:8px;font-size:0.9em" onclick="rfCancelLearn()">✖</button>
        </div>
        <div class="rf-status" id="rf-status">
            <span id="rf-status-icon">ℹ️</span>
            <span id="rf-status-text">输入名称后点击"学习"，然后按遥控器按键</span>
        </div>
        <div style="font-weight:600;color:#888;font-size:0.82em;margin-bottom:6px">已学设备</div>
        <ul class="rf-device-list" id="rf-device-list">
            <li style="color:#ccc;font-size:0.88em;padding:6px 0">暂无设备</li>
        </ul>
    </div>

    <!-- 8. 系统信息 -->
    <h2 class="section-title">⚙️ 系统</h2>
    <div class="card">
        <div class="info-row">
            <span class="info-label">运行时间</span>
            <span class="info-value" id="uptime">--</span>
        </div>
        <div class="info-row">
            <span class="info-label">可用内存</span>
            <span class="info-value" id="freeHeap">--</span>
        </div>
        <div class="info-row">
            <span class="info-label">芯片型号</span>
            <span class="info-value" id="chipModel">--</span>
        </div>
        <div class="info-row">
            <span class="info-label">CPU 频率</span>
            <span class="info-value" id="cpuFreq">--</span>
        </div>
    </div>

</div>

<script>
    let ws;
    let actuatorStates = {};
    let isDragging = {};
    let tftBrightnessDragging = false;

    // ── WebSocket ──────────────────────────────
    function connectWebSocket() {
        const protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
        ws = new WebSocket(`${protocol}//${location.host}/ws`);

        ws.onopen = () => {
            document.getElementById('status').textContent = '已连接';
            document.getElementById('status').className = 'status connected';
        };

        ws.onmessage = (event) => {
            const data = JSON.parse(event.data);

            // 传感器
            if (data.sensors) updateSensors(data.sensors);

            // 执行器
            if (data.actuators) {
                data.actuators.forEach(a => {
                    actuatorStates[a.name] = a.state;
                    if (a.type === 'dual_color_led') updateLEDCard(a);
                });
            }

            // 感应模式开关同步
            if (data.automation && data.automation.pirEnabled !== undefined) {
                window._pirEnabled = data.automation.pirEnabled;
                const t = document.getElementById('pir-mode-toggle');
                if (t) t.checked = data.automation.pirEnabled;
            }

            // PIR 状态徽章
            if (data.automation && data.automation.pirDetected !== undefined) {
                updatePIRBadge(data.automation.pirDetected);
            }

            // TFT 同步
            if (data.tft) {
                const t = data.tft;
                const powerEl = document.getElementById('tft-power');
                const nightEl = document.getElementById('tft-night');
                const bEl = document.getElementById('tft-brightness');
                const bValEl = document.getElementById('tft-brightness-val');
                const bDispEl = document.getElementById('tft-brightness-display');
                if (powerEl) powerEl.checked = t.on;
                if (nightEl) nightEl.checked = t.nightMode;
                if (bEl && !tftBrightnessDragging) {
                    bEl.value = t.brightness;
                    if (bValEl) bValEl.textContent = t.brightness;
                    if (bDispEl) bDispEl.textContent = t.brightness;
                }
            }

            // 系统信息
            if (data.uptime !== undefined) {
                document.getElementById('uptime').textContent = formatUptime(data.uptime);
                document.getElementById('freeHeap').textContent = formatBytes(data.freeHeap);
                document.getElementById('chipModel').textContent = data.chipModel;
                document.getElementById('cpuFreq').textContent = data.cpuFreq + ' MHz';
            }

            // RF
            if (data.rf) updateRF(data.rf);
        };

        ws.onerror = () => {
            document.getElementById('status').textContent = '连接错误';
            document.getElementById('status').className = 'status disconnected';
        };

        ws.onclose = () => {
            document.getElementById('status').textContent = '已断开';
            document.getElementById('status').className = 'status disconnected';
            setTimeout(connectWebSocket, 5000);
        };
    }

    function sendControl(payload) {
        if (ws && ws.readyState === WebSocket.OPEN) ws.send(JSON.stringify(payload));
    }

    // ── Sensors ────────────────────────────────
    function updateSensors(sensors) {
        sensors.forEach(s => {
            if (s.type === 'dht22') {
                document.getElementById('card-dht').style.display = '';
                document.getElementById('dht-temp').textContent = s.temperature != null ? s.temperature.toFixed(1) : '--';
                document.getElementById('dht-humi').textContent = s.humidity    != null ? s.humidity.toFixed(1)    : '--';
            }
            else if (s.type === 'light') {
                document.getElementById('card-light').style.display = '';
                const pct = s.percent != null ? s.percent : 0;
                document.getElementById('light-percent').textContent = pct + '%';
                document.getElementById('light-bar').style.width = pct + '%';

                const isNight = s.isNight;
                const stBadge = document.getElementById('light-state-badge');
                stBadge.textContent = isNight ? '🌙 夜间' : '☀️ 白天';
                stBadge.className = 'led-badge ' + (isNight ? 'badge-night' : 'badge-day');

                // 同步 LED 状态栏的光线徽章
                const bl = document.getElementById('badge-light');
                if (bl) {
                    bl.textContent = isNight ? '🌙 夜间' : '☀️ 白天';
                    bl.className = 'led-badge ' + (isNight ? 'badge-night' : 'badge-day');
                }
            }
            else if (s.type === 'latch_button') {
                // 同步 LED 状态栏的按钮徽章
                const bb = document.getElementById('badge-btn');
                if (bb) {
                    bb.textContent = s.latched ? '🔒 手动' : '🔘 自动';
                    bb.className = 'led-badge ' + (s.latched ? 'badge-btn-on' : 'badge-btn-off');
                }
            }
        });
    }

    function updatePIRBadge(detected) {
        const b = document.getElementById('badge-pir');
        if (!b) return;
        b.textContent = detected ? '👤 有人' : '👤 无人';
        b.className = 'led-badge ' + (detected ? 'badge-pir-on' : 'badge-pir-off');
    }

    // ── LED card ───────────────────────────────
    function updateLEDCard(a) {
        document.getElementById('card-led').style.display = '';
        actuatorStates[a.name] = a.state;

        const isOn = a.state === 'on';
        const btn = document.getElementById('led-toggle-btn');
        if (btn) {
            btn.textContent = isOn ? '关灯' : '开灯';
            btn.className = 'btn ' + (isOn ? 'btn-danger' : 'btn-success');
        }

        if (!isDragging[a.name]) {
            const bSlider = document.getElementById('led-brightness');
            const bVal    = document.getElementById('led-brightness-val');
            const bDisp   = document.getElementById('led-brightness-display');
            if (bSlider) { bSlider.value = a.brightness || 0; }
            if (bVal)    { bVal.textContent = a.brightness || 0; }
            if (bDisp)   { bDisp.textContent = a.brightness || 0; }

            const cSlider = document.getElementById('led-colortemp');
            const cVal    = document.getElementById('led-colortemp-val');
            if (cSlider) { cSlider.value = a.colorTemp || 0; }
            if (cVal)    { cVal.textContent = colorTempDesc(a.colorTemp || 0); }
        }
    }

    function colorTempDesc(v) {
        const arr = ['暖光','暖白','中性白','冷白','纯白'];
        return arr[Math.min(Math.floor(v / 20), 4)];
    }

    function previewBrightness(name, value) {
        document.getElementById('led-brightness-val').textContent = value;
        document.getElementById('led-brightness-display').textContent = value;
        isDragging[name] = true;
    }

    function previewColorTemp(name, value) {
        document.getElementById('led-colortemp-val').textContent = colorTempDesc(parseInt(value));
        isDragging[name] = true;
    }

    function sendBrightness(name, value) {
        isDragging[name] = false;
        sendControl({ cmd: 'control', name, brightness: parseInt(value) });
    }

    function sendColorTemp(name, value) {
        isDragging[name] = false;
        sendControl({ cmd: 'control', name, colorTemp: parseInt(value) });
    }

    function toggleLED(name) {
        const cur = actuatorStates[name] || 'off';
        sendControl({ cmd: 'control', name, state: cur === 'on' ? 'off' : 'on' });
    }

    // ── TFT ────────────────────────────────────
    function sendTft(payload) {
        sendControl(Object.assign({ cmd: 'tft' }, payload));
    }

    // ── RF ─────────────────────────────────────
    function rfStartLearn() {
        const nameEl = document.getElementById('rf-name-input');
        const name = nameEl ? nameEl.value.trim() : '';
        if (!name) { rfSetStatus('error', '⚠️', '请先输入设备名称'); return; }
        sendControl({ cmd: 'rfLearn', name });
        rfSetStatus('waiting', '📡', `正在等待 "${name}" 信号，请按遥控器按键…`);
    }

    function rfCancelLearn() {
        sendControl({ cmd: 'rfCancel' });
        rfSetStatus('idle', 'ℹ️', '学习已取消');
    }

    function rfSend(name) { sendControl({ cmd: 'rfSend', name }); }

    function rfDelete(name) {
        if (!confirm(`确认删除 "${name}"？`)) return;
        sendControl({ cmd: 'rfDelete', name });
    }

    function rfSetStatus(state, icon, text) {
        const bar = document.getElementById('rf-status');
        const ico = document.getElementById('rf-status-icon');
        const txt = document.getElementById('rf-status-text');
        if (!bar) return;
        bar.className = 'rf-status ' + (state === 'error' ? 'timeout' : state);
        if (ico) ico.textContent = icon;
        if (txt) txt.textContent = text;
    }

    function updateRF(rf) {
        if (rf.state === 'waiting') {
            rfSetStatus('waiting', '📡', `正在等待 "${rf.name}" 信号，请按遥控器按键…`);
        } else if (rf.state === 'success') {
            rfSetStatus('success', '✅', `学习成功：${rf.name}`);
            const el = document.getElementById('rf-name-input');
            if (el) el.value = '';
        } else if (rf.state === 'timeout') {
            rfSetStatus('timeout', '⏱️', `学习超时：请重试`);
        } else {
            rfSetStatus('idle', 'ℹ️', '输入名称后点击"学习"，然后按遥控器按键');
        }

        if (!rf.devices) return;

        // 更新学习器列表
        const list = document.getElementById('rf-device-list');
        if (list) {
            if (rf.devices.length === 0) {
                list.innerHTML = '<li style="color:#ccc;font-size:0.88em;padding:6px 0">暂无设备</li>';
            } else {
                list.innerHTML = rf.devices.map(dev => `
                    <li class="rf-device-item">
                        <span class="rf-device-name">${dev.name}</span>
                        <span class="rf-device-meta">协议${dev.protocol}·${dev.bitLength}bit</span>
                        <div class="rf-device-btns">
                            <button class="btn-rf-send" onclick="rfSend('${dev.name.replace(/'/g,"\\'")}')">▶ 发射</button>
                            <button class="btn-rf-del"  onclick="rfDelete('${dev.name.replace(/'/g,"\\'")}')">🗑</button>
                        </div>
                    </li>
                `).join('');
            }
        }

        // 更新顶部 RF 快捷网格
        const grid = document.getElementById('rf-quick-grid');
        if (grid) {
            if (rf.devices.length === 0) {
                grid.innerHTML = '<span class="rf-empty">暂无已学设备</span>';
            } else {
                grid.innerHTML = rf.devices.map(dev => `
                    <button class="rf-quick-btn" onclick="rfSend('${dev.name.replace(/'/g,"\\'")}')">
                        📡 ${dev.name}
                    </button>
                `).join('');
            }
        }
    }

    // ── Helpers ────────────────────────────────
    function formatUptime(s) {
        const d = Math.floor(s/86400), h = Math.floor((s%86400)/3600), m = Math.floor((s%3600)/60), sc = s%60;
        if (d > 0) return `${d}天 ${h}小时`;
        if (h > 0) return `${h}小时 ${m}分钟`;
        if (m > 0) return `${m}分钟 ${sc}秒`;
        return `${sc}秒`;
    }

    function formatBytes(b) {
        if (b < 1024) return b + ' B';
        if (b < 1048576) return (b/1024).toFixed(1) + ' KB';
        return (b/1048576).toFixed(1) + ' MB';
    }

    // ── Init ───────────────────────────────────
    document.addEventListener('DOMContentLoaded', () => {
        const bSlider = document.getElementById('tft-brightness');
        if (bSlider) {
            bSlider.addEventListener('mousedown',  () => tftBrightnessDragging = true);
            bSlider.addEventListener('touchstart', () => tftBrightnessDragging = true);
            bSlider.addEventListener('mouseup',    () => tftBrightnessDragging = false);
            bSlider.addEventListener('touchend',   () => tftBrightnessDragging = false);
        }
    });

    connectWebSocket();
    setTimeout(() => sendControl({ cmd: 'getData' }), 600);
    setInterval(() => {
        if (ws && ws.readyState === WebSocket.OPEN && !Object.values(isDragging).some(Boolean)) {
            sendControl({ cmd: 'getData' });
        }
    }, 10000);
</script>
</body>
</html>
)rawliteral";

#endif // WEB_PAGES_H
