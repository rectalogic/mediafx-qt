// Copyright (C) 2024 Andrew Wason
//
// This file is part of mediaFX.
//
// mediaFX is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// mediaFX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with mediaFX.
// If not, see <https://www.gnu.org/licenses/>.

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
