const net = require('net');

// Create a server
const server = net.createServer((socket) => {
    console.log('Client connected');

    // Event when receiving data from the client
    socket.on('data', (data) => {
        console.log(`Received: ${data}`);
        // Send a response back to the client
        socket.write('Hello from server');
        socket.write('I am signalling');
    });

    // Event when client disconnects
    socket.on('end', () => {
        console.log('Client disconnected');
    });

    // Event if there is an error
    socket.on('error', (err) => {
        console.error('Socket error:', err);
    });
});

// Define port and host
const PORT = 8081;
const HOST = '127.0.0.1';

// Start the server and listen on the specified port
server.listen(PORT, HOST, () => {
    console.log(`Server listening on ${HOST}:${PORT}`);
});
