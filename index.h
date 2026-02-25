const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Cube Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://code.highcharts.com/highcharts.js"></script>
  <style>
    body {
      font-family: sans-serif;
      text-align: center;
      background: #121212;
      color: white;
      margin: 0;
    }

    .container {
      max-width: 800px;
      margin: auto;
      padding: 20px;
    }

    .card {
      background: #1e1e1e;
      padding: 20px;
      border-radius: 12px;
      margin-bottom: 15px;
      border: 1px solid #333;
    }

    .value {
      font-size: 2.5rem;
      color: #00ffcc;
      font-weight: bold;
    }

    .btn {
      padding: 15px 30px;
      font-size: 1rem;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      font-weight: bold;
      margin: 5px;
      transition: 0.2s;
    }

    .btn-on {
      background: #28a745;
      color: white;
    }

    .btn-off {
      background: #dc3545;
      color: white;
    }

    .btn:active {
      transform: scale(0.95);
    }

    #chart-test {
      height: 400px;
      width: 100%;
    }

  </style>
</head>
<body>
  <div class="container">
    <h1>Self-Balancing Cube</h1>
    <div class="card">
      <p>Angle X: <span id="x_val" class="value">0.0</span>° | Angle Y: <span id="y_val" class="value">0.0</span>°</p>
    </div>
    <div class="card">
      <h3>Remote Control</h3>
      <button class="btn btn-on" onclick="sendLED('ON')">LED ON</button>
      <button class="btn btn-off" onclick="sendLED('OFF')">LED OFF</button>
    </div>
    <div id="chart-test" class="card"></div>
  </div>
  <script>
    var chart = new Highcharts.Chart({
      chart:{ renderTo:'chart-test', type:'spline', backgroundColor: 'transparent' },
      title:{ text: 'Angle Data Stream', style: {color: '#888'} },
      series: [{ name: 'X', data: [], color: '#ff3366' }, { name: 'Y', data: [], color: '#00ffcc' }],
      xAxis: { type: 'datetime' },
      yAxis: { title: { text: 'Value' }, gridLineColor: '#222' }
    });
    var gateway = `ws://${window.location.hostname}:81/`;
    var websocket = new WebSocket(gateway);
    websocket.onmessage = function(event) {
      try {
        var data = JSON.parse(event.data);
        if(data.x !== undefined) {
          document.getElementById('x_val').innerHTML = data.x;
          document.getElementById('y_val').innerHTML = data.y;
          var x = (new Date()).getTime();
          var shift = chart.series[0].data.length > 50;
          chart.series[0].addPoint([x, parseFloat(data.x)], true, shift);
          chart.series[1].addPoint([x, parseFloat(data.y)], true, shift);
        }
      } catch(e) { console.log("Received status:", event.data); }
    };
    function sendLED(state) {
      if (websocket.readyState === WebSocket.OPEN) websocket.send(state);
    }
  </script>
</body>
</html>
)rawliteral";
