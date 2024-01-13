// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

function onClipEnded() {
    MediaManager.frameRendered.connect(nextClip);
};

function onCurrentFrameTimechanged() {
    var clip = root.mediaClips[internal.currentClipIndex];
    if (clip.currentFrameTime.start >= internal.mixStartTime) {
        internal.state = "mixer";
        root.mediaMixers[internal.currentMixerIndex].time = (clip.currentFrameTime.start - internal.mixStartTime) / (clip.endTime - internal.mixStartTime);
    }
};

function nextClip() {
    MediaManager.frameRendered.disconnect(nextClip);
    if (internal.currentClipIndex + 1 < root.mediaClips.length) {
        var clip = root.mediaClips[internal.currentClipIndex];
        clip.currentFrameTimeChanged.disconnect(onCurrentFrameTimechanged);
        clip.clipEnded.disconnect(onClipEnded);
        internal.currentClipIndex += 1;
        initializeClip();
    }
    internal.currentMixerIndex = (internal.currentMixerIndex + 1) % root.mediaMixers.length;
    root.mediaMixers[internal.currentMixerIndex].time = 0;
};

function initializeClip() {
    var clip = root.mediaClips[internal.currentClipIndex];
    // Last clip
    if (internal.currentClipIndex >= root.mediaClips.length - 1) {
        clip.onClipEnded.connect(root.mediaSequenceEnded)
    }
    else {
        var mixer = root.mediaMixers[internal.currentMixerIndex];
        var clampedMixDuration = Math.min(Math.min(mixer.duration, clip.duration), root.mediaClips[internal.currentClipIndex + 1].duration);
        internal.mixStartTime = clip.endTime - clampedMixDuration;
        clip.currentFrameTimeChanged.connect(onCurrentFrameTimechanged);
        clip.clipEnded.connect(onClipEnded);
    }
    internal.state = "video";
};
