/* GStreamer Intel MSDK plugin
 * Copyright (c) 2016, Oblong Industries, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <mfxplugin.h>
#include <mfxvp8.h>

#include "gstmsdkvp8enc.h"

GST_DEBUG_CATEGORY_EXTERN (gst_msdkvp8enc_debug);
#define GST_CAT_DEFAULT gst_msdkvp8enc_debug

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-vp8, "
        "framerate = (fraction) [0/1, MAX], "
        "width = (int) [ 1, MAX ], height = (int) [ 1, MAX ], "
        "profile = (string) { 0, 1, 2, 3 }")
    );

#define gst_msdkvp8enc_parent_class parent_class
G_DEFINE_TYPE (GstMsdkVP8Enc, gst_msdkvp8enc, GST_TYPE_MSDKVPXENC);

static const gchar *
gst_msdkvp8enc_media_type (void)
{
  return "video/vp8";
}

static gboolean
gst_msdkvp8enc_configure (GstMsdkEnc * encoder)
{
  GstMsdkVP8Enc *thiz = GST_MSDKVP8ENC (encoder);
  GstMsdkVPXEnc *vpx_enc = GST_MSDKVPXENC (encoder);
  mfxSession session;
  mfxStatus status;

  if (encoder->hardware) {
    session = msdk_context_get_session (encoder->context);
    status = MFXVideoUSER_Load (session, &MFX_PLUGINID_VP8E_HW, 1);
    if (status < MFX_ERR_NONE) {
      GST_ERROR_OBJECT (thiz, "Media SDK Plugin load failed (%s)",
          msdk_status_to_string (status));
      return FALSE;
    } else if (status > MFX_ERR_NONE) {
      GST_WARNING_OBJECT (thiz, "Media SDK Plugin load warning: %s",
          msdk_status_to_string (status));
    }
  }

  encoder->param.mfx.CodecId = MFX_CODEC_VP8;
  encoder->param.mfx.CodecProfile = vpx_enc->profile;
  encoder->param.mfx.CodecLevel = 0;

  return TRUE;
}

static void
gst_msdkvp8enc_class_init (GstMsdkVP8EncClass * klass)
{
  GstElementClass *element_class;
  GstMsdkEncClass *encoder_class;
  GstMsdkVPXEncClass *vpx_encoder_class;

  element_class = GST_ELEMENT_CLASS (klass);
  encoder_class = GST_MSDKENC_CLASS (klass);
  vpx_encoder_class = GST_MSDKVPXENC_CLASS (klass);

  vpx_encoder_class->media_type = GST_DEBUG_FUNCPTR (gst_msdkvp8enc_media_type);
  encoder_class->configure = GST_DEBUG_FUNCPTR (gst_msdkvp8enc_configure);
  gst_element_class_set_static_metadata (element_class,
      "Intel MSDK VP8 encoder",
      "Codec/Encoder/Video",
      "VP8 video encoder based on Intel Media SDK",
      "Josep Torra <jtorra@oblong.com>");

  gst_element_class_add_static_pad_template (element_class, &src_factory);
}

static void
gst_msdkvp8enc_init (GstMsdkVP8Enc * thiz)
{
}
