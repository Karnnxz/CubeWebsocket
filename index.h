const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Motor Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://code.highcharts.com/highcharts.js"></script>
  <style>
    body { font-family: sans-serif; text-align: center; background: #121212; color: white; }
    .container { max-width: 800px; margin: auto; padding: 20px; }
    .card { background: #1e1e1e; padding: 20px; border-radius: 12px; margin-bottom: 10px; border: 1px solid #333; }
    .value { font-size: 2.5rem; color: #00ffcc; font-weight: bold; }
    #chart-test { height: 400px; width: 100%; }
  </style>
</head>
<body>
  <div class="container">
    <h1>Self-Balancing Cube Websocket</h1>
    <div class="card">
      <p>Simulated Angle X: <span id="x_val" class="value">0.0</span>°</p>
      <p>Simulated Angle Y: <span id="y_val" class="value">0.0</span>°</p>
    </div>
    <div id="chart-test" class="card"></div>
  </div>
  <script>
    var chart = new Highcharts.Chart({
      chart:{ renderTo:'chart-test', type:'spline', backgroundColor: 'transparent' },
      title:{ text: 'Mock Data Stream', style: {color: '#888'} },
      series: [{ name: 'Test X', data: [], color: '#ff3366' }, { name: 'Test Y', data: [], color: '#00ffcc' }],
      xAxis: { type: 'datetime' },
      yAxis: { title: { text: 'Value' }, gridLineColor: '#222' }
    });
    var gateway = `ws://${window.location.hostname}:81/`;
    var websocket = new WebSocket(gateway);
    websocket.onmessage = function(event) {
      var data = JSON.parse(event.data);
      document.getElementById('x_val').innerHTML = data.x;
      document.getElementById('y_val').innerHTML = data.y;
      var x = (new Date()).getTime();
      if(chart.series[0].data.length > 50) {
        chart.series[0].addPoint([x, parseFloat(data.x)], true, true);
        chart.series[1].addPoint([x, parseFloat(data.y)], true, true);
      } else {
        chart.series[0].addPoint([x, parseFloat(data.x)], true, false);
        chart.series[1].addPoint([x, parseFloat(data.y)], true, false);
      }
    };
  </script>
</body>
</html>
)rawliteral";
