/*
 * Copyright 2020 Derek Lesho
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __CODECAPI_H
#define __CODECAPI_H

enum eAVEncH264VProfile
{
    eAVEncH264VProfile_unknown  = 0,
    eAVEncH264VProfile_Simple = 66,
    eAVEncH264VProfile_Base = 66,
    eAVEncH264VProfile_Main = 77,
    eAVEncH264VProfile_High = 100,
    eAVEncH264VProfile_422 = 122,
    eAVEncH264VProfile_High10 = 110,
    eAVEncH264VProfile_444 = 244,
    eAVEncH264VProfile_Extended = 88,
    eAVEncH264VProfile_ScalableBase =  83,
    eAVEncH264VProfile_ScalableHigh =  86,
    eAVEncH264VProfile_MultiviewHigh = 118,
    eAVEncH264VProfile_StereoHigh = 128,
    eAVEncH264VProfile_ConstrainedBase = 256,
    eAVEncH264VProfile_UCConstrainedHigh = 257,
    eAVEncH264VProfile_UCScalableConstrainedBase = 258,
    eAVEncH264VProfile_UCScalableConstrainedHigh = 259
};

enum eAVEncH264VLevel
{
    eAVEncH264VLevel1 = 10,
    eAVEncH264VLevel1_b = 11,
    eAVEncH264VLevel1_1 = 11,
    eAVEncH264VLevel1_2 = 12,
    eAVEncH264VLevel1_3 = 13,
    eAVEncH264VLevel2 = 20,
    eAVEncH264VLevel2_1 = 21,
    eAVEncH264VLevel2_2 = 22,
    eAVEncH264VLevel3 = 30,
    eAVEncH264VLevel3_1 = 31,
    eAVEncH264VLevel3_2 = 32,
    eAVEncH264VLevel4 = 40,
    eAVEncH264VLevel4_1 = 41,
    eAVEncH264VLevel4_2 = 42,
    eAVEncH264VLevel5 = 50,
    eAVEncH264VLevel5_1 = 51,
    eAVEncH264VLevel5_2 = 52
};

DEFINE_GUID(AVDecVideoAcceleration_H264, 0xf7db8a2f, 0x4f48, 0x4ee8, 0xae, 0x31, 0x8b, 0x6e, 0xbe, 0x55, 0x8a, 0xe2);

DEFINE_GUID(CODECAPI_AVEncCodecType,                 0x08af4ac1, 0xf3f2, 0x4c74, 0x9d, 0xcf, 0x37, 0xf2, 0xec, 0x79, 0xf8, 0x26);
DEFINE_GUID(CODECAPI_GUID_AVEncMPEG1Video,           0xc8dafefe, 0xda1e, 0x4774, 0xb2, 0x7d, 0x11, 0x83, 0x0c, 0x16, 0xb1, 0xfe);
DEFINE_GUID(CODECAPI_GUID_AVEncMPEG2Video,           0x046dc19a, 0x6677, 0x4aaa, 0xa3, 0x1d, 0xc1, 0xab, 0x71, 0x6f, 0x45, 0x60);
DEFINE_GUID(CODECAPI_GUID_AVEncMPEG1Audio,           0xd4dd1362, 0xcd4a, 0x4cd6, 0x81, 0x38, 0xb9, 0x4d, 0xb4, 0x54, 0x2b, 0x04);
DEFINE_GUID(CODECAPI_GUID_AVEncMPEG2Audio,           0xee4cbb1f, 0x9c3f, 0x4770, 0x92, 0xb5, 0xfc, 0xb7, 0xc2, 0xa8, 0xd3, 0x81);
DEFINE_GUID(CODECAPI_GUID_AVEncWMV,                  0x4e0fef9b, 0x1d43, 0x41bd, 0xb8, 0xbd, 0x4d, 0x7b, 0xf7, 0x45, 0x7a, 0x2a);
DEFINE_GUID(CODECAPI_GUID_AVEndMPEG4Video,           0xdd37b12a, 0x9503, 0x4f8b, 0xb8, 0xd0, 0x32, 0x4a, 0x00, 0xc0, 0xa1, 0xcf);
DEFINE_GUID(CODECAPI_GUID_AVEncH264Video,            0x95044eab, 0x31b3, 0x47de, 0x8e, 0x75, 0x38, 0xa4, 0x2b, 0xb0, 0x3e, 0x28);
DEFINE_GUID(CODECAPI_GUID_AVEncDV,                   0x09b769c7, 0x3329, 0x44fb, 0x89, 0x54, 0xfa, 0x30, 0x93, 0x7d, 0x3d, 0x5a);
DEFINE_GUID(CODECAPI_GUID_AVEncWMAPro,               0x1955f90c, 0x33f7, 0x4a68, 0xab, 0x81, 0x53, 0xf5, 0x65, 0x71, 0x25, 0xc4);
DEFINE_GUID(CODECAPI_GUID_AVEncWMALossless,          0x55ca7265, 0x23d8, 0x4761, 0x90, 0x31, 0xb7, 0x4f, 0xbe, 0x12, 0xf4, 0xc1);
DEFINE_GUID(CODECAPI_GUID_AVEncWMAVoice,             0x13ed18cb, 0x50e8, 0x4276, 0xa2, 0x88, 0xa6, 0xaa, 0x22, 0x83, 0x82, 0xd9);
DEFINE_GUID(CODECAPI_GUID_AVEncDolbyDigitalPro,      0xf5be76cc, 0x0ff8, 0x40eb, 0x9c, 0xb1, 0xbb, 0xa9, 0x40, 0x04, 0xd4, 0x4f);
DEFINE_GUID(CODECAPI_GUID_AVEncDolbyDigitalConsumer, 0xc1a7bf6c, 0x0059, 0x4bfa, 0x94, 0xef, 0xef, 0x74, 0x7a, 0x76, 0x8d, 0x52);
DEFINE_GUID(CODECAPI_GUID_AVEncDolbyDigitalPlus,     0x698d1b80, 0xf7dd, 0x415c, 0x97, 0x1c, 0x42, 0x49, 0x2a, 0x20, 0x56, 0xc6);
DEFINE_GUID(CODECAPI_GUID_AVEncDTSHD,                0x2052e630, 0x469d, 0x4bfb, 0x80, 0xca, 0x1d, 0x65, 0x6e, 0x7e, 0x91, 0x8f);
DEFINE_GUID(CODECAPI_GUID_AVEncDTS,                  0x45fbcaa2, 0x5e6e, 0x4ab0, 0x88, 0x93, 0x59, 0x03, 0xbe, 0xe9, 0x3a, 0xcf);
DEFINE_GUID(CODECAPI_GUID_AVEncMLP,                  0x05f73e29, 0xf0d1, 0x431e, 0xa4, 0x1c, 0xa4, 0x74, 0x32, 0xec, 0x5a, 0x66);
DEFINE_GUID(CODECAPI_GUID_AVEncPCM,                  0x844be7f4, 0x26cf, 0x4779, 0xb3, 0x86, 0xcc, 0x05, 0xd1, 0x87, 0x99, 0x0c);
DEFINE_GUID(CODECAPI_GUID_AVEncSDDS,                 0x1dc1b82f, 0x11c8, 0x4c71, 0xb7, 0xb6, 0xee, 0x3e, 0xb9, 0xbc, 0x2b, 0x94);

#endif /* __CODECAPI_H */
