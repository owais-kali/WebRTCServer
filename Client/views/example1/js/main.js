/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */

'use strict';

const GetOfferBtn = document.getElementById('GetOfferBtn');
const SetAnswerBtn = document.getElementById('SetAnswerBtn');
const GetICEBtn = document.getElementById('GetICEBtn')
const SetICEBtn = document.getElementById('SetICEBtn')

SetAnswerBtn.disabled = true;
// GetICEBtn.disabled = true;
SetICEBtn.disabled = true;

const localVideo = document.getElementById('localVideo');
const remoteVideo = document.getElementById('remoteVideo');

let pc;
let localStream;

var iceArray = [];

GetOfferBtn.onclick = async () => {
  localStream = await navigator.mediaDevices.getUserMedia({ audio: true, video: true });
  localVideo.srcObject = localStream;

  await createPeerConnection();

  // const offer = await pc.createOffer();
  // await pc.setLocalDescription(offer);

  GetOfferBtn.disabled = true;
  SetAnswerBtn.disabled = false;

  // output('createOffer -> onOfferSuccess');
  // output('Offer SDP:begin');
  // output(offer.sdp);
  // output('Offer SDP:end');

  SetICEBtn.disabled = false;
  SetICEBtn.onclick = SetICE;
};

SetAnswerBtn.onclick = async () => {
  var sdp = new RTCSessionDescription({
    type: 'answer',
    sdp: input()
  });
  pc.setRemoteDescription(sdp);
};

async function hangup() {
  if (pc) {
    pc.close();
    pc = null;
  }
  localStream.getTracks().forEach(track => track.stop());
  localStream = null;
  GetOfferBtn.disabled = false;
  hangupButton.disabled = true;
};

function createPeerConnection() {
  pc = new RTCPeerConnection();

  pc.onicecandidate = onIceCandidate;

  pc.ontrack = e => remoteVideo.srcObject = e.streams[0];
  localStream.getTracks().forEach(track => pc.addTrack(track, localStream));
}

function onIceCandidate(evt) {
  if (evt.candidate) {
    iceArray.push(evt.candidate);
  } else {
    output("end of ice candidate" + evt.eventPhase);
    GetICEBtn.disabled = false;
    GetICEBtn.onclick = GetICE;
  }
}

function GetICE() {
  output('ICE:begin');
  output(JSON.stringify(iceArray));
  iceArray = [];
  output('ICE:end');
}

function SetICE() {
  var ices = JSON.parse(input());
  for (var ice of ices) {
    var iceObj = new RTCIceCandidate(ice);
    pc.addIceCandidate(iceObj);
  }
}

function output(log) {
  var stdout = document.getElementById('stdout');
  stdout.value = stdout.value + log + '\n';
}

function input() {
  var stdin = document.getElementById('stdin');
  var input = stdin.value;
  stdin.value = '';
  return input;
}

