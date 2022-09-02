# WebRTCServer

This is a simple WebRTC server using https://webrtc.googlesource.com/src/

# How to Build
Since webrtc library is very big I have only added files that I have created. To build you will need to clone webrtc source code from
[here](https://webrtc.googlesource.com/src/)

now clone this repo inside src/examples folder
and replace src/examples/BUILD.gn with BUILD.gn in this repo.

To Generate ninja files run in src/:

For Release Build:
```gn gen out/Default --args='is_debug=false'``` 

For Debug Build:
```gn gen out/Default --args='is_debug=true'``` 

To compile run:
```ninja -C out/Default```

# Run Client Server
requirement 
nodejs: v16.14.0
npm: 8.3.1

Go to WebRTCServer/Client  
```npm install```  
```node index.js```

Open Chrome Browser and go to   
```http://localhost:3000/```
