/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */

'use strict';

const CallBtn = document.getElementById('CallBtn');

const localVideo = document.getElementById('localVideo');
const remoteVideo = document.getElementById('remoteVideo');

let pc;
let localStream;

CallBtn.onclick = async () => {

};


const ws = new WebSocket('wss://192.168.1.22');

// Handle connection open
ws.onopen = function () {
  console.log('Connected to WebSocket server');
};