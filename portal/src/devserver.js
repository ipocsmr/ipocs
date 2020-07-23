const WebSocket = require('ws');
const { IncomingMessage } = require('http');
const { setUncaughtExceptionCaptureCallback } = require('process');

let files = [{
  name: "/hex/firmware.hex",
  size: 100
}];
let unitId = 10;
let ssid = "NTJ";
let pwd = "testpwd";
let siteData = "00 01 02 03";

const wss = new WebSocket.Server({
  port: 81,
  perMessageDeflate: {
    zlibDeflateOptions: {
      // See zlib defaults.
      chunkSize: 1024,
      memLevel: 7,
      level: 3
    },
    zlibInflateOptions: {
      chunkSize: 10 * 1024
    },
    // Other options settable:
    clientNoContextTakeover: true, // Defaults to negotiated value.
    serverNoContextTakeover: true, // Defaults to negotiated value.
    serverMaxWindowBits: 10, // Defaults to negotiated value.
    // Below options specified as default values.
    concurrencyLimit: 10, // Limits zlib concurrency for perf.
    threshold: 1024 // Size (in bytes) below which messages
    // should not be compressed.
  }
});

function sendToAll(message, ws) {
  wss.clients.forEach(function each(client) {
    if (ws == undefined || (client !== ws && client.readyState === WebSocket.OPEN)) {
      sendToOne(client, message);
    }
  });
}

function sendToOne(client, message) {
  client.send(JSON.stringify(message));
}

wss.on('connection', function connection(ws) {
  ws.on('message', function incoming(message) {
    console.log('received: %s', message);
    let data = JSON.parse(message);
    switch (data.action) {
      case 'setUnitId':
        unitId = data.value;
        sendToAll({ action: 'valueUnitId', value: unitId });
        break;
      case 'getUnitId':
        sendToOne(ws, { action: 'valueUnitId', value: unitId });
        break;
      case 'setSsid':
        ssid = data.value;
        sendToAll({ action: 'valueSsid', value: ssid });
        break;
      case 'getSsid':
        sendToOne(ws, { action: 'valueSsid', value: ssid });
        break;
      case 'setPwd':
        pwd = data.value;
        sendToAll({ action: 'valuePwd', value: pwd });
        break;
      case 'getPwd':
        sendToOne(ws, { action: 'valuePwd', value: pwd });
        break;
      case 'setSiteData':
        siteData = data.value;
        sendToAll({ action: 'valueSsid', value: siteData });
        break;
      case 'getSiteData':
        sendToOne(ws, { action: 'valueSiteData', value: siteData });
        break;
      case 'deleteFile':
        let toDelete = data.value;
        files = files.filter((value) => value.name == data.name);
        sendFiles(ws);
        break;
      default:
        console.log('UNKNOWN INPUT');
        return;
    }
  });
  sendToOne(ws, { action: 'valueUnitId', value: unitId });
  sendToOne(ws, { action: 'valueSsid', value: ssid });
  sendToOne(ws, { action: 'valuePwd', value: pwd });
  sendToOne(ws, { action: 'valueSiteData', value: siteData });
  sendFiles(ws);
});

function sendFiles(ws) {
  sendToOne(ws, { action: 'valueFiles', value: JSON.stringify(files) });
}