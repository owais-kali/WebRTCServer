/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */

'use strict';

const SignalingType = {
  TYPE_SDP: 1,
  TYPE_ICE: 2,
  START: 3,
  END: 4,
}

const CallBtn = document.getElementById('CallBtn');

const localVideo = document.getElementById('localVideo');
const remoteVideo = document.getElementById('remoteVideo');

let pc;
let localStream;

const WebRTCConfig = { iceServers: [] };
const Conn = new RTCPeerConnection(WebRTCConfig);

Conn.ondatachannel = event => {
  const receiveChannel = event.channel;
  receiveChannel.onopen = () => {
    console.log('DataChannel is open and ready to communicate');
  };
  receiveChannel.onmessage = (msg) =>{
    console.log("Datachannel msg: "+msg.data);
  }
};

const ws = new WebSocket('wss://192.168.1.22');

// Handle connection open
ws.onopen = function () {
  console.log('Connected to WebSocket server');
};

Conn.onicecandidate = (obj, ev) => {
  if (obj.candidate != null){
    const str = JSON.stringify({
      "Candidate": obj.candidate.candidate,
      "SDPMid": obj.candidate.sdpMid,
      "sdpMLineIndex": obj.candidate.sdpMLineIndex
    });
    ws.send(str)
  }
}

ws.onmessage = async function (msg) {
  const data = JSON.parse(msg.data).data;
  switch (data.type) {
    case SignalingType.TYPE_SDP:
      Conn.setRemoteDescription({
        sdp: data.sdp.SDPData,
        type: data.sdp.Type,
      });

      const ans = await Conn.createAnswer();
      await Conn.setLocalDescription(ans)
      const init = Conn.localDescription;

      const res = {
        type: SignalingType.TYPE_SDP,
        sdp: {
          Type: init.type,
          SDPData: init.sdp,
        },
      };
      const str = JSON.stringify(res);
      ws.send(str);
      break;
    case SignalingType.TYPE_ICE:
      const ice = {
        candidate: data.ice.Candidate,
        sdpMid: data.ice.SDPMid != undefined ? data.ice.SDPMid : "",
        sdpMLineIndex:
          data.ice.SDPMLineIndex != undefined ? data.ice.SDPMLineIndex : 0,
      }
      const candidate = new RTCIceCandidate(ice);
      console.log("Got ICE: " + ice.candidate);

      await Conn.addIceCandidate(candidate);
      break;
    default:
      break;
  }
}