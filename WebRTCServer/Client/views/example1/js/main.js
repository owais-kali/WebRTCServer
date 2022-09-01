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
SetAnswerBtn.disabled = true;

const localVideo = document.getElementById('localVideo');
const remoteVideo = document.getElementById('remoteVideo');

let pc;
let localStream;


GetOfferBtn.onclick = async () => {
  localStream = await navigator.mediaDevices.getUserMedia({audio: true, video: true});
  localVideo.srcObject = localStream;


  GetOfferBtn.disabled = true;
  hangupButton.disabled = false;

  signaling.postMessage({type: 'ready'});
};

hangupButton.onclick = async () => {
  hangup();
  signaling.postMessage({type: 'bye'});
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
  pc.onicecandidate = e => {
    const message = {
      type: 'candidate',
      candidate: null,
    };
    if (e.candidate) {
      message.candidate = e.candidate.candidate;
      message.sdpMid = e.candidate.sdpMid;
      message.sdpMLineIndex = e.candidate.sdpMLineIndex;
    }
    signaling.postMessage(message);
  };
  pc.ontrack = e => remoteVideo.srcObject = e.streams[0];
  localStream.getTracks().forEach(track => pc.addTrack(track, localStream));
}

async function makeCall() {
  await createPeerConnection();

  const offer = await pc.createOffer();
  signaling.postMessage({type: 'offer', sdp: offer.sdp});
  await pc.setLocalDescription(offer);
}

async function handleOffer(offer) {
  if (pc) {
    console.error('existing peerconnection');
    return;
  }
  await createPeerConnection();
  await pc.setRemoteDescription(offer);

  const answer = await pc.createAnswer();
  signaling.postMessage({type: 'answer', sdp: answer.sdp});
  await pc.setLocalDescription(answer);
}

async function handleAnswer(answer) {
  if (!pc) {
    console.error('no peerconnection');
    return;
  }
  await pc.setRemoteDescription(answer);
}

async function handleCandidate(candidate) {
  if (!pc) {
    console.error('no peerconnection');
    return;
  }
  if (!candidate.candidate) {
    await pc.addIceCandidate(null);
  } else {
    await pc.addIceCandidate(candidate);
  }
}

