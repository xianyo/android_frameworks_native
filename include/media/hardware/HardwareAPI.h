/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HARDWARE_API_H_

#define HARDWARE_API_H_

#include <media/hardware/OMXPluginBase.h>
#include <media/hardware/MetadataBufferType.h>
#include <system/window.h>
#include <utils/RefBase.h>

#include <OMX_Component.h>

namespace android {

// A pointer to this struct is passed to the OMX_SetParameter when the extension
// index for the 'OMX.google.android.index.enableAndroidNativeBuffers' extension
// is given.
//
// When Android native buffer use is disabled for a port (the default state),
// the OMX node should operate as normal, and expect UseBuffer calls to set its
// buffers.  This is the mode that will be used when CPU access to the buffer is
// required.
//
// When Android native buffer use has been enabled for a given port, the video
// color format for the port is to be interpreted as an Android pixel format
// rather than an OMX color format.  Enabling Android native buffers may also
// change how the component receives the native buffers.  If store-metadata-mode
// is enabled on the port, the component will receive the buffers as specified
// in the section below. Otherwise, unless the node supports the
// 'OMX.google.android.index.useAndroidNativeBuffer2' extension, it should
// expect to receive UseAndroidNativeBuffer calls (via OMX_SetParameter) rather
// than UseBuffer calls for that port.
struct EnableAndroidNativeBuffersParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL enable;
};

// A pointer to this struct is passed to OMX_SetParameter() when the extension
// index "OMX.google.android.index.storeMetaDataInBuffers"
// is given.
//
// When meta data is stored in the video buffers passed between OMX clients
// and OMX components, interpretation of the buffer data is up to the
// buffer receiver, and the data may or may not be the actual video data, but
// some information helpful for the receiver to locate the actual data.
// The buffer receiver thus needs to know how to interpret what is stored
// in these buffers, with mechanisms pre-determined externally. How to
// interpret the meta data is outside of the scope of this method.
//
// Currently, this is specifically used to pass meta data from video source
// (camera component, for instance) to video encoder to avoid memcpying of
// input video frame data. To do this, bStoreMetaData is set to OMX_TRUE.
// If bStoreMetaData is set to false, real YUV frame data will be stored
// in the buffers. In addition, if no OMX_SetParameter() call is made
// with the corresponding extension index, real YUV data is stored
// in the buffers.
//
// For video decoder output port, the metadata buffer layout is defined below.
//
// Metadata buffers are registered with the component using UseBuffer calls.
struct StoreMetaDataInBuffersParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bStoreMetaData;
};

// Meta data buffer layout used to transport output frames to the decoder for
// dynamic buffer handling.
struct VideoDecoderOutputMetaData {
  MetadataBufferType eType;
  buffer_handle_t pHandle;
};

// A pointer to this struct is passed to OMX_SetParameter() when the extension
// index "OMX.google.android.index.prepareForAdaptivePlayback" is given.
//
// This method is used to signal a video decoder, that the user has requested
// seamless resolution change support (if bEnable is set to OMX_TRUE).
// nMaxFrameWidth and nMaxFrameHeight are the dimensions of the largest
// anticipated frames in the video.  If bEnable is OMX_FALSE, no resolution
// change is expected, and the nMaxFrameWidth/Height fields are unused.
//
// If the decoder supports dynamic output buffers, it may ignore this
// request.  Otherwise, it shall request resources in such a way so that it
// avoids full port-reconfiguration (due to output port-definition change)
// during resolution changes.
//
// DO NOT USE THIS STRUCTURE AS IT WILL BE REMOVED.  INSTEAD, IMPLEMENT
// METADATA SUPPORT FOR VIDEO DECODERS.
struct PrepareForAdaptivePlaybackParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bEnable;
    OMX_U32 nMaxFrameWidth;
    OMX_U32 nMaxFrameHeight;
};

// A pointer to this struct is passed to OMX_SetParameter when the extension
// index for the 'OMX.google.android.index.useAndroidNativeBuffer' extension is
// given.  This call will only be performed if a prior call was made with the
// 'OMX.google.android.index.enableAndroidNativeBuffers' extension index,
// enabling use of Android native buffers.
struct UseAndroidNativeBufferParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_PTR pAppPrivate;
    OMX_BUFFERHEADERTYPE **bufferHeader;
    const sp<ANativeWindowBuffer>& nativeBuffer;
};

// A pointer to this struct is passed to OMX_GetParameter when the extension
// index for the 'OMX.google.android.index.getAndroidNativeBufferUsage'
// extension is given.  The usage bits returned from this query will be used to
// allocate the Gralloc buffers that get passed to the useAndroidNativeBuffer
// command.
struct GetAndroidNativeBufferUsageParams {
    OMX_U32 nSize;              // IN
    OMX_VERSIONTYPE nVersion;   // IN
    OMX_U32 nPortIndex;         // IN
    OMX_U32 nUsage;             // OUT
};

// An enum OMX_COLOR_FormatAndroidOpaque to indicate an opaque colorformat
// is declared in media/stagefright/openmax/OMX_IVCommon.h
// This will inform the encoder that the actual
// colorformat will be relayed by the GRalloc Buffers.
// OMX_COLOR_FormatAndroidOpaque  = 0x7F000001,

// A pointer to this struct is passed to OMX_SetParameter when the extension
// index for the 'OMX.google.android.index.prependSPSPPSToIDRFrames' extension
// is given.
// A successful result indicates that future IDR frames will be prefixed by
// SPS/PPS.
struct PrependSPSPPSToIDRFramesParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_BOOL bEnable;
};

// Structure describing a media image (frame)
// Currently only supporting YUV
struct MediaImage {
    enum Type {
        MEDIA_IMAGE_TYPE_UNKNOWN = 0,
        MEDIA_IMAGE_TYPE_YUV,
    };

    enum PlaneIndex {
        Y = 0,
        U,
        V,
        MAX_NUM_PLANES
    };

    Type mType;
    size_t mNumPlanes;              // number of planes
    size_t mWidth;                  // width of largest plane
    size_t mHeight;                 // height of largest plane
    size_t mBitDepth;               // useable bit depth
    struct PlaneInfo {
        size_t mOffset;             // offset of first pixel of the plane in bytes
                                    // from buffer offset
        size_t mColInc;             // column increment in bytes
        size_t mRowInc;             // row increment in bytes
        size_t mHorizSubsampling;   // subsampling compared to the largest plane
        size_t mVertSubsampling;    // subsampling compared to the largest plane
    };
    PlaneInfo mPlane[MAX_NUM_PLANES];
};

// A pointer to this struct is passed to OMX_GetParameter when the extension
// index for the 'OMX.google.android.index.describeColorFormat'
// extension is given.  This method can be called from any component state
// other than invalid.  The color-format, frame width/height, and stride/
// slice-height parameters are ones that are associated with a raw video
// port (input or output), but the stride/slice height parameters may be
// incorrect.  The component shall fill out the MediaImage structure that
// corresponds to the described raw video format, and the potentially corrected
// stride and slice-height info.
//
// For non-YUV packed planar/semiplanar image formats, the component shall set
// mNumPlanes to 0, and mType to MEDIA_IMAGE_TYPE_UNKNOWN.
struct DescribeColorFormatParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    // input: parameters from OMX_VIDEO_PORTDEFINITIONTYPE
    OMX_COLOR_FORMATTYPE eColorFormat;
    OMX_U32 nFrameWidth;
    OMX_U32 nFrameHeight;
    OMX_U32 nStride;
    OMX_U32 nSliceHeight;

    // output: fill out the MediaImage fields
    MediaImage sMediaImage;
};

// A pointer to this struct is passed to OMX_SetParameter or OMX_GetParameter
// when the extension index for the
// 'OMX.google.android.index.configureVideoTunnelMode' extension is  given.
// If the extension is supported then tunneled playback mode should be supported
// by the codec. If bTunneled is set to OMX_TRUE then the video decoder should
// operate in "tunneled" mode and output its decoded frames directly to the
// sink. In this case nAudioHwSync is the HW SYNC ID of the audio HAL Output
// stream to sync the video with. If bTunneled is set to OMX_FALSE, "tunneled"
// mode should be disabled and nAudioHwSync should be ignored.
// OMX_GetParameter is used to query tunneling configuration. bTunneled should
// return whether decoder is operating in tunneled mode, and if it is,
// pSidebandWindow should contain the codec allocated sideband window handle.
struct ConfigureVideoTunnelModeParams {
    OMX_U32 nSize;              // IN
    OMX_VERSIONTYPE nVersion;   // IN
    OMX_U32 nPortIndex;         // IN
    OMX_BOOL bTunneled;         // IN/OUT
    OMX_U32 nAudioHwSync;       // IN
    OMX_PTR pSidebandWindow;    // OUT
};

// A pointer to this struct is passed to OMX_GetParameter when the extension
// index for the 'OMX.google.android.index.disableAVCReorder'
// extension is given.  The usage bits bDisable indicates that AVC reorder
// should be enable or disable.
struct DisableAVCReorderParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_BOOL bDisable;
};

}  // namespace android

extern android::OMXPluginBase *createOMXPlugin();

#endif  // HARDWARE_API_H_
