/*
* Copyright (c) 2009-2011 Intel Corporation.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


// #define LOG_NDEBUG 0
#define LOG_TAG "OMXVideoDecoderMPEG4"
#include <utils/Log.h>
#include "OMXVideoDecoderMPEG4.h"

// Be sure to have an equal string in VideoDecoderHost.cpp (libmix)
static const char* MPEG4_MIME_TYPE = "video/mpeg4";

OMXVideoDecoderMPEG4::OMXVideoDecoderMPEG4() {
    LOGV("OMXVideoDecoderMPEG4 is constructed.");
    mVideoDecoder = createVideoDecoder(MPEG4_MIME_TYPE);
    if (!mVideoDecoder) {
        LOGE("createVideoDecoder failed for \"%s\"", MPEG4_MIME_TYPE);
    }
    BuildHandlerList();
}

OMXVideoDecoderMPEG4::~OMXVideoDecoderMPEG4() {
    LOGV("OMXVideoDecoderMPEG4 is destructed.");
}

OMX_ERRORTYPE OMXVideoDecoderMPEG4::InitInputPortFormatSpecific(OMX_PARAM_PORTDEFINITIONTYPE *paramPortDefinitionInput) {
    // OMX_PARAM_PORTDEFINITIONTYPE
    paramPortDefinitionInput->nBufferCountActual = INPORT_ACTUAL_BUFFER_COUNT;
    paramPortDefinitionInput->nBufferCountMin = INPORT_MIN_BUFFER_COUNT;
    paramPortDefinitionInput->nBufferSize = INPORT_BUFFER_SIZE;
    paramPortDefinitionInput->format.video.cMIMEType = (OMX_STRING)MPEG4_MIME_TYPE;
    paramPortDefinitionInput->format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG4;

    // OMX_VIDEO_PARAM_MPEG4TYPE
    memset(&mParamMpeg4, 0, sizeof(mParamMpeg4));
    SetTypeHeader(&mParamMpeg4, sizeof(mParamMpeg4));
    mParamMpeg4.nPortIndex = INPORT_INDEX;
    // TODO: check eProfile/eLevel
    mParamMpeg4.eProfile = OMX_VIDEO_MPEG4ProfileAdvancedSimple; //OMX_VIDEO_MPEG4ProfileSimple;
    mParamMpeg4.eLevel = OMX_VIDEO_MPEG4Level3;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE OMXVideoDecoderMPEG4::ProcessorInit(void) {
    return OMXVideoDecoderBase::ProcessorInit();
}

OMX_ERRORTYPE OMXVideoDecoderMPEG4::ProcessorDeinit(void) {
    return OMXVideoDecoderBase::ProcessorDeinit();
}

OMX_ERRORTYPE OMXVideoDecoderMPEG4::ProcessorProcess(
        OMX_BUFFERHEADERTYPE ***pBuffers,
        buffer_retain_t *retains,
        OMX_U32 numberBuffers) {

    return OMXVideoDecoderBase::ProcessorProcess(pBuffers, retains, numberBuffers);
}

OMX_ERRORTYPE OMXVideoDecoderMPEG4::PrepareConfigBuffer(VideoConfigBuffer *p) {
    return OMXVideoDecoderBase::PrepareConfigBuffer(p);
}

OMX_ERRORTYPE OMXVideoDecoderMPEG4::PrepareDecodeBuffer(OMX_BUFFERHEADERTYPE *buffer, buffer_retain_t *retain, VideoDecodeBuffer *p) {
    return OMXVideoDecoderBase::PrepareDecodeBuffer(buffer, retain, p);
}

OMX_ERRORTYPE OMXVideoDecoderMPEG4::BuildHandlerList(void) {
    OMXVideoDecoderBase::BuildHandlerList();
    AddHandler(OMX_IndexParamVideoMpeg4, GetParamVideoMpeg4, SetParamVideoMpeg4);
    AddHandler(static_cast<OMX_INDEXTYPE>(OMX_IndexExtEnableNativeBuffer),GetNativeBufferMode,SetNativeBufferMode);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE OMXVideoDecoderMPEG4::GetParamVideoMpeg4(OMX_PTR pStructure) {
    OMX_ERRORTYPE ret;
    OMX_VIDEO_PARAM_MPEG4TYPE *p = (OMX_VIDEO_PARAM_MPEG4TYPE *)pStructure;
    CHECK_TYPE_HEADER(p);
    CHECK_PORT_INDEX(p, INPORT_INDEX);

    memcpy(p, &mParamMpeg4, sizeof(*p));
    return OMX_ErrorNone;
}

OMX_ERRORTYPE OMXVideoDecoderMPEG4::SetParamVideoMpeg4(OMX_PTR pStructure) {
    OMX_ERRORTYPE ret;
    OMX_VIDEO_PARAM_MPEG4TYPE *p = (OMX_VIDEO_PARAM_MPEG4TYPE *)pStructure;
    CHECK_TYPE_HEADER(p);
    CHECK_PORT_INDEX(p, INPORT_INDEX);
    CHECK_SET_PARAM_STATE();

    // TODO: do we need to check if port is enabled?
    // TODO: see SetPortMpeg4Param implementation - Can we make simple copy????
    memcpy(&mParamMpeg4, p, sizeof(mParamMpeg4));
    return OMX_ErrorNone;
}
OMX_ERRORTYPE OMXVideoDecoderMPEG4::GetNativeBufferMode(OMX_PTR pStructure) {
     OMX_ERRORTYPE ret;
     return OMX_ErrorNone; //would not be here
}


#define MAX_OUTPUT_BUFFER_COUNT_FOR_MPEG4 10
OMX_ERRORTYPE OMXVideoDecoderMPEG4::SetNativeBufferMode(OMX_PTR pStructure) {
    OMX_ERRORTYPE ret;
    //EnableAndroidNativeBuffersParams *param = (EnableAndroidNativeBuffersParams*)pStructure;
    //CHECK_TYPE_HEADER(param);
    CHECK_SET_PARAM_STATE();
    mNativeBufferMode = true;
    PortVideo *port = NULL;
    port = static_cast<PortVideo *>(this->ports[OUTPORT_INDEX]);
    OMX_PARAM_PORTDEFINITIONTYPE port_def;
    memcpy(&port_def,port->GetPortDefinition(),sizeof(port_def));
    port_def.nBufferCountMin = 1;
    port_def.nBufferCountActual = MAX_OUTPUT_BUFFER_COUNT_FOR_MPEG4;
    port_def.format.video.cMIMEType = (OMX_STRING)VA_VED_RAW_MIME_TYPE;
    port_def.format.video.eColorFormat =static_cast<OMX_COLOR_FORMATTYPE>(VA_VED_COLOR_FORMAT);
    port->SetPortDefinition(&port_def,true);
    return OMX_ErrorNone;
}


DECLARE_OMX_COMPONENT("OMX.Intel.VideoDecoder.MPEG4", "video_decoder.mpeg4", OMXVideoDecoderMPEG4);

