<body>
  <div class="container">
    <h1>Self-Balancing Cube Websocket</h1>
    
    <div class="card">
      <p>Simulated Angle X: <span id="x_val" class="value">0.0</span>°</p>
      <p>Simulated Angle Y: <span id="y_val" class="value">0.0</span>°</p>
    </div>

    <div class="card">
      <h3>Manual Control</h3>
      <button onclick="sendLED('ON')" style="padding:15px 30px; background:#28a745; color:white; border:none; border-radius:8px; cursor:pointer; font-weight:bold;">LED ON</button>
      <button onclick="sendLED('OFF')" style="padding:15px 30px; background:#dc3545; color:white; border:none; border-radius:8px; cursor:pointer; font-weight:bold; margin-left:10px;">LED OFF</button>
    </div>

    <div id="chart-test" class="card"></div>
  </div>

  <script>
    // ... ส่วนของ Highcharts เหมือนเดิม ...

    var gateway = `ws://${window.location.hostname}:81/`;
    var websocket = new WebSocket(gateway);

    websocket.onmessage = function(event) {
      // ป้องกัน Error กรณี ESP32 ส่งข้อความทักทายตอนแรกที่ไม่ใช่ JSON
      try {
          var data = JSON.parse(event.data);
          document.getElementById('x_val').innerHTML = data.x;
          document.getElementById('y_val').innerHTML = data.y;
          
          var x = (new Date()).getTime();
          var shift = chart.series[0].data.length > 50;
          chart.series[0].addPoint([x, parseFloat(data.x)], true, shift);
          chart.series[1].addPoint([x, parseFloat(data.y)], true, shift);
      } catch(e) {
          console.log("Non-JSON message received:", event.data);
      }
    };

    function sendLED(state) {
      if (websocket.readyState === WebSocket.OPEN) {
          websocket.send(state);
      } else {
          alert("WebSocket is not connected!");
      }
    }
  </script>
</body>
