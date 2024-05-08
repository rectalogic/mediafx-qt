// Copyright (C) 2024 Andrew Wason
// SPDX-License-Identifier: GPL-3.0-or-later

function onClipEnded() {
    root.RenderSession.session.currentRenderTimeChanged.connect(initializeNextClip);
};

function onCurrentFrameTimechanged() {
    const clip = internal.currentClip;
    if (internal.transitionStartTime > 0 && clip.currentFrameTime.start >= internal.transitionStartTime) {
        if (clip.endTransition) {
            if (!clip.endTransition.parent) {
                clip.endTransition.parent = _transitionContainer;
                clip.endTransition.anchors.fill = _transitionContainer;
                clip.endTransition.source = _mainVideoRenderer;
                clip.endTransition.dest = _auxVideoRenderer;
                root.currentTransition = clip.endTransition;

                _mainVideoRenderer.mediaClip = internal.currentClip;
                _auxVideoRenderer.mediaClip = internal.nextClip;
                _mainVideoRenderer.visible = false;
                _transitionContainer.visible = true;
            }
            clip.endTransition.time = (clip.currentFrameTime.start - internal.transitionStartTime) / (clip.endTime - internal.transitionStartTime);
        }
    }
};

function initializeNextClip() {
    root.RenderSession.session.currentRenderTimeChanged.disconnect(initializeNextClip);
    if (internal.currentClipIndex + 1 < root.mediaClips.length) {
        internal.currentClip.currentFrameTimeChanged.disconnect(onCurrentFrameTimechanged);
        internal.currentClip.clipEnded.disconnect(onClipEnded);
        internal.currentClipIndex += 1;
        initializeClip();
    }
};

function initializeClip() {
    if (internal.currentClip) {
        if (internal.currentClip.endTransition) {
            internal.currentClip.endTransition.parent = null;
            internal.currentClip.endTransition.destroy();
            root.currentTransition = null;
        }
        internal.currentClip.destroy();
        internal.currentClip = internal.nextClip;
        internal.nextClip = null;
    }
    if (!internal.currentClip) {
        internal.currentClip = root.mediaClips[internal.currentClipIndex].createObject(null);
    }

    // Last clip
    if (internal.currentClipIndex >= root.mediaClips.length - 1) {
        internal.currentClip.clipEnded.connect(root.mediaSequenceEnded)
    }
    else {
        if (!internal.nextClip) {
            internal.nextClip = root.mediaClips[internal.currentClipIndex + 1].createObject(null);
        }
        const transition = internal.currentClip.endTransition;
        if (transition && internal.nextClip) {
            const clampedTransitionDuration = Math.min(Math.min(transition.duration, internal.currentClip.duration), internal.nextClip.duration);
            internal.transitionStartTime = internal.currentClip.endTime - clampedTransitionDuration;
        }
        else {
            internal.transitionStartTime = -1;
        }
        internal.currentClip.currentFrameTimeChanged.connect(onCurrentFrameTimechanged);
        internal.currentClip.clipEnded.connect(onClipEnded);
    }

    _mainVideoRenderer.mediaClip = internal.currentClip;
    _auxVideoRenderer.mediaClip = null;
    _mainVideoRenderer.visible = true;
    _transitionContainer.visible = false;
};
