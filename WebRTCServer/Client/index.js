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

const httpsServer = https.createServer({ key, cert }, app);
const wss = new WebSocket.Server({ server: httpsServer });

let client;
let webrtc_socket;

app.use(express.static(path.join(__dirname, 'views')));

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '/example1/index.html'));
})

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`)
})

const net = require('net');

// Create a server
const server = net.createServer((socket) => {
    console.log('Webrtc Server connected');
    webrtc_socket = socket;
    // Event when receiving data from the client
    socket.on('data', (data) => {
        console.log(`Webrtc Server Sent: ${data}`);
        if (client) {
          (client).send(data.toString())
        }
    });

    // Event when client disconnects
    socket.on('end', () => {
        console.log('Webrtc Server disconnected');
    });

    // Event if there is an error
    socket.on('error', (err) => {
        console.error('Webrtc Server error:', err);
    });
});

// Start the server and listen on the specified port
server.listen(8081, '127.0.0.1', () => {
    console.log(`TCP listening on 8081`);
});

wss.on('connection', (ws) => {
  console.log('New client connected');
  client = ws;
  // Handle messages received from the client
  ws.on('message', (message) => {
    if (Buffer.isBuffer(message)) {
      message = message.toString();
    }
    webrtc_socket.write(message);
    console.log('Received message:', message);
  });

  // Handle client disconnection
  ws.on('close', () => {
    console.log('Client disconnected');
  });
});

httpsServer.listen(443, () => {
  console.log('HTTPS server running on port 443');
});