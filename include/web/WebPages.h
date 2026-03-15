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
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>BottleBoy - 智能监控</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        
        .header {
            text-align: center;
            color: white;
            margin-bottom: 30px;
        }
        
        .header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
        }
        
        .status {
            display: inline-block;
            padding: 5px 15px;
            background: rgba(255, 255, 255, 0.2);
            border-radius: 20px;
            font-size: 0.9em;
        }
        
        .status.connected {
            background: rgba(76, 175, 80, 0.8);
        }
        
        .status.disconnected {
            background: rgba(244, 67, 54, 0.8);
        }
        
        .section-title {
            color: white;
            font-size: 1.5em;
            margin: 20px 0 15px 0;
            text-align: left;
        }
        
        .dashboard {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        
        .card {
            background: white;
            border-radius: 15px;
            padding: 25px;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.2);
            transition: transform 0.3s ease;
        }
        
        .card:hover {
            transform: translateY(-5px);
        }
        
        .card h3 {
            color: #667eea;
            margin-bottom: 15px;
            font-size: 1.2em;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        
        .sensor-value {
            font-size: 2.5em;
            font-weight: bold;
            color: #333;
            margin: 10px 0;
        }
        
        .sensor-dual {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
            margin: 15px 0;
        }
        
        .sensor-item {
            text-align: center;
        }
        
        .sensor-item-label {
            color: #999;
            font-size: 0.85em;
            margin-bottom: 5px;
        }
        
        .sensor-item-value {
            font-size: 1.8em;
            font-weight: bold;
            color: #333;
        }
        
        .sensor-unit {
            color: #999;
            font-size: 0.9em;
        }
        
        .sensor-status {
            display: inline-block;
            padding: 5px 15px;
            border-radius: 15px;
            font-size: 0.9em;
            margin-top: 10px;
        }
        
        .sensor-status.active {
            background: #4CAF50;
            color: white;
        }
        
        .sensor-status.inactive {
            background: #e0e0e0;
            color: #666;
        }
        
        .sensor-badge {
            display: inline-block;
            padding: 3px 10px;
            border-radius: 12px;
            font-size: 0.75em;
            background: #e3f2fd;
            color: #1976d2;
            margin-top: 5px;
        }
        
        .control-group {
            margin: 15px 0;
        }
        
        .control-label {
            display: block;
            color: #666;
            margin-bottom: 8px;
            font-size: 0.9em;
        }
        
        .slider-container {
            display: flex;
            align-items: center;
            gap: 10px;
        }
        
        .slider {
            flex: 1;
            height: 8px;
            border-radius: 5px;
            background: #ddd;
            outline: none;
            -webkit-appearance: none;
        }
        
        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
        }
        
        .slider::-moz-range-thumb {
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
            border: none;
        }
        
        .slider-value {
            min-width: 50px;
            text-align: right;
            color: #333;
            font-weight: 500;
        }
        
        .btn-group {
            display: flex;
            gap: 10px;
            margin-top: 15px;
        }
        
        .btn {
            flex: 1;
            padding: 12px;
            border: none;
            border-radius: 8px;
            font-size: 1em;
            cursor: pointer;
            transition: all 0.3s ease;
            font-weight: 500;
        }
        
        .btn-primary {
            background: #667eea;
            color: white;
        }
        
        .btn-primary:hover {
            background: #5568d3;
        }
        
        .btn-success {
            background: #4CAF50;
            color: white;
        }
        
        .btn-success:hover {
            background: #45a049;
        }
        
        .btn-danger {
            background: #f44336;
            color: white;
        }
        
        .btn-danger:hover {
            background: #da190b;
        }
        
        .btn:active {
            transform: scale(0.98);
        }
        
        .system-info {
            background: white;
            border-radius: 15px;
            padding: 25px;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.2);
        }
        
        .system-info h3 {
            color: #667eea;
            margin-bottom: 15px;
        }
        
        .info-row {
            display: flex;
            justify-content: space-between;
            padding: 10px 0;
            border-bottom: 1px solid #eee;
        }
        
        .info-row:last-child {
            border-bottom: none;
        }
        
        .info-label {
            color: #666;
        }
        
        .info-value {
            color: #333;
            font-weight: 500;
        }
        
        .controls {
            margin-top: 20px;
            text-align: center;
        }
        
        .tft-card {
            background: white;
            border-radius: 15px;
            padding: 25px;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.2);
            margin-bottom: 20px;
        }
        .tft-card h3 {
            color: #667eea;
            margin-bottom: 20px;
            font-size: 1.2em;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        .toggle-row {
            display: flex;
            align-items: center;
            justify-content: space-between;
            margin: 12px 0;
        }
        .toggle-label { color: #555; font-size: 0.95em; }
        .toggle-switch {
            position: relative;
            width: 52px;
            height: 28px;
        }
        .toggle-switch input { opacity: 0; width: 0; height: 0; }
        .toggle-track {
            position: absolute;
            inset: 0;
            background: #ccc;
            border-radius: 28px;
            cursor: pointer;
            transition: background 0.3s;
        }
        .toggle-track:before {
            content: '';
            position: absolute;
            width: 22px; height: 22px;
            left: 3px; top: 3px;
            background: white;
            border-radius: 50%;
            transition: transform 0.3s;
        }
        .toggle-switch input:checked + .toggle-track { background: #667eea; }
        .toggle-switch input:checked + .toggle-track:before { transform: translateX(24px); }

        /* ── RF 遥控卡片 ─────────────────────────────── */
        .rf-card {
            background: white;
            border-radius: 15px;
            padding: 25px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.2);
            margin-bottom: 20px;
        }
        .rf-card h3 {
            color: #667eea;
            margin-bottom: 16px;
            font-size: 1.2em;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        .rf-learn-bar {
            display: flex;
            gap: 8px;
            margin-bottom: 16px;
            flex-wrap: wrap;
            align-items: center;
        }
        .rf-learn-bar input[type=text] {
            flex: 1;
            min-width: 120px;
            padding: 8px 12px;
            border: 1.5px solid #ddd;
            border-radius: 8px;
            font-size: 0.95em;
            outline: none;
            transition: border-color 0.2s;
        }
        .rf-learn-bar input[type=text]:focus { border-color: #667eea; }
        .rf-status {
            margin: 10px 0 14px;
            padding: 8px 14px;
            border-radius: 8px;
            font-size: 0.9em;
            background: #f0f0f0;
            color: #555;
            display: flex;
            align-items: center;
            gap: 8px;
            min-height: 36px;
        }
        .rf-status.waiting { background: #fff8e1; color: #e65100; }
        .rf-status.success { background: #e8f5e9; color: #2e7d32; }
        .rf-status.timeout { background: #ffebee; color: #c62828; }
        .rf-device-list { list-style: none; padding: 0; margin: 0; }
        .rf-device-item {
            display: flex;
            align-items: center;
            justify-content: space-between;
            padding: 10px 12px;
            border-radius: 10px;
            background: #f7f8fc;
            margin-bottom: 8px;
            gap: 8px;
        }
        .rf-device-name {
            font-weight: 600;
            color: #333;
            flex: 1;
            font-size: 0.95em;
            overflow: hidden;
            text-overflow: ellipsis;
            white-space: nowrap;
        }
        .rf-device-meta {
            font-size: 0.78em;
            color: #999;
            white-space: nowrap;
        }
        .rf-device-btns { display: flex; gap: 6px; flex-shrink: 0; }
        .btn-rf-send {
            padding: 5px 14px;
            background: #667eea;
            color: white;
            border: none;
            border-radius: 7px;
            cursor: pointer;
            font-size: 0.85em;
            transition: background 0.2s;
        }
        .btn-rf-send:hover { background: #5569d4; }
        .btn-rf-del {
            padding: 5px 10px;
            background: #fff0f0;
            color: #e53935;
            border: 1px solid #ffcdd2;
            border-radius: 7px;
            cursor: pointer;
            font-size: 0.85em;
            transition: background 0.2s;
        }
        .btn-rf-del:hover { background: #ffebee; }

        @media (max-width: 768px) {
            .header h1 { font-size: 1.8em; }
            .dashboard { grid-template-columns: 1fr; }
            .sensor-dual { grid-template-columns: 1fr; }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🍼 BottleBoy</h1>
            <div id="status" class="status disconnected">连接中...</div>
        </div>
        
        <h2 class="section-title">📊 传感器</h2>
        <div id="sensors" class="dashboard">
            <div class="card">
                <h3>等待数据...</h3>
                <div class="sensor-value">--</div>
            </div>
        </div>
        
        <h2 class="section-title">🎛️ 执行器</h2>
        <div id="actuators" class="dashboard">
            <div class="card">
                <h3>等待数据...</h3>
                <div class="sensor-value">--</div>
            </div>
        </div>
        
        <h2 class="section-title">🖥️ 屏幕</h2>
        <div class="tft-card">
            <h3>📺 TFT 显示屏</h3>

            <div class="toggle-row">
                <span class="toggle-label">屏幕开关</span>
                <label class="toggle-switch">                    <input type="checkbox" id="tft-power" onchange="sendTft({state: this.checked ? 'on' : 'off'})">
                    <span class="toggle-track"></span>
                </label>
            </div>

            <div class="control-group">
                <label class="control-label">
                    背光亮度: <span id="tft-brightness-val">255</span>
                </label>
                <div class="slider-container">
                    <input type="range" min="0" max="255" value="255"
                           class="slider" id="tft-brightness"
                           oninput="document.getElementById('tft-brightness-val').textContent=this.value"
                           onchange="sendTft({brightness: parseInt(this.value)})">
                    <span class="slider-value" id="tft-brightness-display">255</span>
                </div>
            </div>

            <div class="toggle-row">
                <span class="toggle-label">夜间模式 <small style="color:#999">(黑底琥珀橙)</small></span>
                <label class="toggle-switch">
                    <input type="checkbox" id="tft-night" onchange="sendTft({nightMode: this.checked})">
                    <span class="toggle-track"></span>
                </label>
            </div>

            <div style="margin-top:12px">
                <button class="btn btn-primary" style="font-size:0.85em;padding:8px 16px"
                        onclick="sendTft({autoMode:true}); document.getElementById('tft-night').disabled=false">
                    🔄 跟随光线自动
                </button>
            </div>
        </div>

        <h2 class="section-title">📡 433MHz 遥控</h2>
        <div class="rf-card">
            <h3>📡 RF 遥控学习器</h3>

            <!-- 学习栏 -->
            <div class="rf-learn-bar">
                <input type="text" id="rf-name-input" placeholder="输入设备名称，如：灯1" maxlength="30">
                <button class="btn btn-success" style="white-space:nowrap" onclick="rfStartLearn()">🎯 开始学习</button>
                <button class="btn btn-danger"  style="white-space:nowrap" onclick="rfCancelLearn()">✖ 取消</button>
            </div>

            <!-- 学习状态提示 -->
            <div class="rf-status" id="rf-status">
                <span id="rf-status-icon">ℹ️</span>
                <span id="rf-status-text">空闲 — 输入名称后点击"开始学习"，然后按遥控器按键</span>
            </div>

            <!-- 已存设备列表 -->
            <div style="font-weight:600;color:#555;margin-bottom:8px;font-size:0.9em">已学设备</div>
            <ul class="rf-device-list" id="rf-device-list">
                <li style="color:#bbb;font-size:0.9em;padding:8px 0">暂无设备</li>
            </ul>
        </div>

        <div class="system-info">
            <h3>系统信息</h3>
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
                <span class="info-label">CPU频率</span>
                <span class="info-value" id="cpuFreq">--</span>
            </div>
        </div>
        
        <div class="controls">
            <button class="btn btn-primary" onclick="refreshData()">刷新数据</button>
            <button class="btn btn-primary" onclick="location.reload()">重新加载</button>
        </div>
    </div>
    
    <script>
        let ws;
        let reconnectInterval = 5000;
        
        function connectWebSocket() {
            const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
            const wsUrl = `${protocol}//${window.location.host}/ws`;
            
            ws = new WebSocket(wsUrl);
            
            ws.onopen = function() {
                document.getElementById('status').textContent = '已连接';
                document.getElementById('status').className = 'status connected';
            };
            
            ws.onmessage = function(event) {
                const data = JSON.parse(event.data);
        // 接收到数据时同步 TFT 状态
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
                if (data.sensors) updateSensors(data.sensors);
                if (data.actuators) {
                    data.actuators.forEach(a => { actuatorStates[a.name] = a.state; });
                    const actuatorsDiv = document.getElementById('actuators');
                    if (actuatorsDiv.children.length === 0 || actuatorsDiv.querySelector('.card h3')?.textContent === '等待数据...') {
                        updateActuators(data.actuators);
                    } else {
                        // 已有卡片，只更新状态
                        data.actuators.forEach(actuator => {
                            if (!isDragging[actuator.name]) {
                                updateActuatorState(actuator);
                            } else {
                                // 拖动时至少更新按钮
                                updateButtonState(actuator);
                            }
                        });
                    }
                }
                if (data.uptime !== undefined) {
                    document.getElementById('uptime').textContent = formatUptime(data.uptime);
                    document.getElementById('freeHeap').textContent = formatBytes(data.freeHeap);
                    document.getElementById('chipModel').textContent = data.chipModel;
                    document.getElementById('cpuFreq').textContent = data.cpuFreq + ' MHz';
                }
                // RF 状态同步
                if (data.rf) updateRF(data.rf);
            };
            
            ws.onerror = function(error) {
                document.getElementById('status').textContent = '连接错误';
                document.getElementById('status').className = 'status disconnected';
            };
            
            ws.onclose = function() {
                document.getElementById('status').textContent = '已断开';
                document.getElementById('status').className = 'status disconnected';
                setTimeout(connectWebSocket, reconnectInterval);
            };
        }
        
        function updateSensors(sensors) {
            const sensorsDiv = document.getElementById('sensors');
            
            if (sensors && sensors.length > 0) {
                sensorsDiv.innerHTML = '';
                
                sensors.forEach(sensor => {
                    const card = document.createElement('div');
                    card.className = 'card';
                    
                    // DHT22温湿度传感器
                    if (sensor.type === 'dht22') {
                        card.innerHTML = `
                            <h3>🌡️ ${sensor.name}</h3>
                            <div class="sensor-dual">
                                <div class="sensor-item">
                                    <div class="sensor-item-label">温度</div>
                                    <div class="sensor-item-value">${sensor.temperature?.toFixed(1) || '--'}</div>
                                    <div class="sensor-unit">${sensor.tempUnit || '°C'}</div>
                                </div>
                                <div class="sensor-item">
                                    <div class="sensor-item-label">湿度</div>
                                    <div class="sensor-item-value">${sensor.humidity?.toFixed(1) || '--'}</div>
                                    <div class="sensor-unit">${sensor.humUnit || '%'}</div>
                                </div>
                            </div>
                        `;
                    }
                    else if (sensor.type === 'pir') {
                        const isDetected = sensor.detected || false;
                        card.innerHTML = `
                            <h3>👤 ${sensor.name}</h3>
                            <div class="sensor-value">${sensor.state || '无人'}</div>
                            <div class="sensor-status ${isDetected ? 'active' : 'inactive'}">
                                ${isDetected ? '✓ 检测到人' : '○ 无人'}
                            </div>
                        `;
                    }
                    else if (sensor.type === 'light') {
                        const displayValue = sensor.displayValue || '--';
                        card.innerHTML = `
                            <h3>💡 ${sensor.name}</h3>
                            <div class="sensor-value">${displayValue}</div>
                            <div class="sensor-unit">环境光线</div>
                        `;
                    }
                    // 其他传感器
                    else {
                        card.innerHTML = `
                            <h3>${sensor.name}</h3>
                            <div class="sensor-value">${sensor.value !== undefined ? sensor.value.toFixed(2) : '--'}</div>
                            <div class="sensor-unit">${sensor.unit || ''}</div>
                            <div class="sensor-badge">类型: ${sensor.type}</div>
                        `;
                    }
                    
                    sensorsDiv.appendChild(card);
                });
            }
        }
        
        let actuatorStates = {};
        
        function updateActuators(actuators) {
            const actuatorsDiv = document.getElementById('actuators');
            
            if (actuators && actuators.length > 0) {
                actuators.forEach(actuator => {
                    updateOrCreateActuatorCard(actuator, actuatorsDiv);
                });
            }
        }
        
        function updateActuatorState(actuator) {
            // 更新按钮、滑块和显示值
            const cards = document.querySelectorAll('.card');
            cards.forEach(card => {
                const h3 = card.querySelector('h3');
                if (h3 && h3.textContent.includes(actuator.name)) {
                    // 更新滑块值
                    const brightnessSlider = card.querySelector(`#${actuator.name}-brightness`);
                    const colorTempSlider = card.querySelector(`#${actuator.name}-colortemp`);
                    if (brightnessSlider) {
                        brightnessSlider.value = actuator.brightness || 0;
                        document.getElementById(actuator.name + '-brightness-val').textContent = actuator.brightness || 0;
                        document.getElementById(actuator.name + '-brightness-display').textContent = actuator.brightness || 0;
                    }
                    if (colorTempSlider) {
                        colorTempSlider.value = actuator.colorTemp || 50;
                        const temps = ['暖光', '暖白', '中性白', '冷白', '纯白光'];
                        const index = Math.floor((actuator.colorTemp || 50) / 20);
                        document.getElementById(actuator.name + '-colortemp-val').textContent = temps[Math.min(index, 4)];
                    }
                    
                    // 更新按钮
                    updateButtonState(actuator);
                    
                    // 更新状态显示
                    if (actuator.type === 'dual_color_led') {
                        const statusDiv = card.querySelector('[style*="margin-top: 10px"]');
                        if (statusDiv) {
                            const isOn = actuator.state === 'on';
                            statusDiv.innerHTML = `
                                状态: ${isOn ? '开启' : '关闭'} | 
                                白光: ${actuator.whiteValue || 0} | 
                                暖光: ${actuator.warmValue || 0}
                            `;
                        }
                    }
                }
            });
        }
        
        function updateButtonState(actuator) {
            // 只更新按钮状态
            const cards = document.querySelectorAll('.card');
            cards.forEach(card => {
                const h3 = card.querySelector('h3');
                if (h3 && h3.textContent.includes(actuator.name)) {
                    const btn = card.querySelector('.btn');
                    if (btn && actuator.state) {
                        const isOn = actuator.state === 'on';
                        btn.className = `btn ${isOn ? 'btn-danger' : 'btn-success'}`;
                        btn.textContent = isOn ? '关灯' : '开灯';
                    }
                }
            });
        }
        
        function updateOrCreateActuatorCard(actuator, container) {
            // 查找现有卡片
            let existingCard = null;
            const cards = container.querySelectorAll('.card');
            cards.forEach(card => {
                const h3 = card.querySelector('h3');
                if (h3 && h3.textContent.includes(actuator.name)) {
                    existingCard = card;
                }
            });
            
            if (!existingCard) {
                // 创建新卡片
                createActuatorCard(actuator, container);
            }
        }
        
        function createActuatorCard(actuator, container) {
            container.innerHTML = '';
            const card = document.createElement('div');
            card.className = 'card';
            
            // 双色调温LED
            if (actuator.type === 'dual_color_led') {
                const isOn = actuator.state === 'on';
                card.innerHTML = `
                            <h3>💡 ${actuator.name}</h3>
                            
                            <div class="control-group">
                                <label class="control-label">
                                    亮度: <span id="${actuator.name}-brightness-val">${actuator.brightness || 0}</span>
                                </label>
                                <div class="slider-container">
                                    <input type="range" min="0" max="255" value="${actuator.brightness || 0}" 
                                           class="slider" id="${actuator.name}-brightness"
                                           oninput="previewBrightness('${actuator.name}', this.value)"
                                           onchange="sendBrightness('${actuator.name}', this.value)">
                                    <span class="slider-value" id="${actuator.name}-brightness-display">${actuator.brightness || 0}</span>
                                </div>
                            </div>
                            
                            <div class="control-group">
                                <label class="control-label">
                                    色温: <span id="${actuator.name}-colortemp-val">${actuator.colorTempDesc || ''}</span>
                                </label>
                                <div class="slider-container">
                                    <span style="font-size: 1.2em;">🔥</span>
                                    <input type="range" min="0" max="100" value="${actuator.colorTemp || 50}" 
                                           class="slider" id="${actuator.name}-colortemp"
                                           oninput="previewColorTemp('${actuator.name}', this.value)"
                                           onchange="sendColorTemp('${actuator.name}', this.value)">
                                    <span style="font-size: 1.2em;">❄️</span>
                                </div>
                            </div>
                            
                            <div class="btn-group">
                                <button class="btn ${isOn ? 'btn-danger' : 'btn-success'}" 
                                        onclick="toggleLED('${actuator.name}')">
                                    ${isOn ? '关灯' : '开灯'}
                                </button>
                            </div>
                            
                            <div style="margin-top: 10px; color: #999; font-size: 0.85em;">
                                状态: ${isOn ? '开启' : '关闭'} | 
                                白光: ${actuator.whiteValue || 0} | 
                                暖光: ${actuator.warmValue || 0}
                            </div>
                        `;
            }
            // 普通LED
            else if (actuator.type === 'led') {
                const isOn = actuator.state === 'on';
                card.innerHTML = `
                            <h3>💡 ${actuator.name}</h3>
                            <div class="sensor-value">${actuator.brightness || 0}</div>
                            <div class="sensor-unit">亮度</div>
                            
                            <div class="control-group">
                                <input type="range" min="0" max="255" value="${actuator.brightness || 0}" 
                                       class="slider" onchange="setActuator('${actuator.name}', this.value)">
                            </div>
                            
                            <div class="btn-group">
                                <button class="btn ${isOn ? 'btn-danger' : 'btn-success'}" 
                                        onclick="toggleActuator('${actuator.name}')">
                                    ${isOn ? '关闭' : '开启'}
                                </button>
                            </div>
                        `;
            }
            // 其他执行器
            else {
                card.innerHTML = `
                            <h3>${actuator.name}</h3>
                            <div class="sensor-value">${actuator.value !== undefined ? actuator.value : '--'}</div>
                            <div class="sensor-badge">类型: ${actuator.type}</div>
                            <div class="sensor-badge">状态: ${actuator.state || 'unknown'}</div>
                        `;
            }
            
            container.appendChild(card);
        }
        
        let isDragging = {};
        
        // oninput: 只更新界面显示，零延迟
        function previewBrightness(name, value) {
            document.getElementById(name + '-brightness-val').textContent = value;
            document.getElementById(name + '-brightness-display').textContent = value;
            isDragging[name] = true;
        }
        
        function previewColorTemp(name, value) {
            const temps = ['暖光', '暖白', '中性白', '冷白', '纯白光'];
            const index = Math.min(Math.floor(value / 20), 4);
            document.getElementById(name + '-colortemp-val').textContent = temps[index];
            isDragging[name] = true;
        }
        
        // onchange: 松手才发送，一条命令搞定
        function sendBrightness(name, value) {
            isDragging[name] = false;
            sendControl({ cmd: 'control', name: name, brightness: parseInt(value) });
        }
        
        function sendColorTemp(name, value) {
            isDragging[name] = false;
            sendControl({ cmd: 'control', name: name, colorTemp: parseInt(value) });
        }
        
        function sendControl(payload) {
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify(payload));
            }
        }
        
        function toggleLED(name) {
            const currentState = actuatorStates[name] || 'off';
            const newState = currentState === 'on' ? 'off' : 'on';
            sendControl({ cmd: 'control', name: name, state: newState });
        }
        
        function setActuator(name, value) {
            sendControl({ cmd: 'control', name: name, brightness: parseFloat(value) });
        }
        
        function toggleActuator(name) {
            const currentState = actuatorStates[name] || 'off';
            const newState = currentState === 'on' ? 'off' : 'on';
            sendControl({ cmd: 'control', name: name, state: newState });
        }
        
        function sendTft(payload) {
            sendControl(Object.assign({ cmd: 'tft' }, payload));
        }

        // ──────────────────────────────────────────────
        // RF 遥控相关函数
        // ──────────────────────────────────────────────

        /**
         * 开始学习：从输入框取名称，发送 rfLearn 命令
         */
        function rfStartLearn() {
            const nameEl = document.getElementById('rf-name-input');
            const name = nameEl ? nameEl.value.trim() : '';
            if (!name) {
                rfSetStatus('error', '⚠️', '请先输入设备名称');
                return;
            }
            sendControl({ cmd: 'rfLearn', name });
            rfSetStatus('waiting', '📡', `正在等待 "${name}" 的信号，请按遥控器按键…`);
        }

        /**
         * 取消学习
         */
        function rfCancelLearn() {
            sendControl({ cmd: 'rfCancel' });
            rfSetStatus('idle', 'ℹ️', '学习已取消');
        }

        /**
         * 发射 RF 信号
         * @param {string} name 设备名称
         */
        function rfSend(name) {
            sendControl({ cmd: 'rfSend', name });
        }

        /**
         * 删除设备
         * @param {string} name 设备名称
         */
        function rfDelete(name) {
            if (!confirm(`确认删除设备 "${name}"？`)) return;
            sendControl({ cmd: 'rfDelete', name });
        }

        /**
         * 设置学习状态栏样式和文字
         * @param {'idle'|'waiting'|'success'|'timeout'|'error'} state
         * @param {string} icon
         * @param {string} text
         */
        function rfSetStatus(state, icon, text) {
            const bar  = document.getElementById('rf-status');
            const ico  = document.getElementById('rf-status-icon');
            const txt  = document.getElementById('rf-status-text');
            if (!bar) return;
            bar.className = 'rf-status ' + (state === 'error' ? 'timeout' : state);
            if (ico) ico.textContent = icon;
            if (txt) txt.textContent = text;
        }

        /**
         * 根据后端推送的 rf 对象更新 UI
         * @param {{ state:string, name:string, devices:Array }} rf
         */
        function updateRF(rf) {
            // 更新状态栏
            if (rf.state === 'waiting') {
                rfSetStatus('waiting', '📡', `正在等待 "${rf.name}" 的信号，请按遥控器按键…`);
            } else if (rf.state === 'success') {
                rfSetStatus('success', '✅', `学习成功：${rf.name}`);
                const nameEl = document.getElementById('rf-name-input');
                if (nameEl) nameEl.value = '';
            } else if (rf.state === 'timeout') {
                rfSetStatus('timeout', '⏱️', `学习超时：等待 "${rf.name}" 信号超时，请重试`);
            } else {
                rfSetStatus('idle', 'ℹ️', '空闲 — 输入名称后点击"开始学习"，然后按遥控器按键');
            }

            // 更新设备列表
            if (!rf.devices) return;
            const list = document.getElementById('rf-device-list');
            if (!list) return;
            if (rf.devices.length === 0) {
                list.innerHTML = '<li style="color:#bbb;font-size:0.9em;padding:8px 0">暂无设备</li>';
                return;
            }
            list.innerHTML = rf.devices.map(dev => `
                <li class="rf-device-item">
                    <span class="rf-device-name">${dev.name}</span>
                    <span class="rf-device-meta">协议${dev.protocol} · ${dev.bitLength}bit</span>
                    <div class="rf-device-btns">
                        <button class="btn-rf-send" onclick="rfSend('${dev.name.replace(/'/g, "\\'")}')">▶ 发射</button>
                        <button class="btn-rf-del"  onclick="rfDelete('${dev.name.replace(/'/g, "\\'")}')">🗑</button>
                    </div>
                </li>
            `).join('');
        }
        
        let tftBrightnessDragging = false;
        document.addEventListener('DOMContentLoaded', () => {
            const bSlider = document.getElementById('tft-brightness');
            if (bSlider) {
                bSlider.addEventListener('mousedown', () => { tftBrightnessDragging = true; });
                bSlider.addEventListener('touchstart', () => { tftBrightnessDragging = true; });
                bSlider.addEventListener('mouseup', () => { tftBrightnessDragging = false; });
                bSlider.addEventListener('touchend', () => { tftBrightnessDragging = false; });
            }
        });
        
        function refreshData() {
            sendControl({ cmd: 'getData' });
        }
        
        function formatUptime(seconds) {
            const days = Math.floor(seconds / 86400);
            const hours = Math.floor((seconds % 86400) / 3600);
            const minutes = Math.floor((seconds % 3600) / 60);
            const secs = seconds % 60;
            
            if (days > 0) return `${days}天 ${hours}小时`;
            if (hours > 0) return `${hours}小时 ${minutes}分钟`;
            if (minutes > 0) return `${minutes}分钟 ${secs}秒`;
            return `${secs}秒`;
        }
        
        function formatBytes(bytes) {
            if (bytes < 1024) return bytes + ' B';
            if (bytes < 1048576) return (bytes / 1024).toFixed(2) + ' KB';
            return (bytes / 1048576).toFixed(2) + ' MB';
        }
        
        connectWebSocket();
        
        // 降低自动刷新频率到10秒，避免干扰滑动
        setInterval(() => {
            if (ws && ws.readyState === WebSocket.OPEN && !Object.values(isDragging).some(Boolean)) {
                sendControl({ cmd: 'getData' });
            }
        }, 10000);
        
        // 初始数据加载
        setTimeout(refreshData, 500);
    </script>
</body>
</html>
)rawliteral";

#endif // WEB_PAGES_H
