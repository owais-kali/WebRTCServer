const express = require('express')
const https = require('https');
const path = require('path');
const WebSocket = require('ws');
const fs = require('fs');

const app = express()
const port = 3000

const key = fs.readFileSync(path.join(__dirname, 'server.key'));
const cert = fs.readFileSync(path.join(__dirname, 'server.cert'));

const rootPath = "/views"
const webrtc_write = "/tmp/webrtc_write";
const webrtc_read = "webrtc_read";

app.use(express.static(path.join(__dirname, 'views')));

app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, '/example1/index.html'));
})

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`)
})

function createFifoStream() {
  console.log('Opening FIFO stream...');

  const fifoStream = fs.createReadStream(webrtc_write, { encoding: 'utf8' });

  fifoStream.on('data', (data) => {
    console.log('Data from FIFO:', data);
  });

  fifoStream.on('end', () => {
    console.log('FIFO stream ended. Re-opening...');
    // Wait a bit before reopening to prevent infinite loops in some cases
    setTimeout(createFifoStream, 1000); // Recreate after a short delay
  });

  fifoStream.on('error', (err) => {
    console.error('FIFO stream error:', err);
    // Handle errors (e.g., FIFO pipe not available) and retry
    setTimeout(createFifoStream, 1000); // Retry opening the pipe
  });
}

createFifoStream();

const httpsServer = https.createServer({ key, cert }, app);
const wss = new WebSocket.Server({ server: httpsServer });

wss.on('connection', (ws) => {
  console.log('New client connected');

  // Handle messages received from the client
  ws.on('message', (message) => {
    console.log('Received message:', message);
    // Echo the message back to the client
    ws.send(`Server: You said '${message}'`);
  });

  // Handle client disconnection
  ws.on('close', () => {
    console.log('Client disconnected');
  });
});

httpsServer.listen(443, () => {
  console.log('HTTPS server running on port 443');
});