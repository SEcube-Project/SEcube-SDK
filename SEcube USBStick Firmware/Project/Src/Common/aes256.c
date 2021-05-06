/**
  ******************************************************************************
  * File Name          : aes256.c
  * Description        : This file includes the implementation of the functions
  *                      for computing the AES and CMAC-AES algorithms.
  ******************************************************************************
  *
  * Copyright(c) 2016-present Blu5 Group <https://www.blu5group.com>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 3 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, see <https://www.gnu.org/licenses/>.
  *
  ******************************************************************************
  */

#include "aes256.h"

const uint32_t B5Te0_S[256] = {
    0xc66363a5, 0xf87c7c84, 0xee777799, 0xf67b7b8d,
    0xfff2f20d, 0xd66b6bbd, 0xde6f6fb1, 0x91c5c554,
    0x60303050, 0x02010103, 0xce6767a9, 0x562b2b7d,
    0xe7fefe19, 0xb5d7d762, 0x4dababe6, 0xec76769a,
    0x8fcaca45, 0x1f82829d, 0x89c9c940, 0xfa7d7d87,
    0xeffafa15, 0xb25959eb, 0x8e4747c9, 0xfbf0f00b,
    0x41adadec, 0xb3d4d467, 0x5fa2a2fd, 0x45afafea,
    0x239c9cbf, 0x53a4a4f7, 0xe4727296, 0x9bc0c05b,
    0x75b7b7c2, 0xe1fdfd1c, 0x3d9393ae, 0x4c26266a,
    0x6c36365a, 0x7e3f3f41, 0xf5f7f702, 0x83cccc4f,
    0x6834345c, 0x51a5a5f4, 0xd1e5e534, 0xf9f1f108,
    0xe2717193, 0xabd8d873, 0x62313153, 0x2a15153f,
    0x0804040c, 0x95c7c752, 0x46232365, 0x9dc3c35e,
    0x30181828, 0x379696a1, 0x0a05050f, 0x2f9a9ab5,
    0x0e070709, 0x24121236, 0x1b80809b, 0xdfe2e23d,
    0xcdebeb26, 0x4e272769, 0x7fb2b2cd, 0xea75759f,
    0x1209091b, 0x1d83839e, 0x582c2c74, 0x341a1a2e,
    0x361b1b2d, 0xdc6e6eb2, 0xb45a5aee, 0x5ba0a0fb,
    0xa45252f6, 0x763b3b4d, 0xb7d6d661, 0x7db3b3ce,
    0x5229297b, 0xdde3e33e, 0x5e2f2f71, 0x13848497,
    0xa65353f5, 0xb9d1d168, 0x00000000, 0xc1eded2c,
    0x40202060, 0xe3fcfc1f, 0x79b1b1c8, 0xb65b5bed,
    0xd46a6abe, 0x8dcbcb46, 0x67bebed9, 0x7239394b,
    0x944a4ade, 0x984c4cd4, 0xb05858e8, 0x85cfcf4a,
    0xbbd0d06b, 0xc5efef2a, 0x4faaaae5, 0xedfbfb16,
    0x864343c5, 0x9a4d4dd7, 0x66333355, 0x11858594,
    0x8a4545cf, 0xe9f9f910, 0x04020206, 0xfe7f7f81,
    0xa05050f0, 0x783c3c44, 0x259f9fba, 0x4ba8a8e3,
    0xa25151f3, 0x5da3a3fe, 0x804040c0, 0x058f8f8a,
    0x3f9292ad, 0x219d9dbc, 0x70383848, 0xf1f5f504,
    0x63bcbcdf, 0x77b6b6c1, 0xafdada75, 0x42212163,
    0x20101030, 0xe5ffff1a, 0xfdf3f30e, 0xbfd2d26d,
    0x81cdcd4c, 0x180c0c14, 0x26131335, 0xc3ecec2f,
    0xbe5f5fe1, 0x359797a2, 0x884444cc, 0x2e171739,
    0x93c4c457, 0x55a7a7f2, 0xfc7e7e82, 0x7a3d3d47,
    0xc86464ac, 0xba5d5de7, 0x3219192b, 0xe6737395,
    0xc06060a0, 0x19818198, 0x9e4f4fd1, 0xa3dcdc7f,
    0x44222266, 0x542a2a7e, 0x3b9090ab, 0x0b888883,
    0x8c4646ca, 0xc7eeee29, 0x6bb8b8d3, 0x2814143c,
    0xa7dede79, 0xbc5e5ee2, 0x160b0b1d, 0xaddbdb76,
    0xdbe0e03b, 0x64323256, 0x743a3a4e, 0x140a0a1e,
    0x924949db, 0x0c06060a, 0x4824246c, 0xb85c5ce4,
    0x9fc2c25d, 0xbdd3d36e, 0x43acacef, 0xc46262a6,
    0x399191a8, 0x319595a4, 0xd3e4e437, 0xf279798b,
    0xd5e7e732, 0x8bc8c843, 0x6e373759, 0xda6d6db7,
    0x018d8d8c, 0xb1d5d564, 0x9c4e4ed2, 0x49a9a9e0,
    0xd86c6cb4, 0xac5656fa, 0xf3f4f407, 0xcfeaea25,
    0xca6565af, 0xf47a7a8e, 0x47aeaee9, 0x10080818,
    0x6fbabad5, 0xf0787888, 0x4a25256f, 0x5c2e2e72,
    0x381c1c24, 0x57a6a6f1, 0x73b4b4c7, 0x97c6c651,
    0xcbe8e823, 0xa1dddd7c, 0xe874749c, 0x3e1f1f21,
    0x964b4bdd, 0x61bdbddc, 0x0d8b8b86, 0x0f8a8a85,
    0xe0707090, 0x7c3e3e42, 0x71b5b5c4, 0xcc6666aa,
    0x904848d8, 0x06030305, 0xf7f6f601, 0x1c0e0e12,
    0xc26161a3, 0x6a35355f, 0xae5757f9, 0x69b9b9d0,
    0x17868691, 0x99c1c158, 0x3a1d1d27, 0x279e9eb9,
    0xd9e1e138, 0xebf8f813, 0x2b9898b3, 0x22111133,
    0xd26969bb, 0xa9d9d970, 0x078e8e89, 0x339494a7,
    0x2d9b9bb6, 0x3c1e1e22, 0x15878792, 0xc9e9e920,
    0x87cece49, 0xaa5555ff, 0x50282878, 0xa5dfdf7a,
    0x038c8c8f, 0x59a1a1f8, 0x09898980, 0x1a0d0d17,
    0x65bfbfda, 0xd7e6e631, 0x844242c6, 0xd06868b8,
    0x824141c3, 0x299999b0, 0x5a2d2d77, 0x1e0f0f11,
    0x7bb0b0cb, 0xa85454fc, 0x6dbbbbd6, 0x2c16163a
};

const uint32_t B5Te1_S[256] = {
    0xa5c66363, 0x84f87c7c, 0x99ee7777, 0x8df67b7b,
    0x0dfff2f2, 0xbdd66b6b, 0xb1de6f6f, 0x5491c5c5,
    0x50603030, 0x03020101, 0xa9ce6767, 0x7d562b2b,
    0x19e7fefe, 0x62b5d7d7, 0xe64dabab, 0x9aec7676,
    0x458fcaca, 0x9d1f8282, 0x4089c9c9, 0x87fa7d7d,
    0x15effafa, 0xebb25959, 0xc98e4747, 0x0bfbf0f0,
    0xec41adad, 0x67b3d4d4, 0xfd5fa2a2, 0xea45afaf,
    0xbf239c9c, 0xf753a4a4, 0x96e47272, 0x5b9bc0c0,
    0xc275b7b7, 0x1ce1fdfd, 0xae3d9393, 0x6a4c2626,
    0x5a6c3636, 0x417e3f3f, 0x02f5f7f7, 0x4f83cccc,
    0x5c683434, 0xf451a5a5, 0x34d1e5e5, 0x08f9f1f1,
    0x93e27171, 0x73abd8d8, 0x53623131, 0x3f2a1515,
    0x0c080404, 0x5295c7c7, 0x65462323, 0x5e9dc3c3,
    0x28301818, 0xa1379696, 0x0f0a0505, 0xb52f9a9a,
    0x090e0707, 0x36241212, 0x9b1b8080, 0x3ddfe2e2,
    0x26cdebeb, 0x694e2727, 0xcd7fb2b2, 0x9fea7575,
    0x1b120909, 0x9e1d8383, 0x74582c2c, 0x2e341a1a,
    0x2d361b1b, 0xb2dc6e6e, 0xeeb45a5a, 0xfb5ba0a0,
    0xf6a45252, 0x4d763b3b, 0x61b7d6d6, 0xce7db3b3,
    0x7b522929, 0x3edde3e3, 0x715e2f2f, 0x97138484,
    0xf5a65353, 0x68b9d1d1, 0x00000000, 0x2cc1eded,
    0x60402020, 0x1fe3fcfc, 0xc879b1b1, 0xedb65b5b,
    0xbed46a6a, 0x468dcbcb, 0xd967bebe, 0x4b723939,
    0xde944a4a, 0xd4984c4c, 0xe8b05858, 0x4a85cfcf,
    0x6bbbd0d0, 0x2ac5efef, 0xe54faaaa, 0x16edfbfb,
    0xc5864343, 0xd79a4d4d, 0x55663333, 0x94118585,
    0xcf8a4545, 0x10e9f9f9, 0x06040202, 0x81fe7f7f,
    0xf0a05050, 0x44783c3c, 0xba259f9f, 0xe34ba8a8,
    0xf3a25151, 0xfe5da3a3, 0xc0804040, 0x8a058f8f,
    0xad3f9292, 0xbc219d9d, 0x48703838, 0x04f1f5f5,
    0xdf63bcbc, 0xc177b6b6, 0x75afdada, 0x63422121,
    0x30201010, 0x1ae5ffff, 0x0efdf3f3, 0x6dbfd2d2,
    0x4c81cdcd, 0x14180c0c, 0x35261313, 0x2fc3ecec,
    0xe1be5f5f, 0xa2359797, 0xcc884444, 0x392e1717,
    0x5793c4c4, 0xf255a7a7, 0x82fc7e7e, 0x477a3d3d,
    0xacc86464, 0xe7ba5d5d, 0x2b321919, 0x95e67373,
    0xa0c06060, 0x98198181, 0xd19e4f4f, 0x7fa3dcdc,
    0x66442222, 0x7e542a2a, 0xab3b9090, 0x830b8888,
    0xca8c4646, 0x29c7eeee, 0xd36bb8b8, 0x3c281414,
    0x79a7dede, 0xe2bc5e5e, 0x1d160b0b, 0x76addbdb,
    0x3bdbe0e0, 0x56643232, 0x4e743a3a, 0x1e140a0a,
    0xdb924949, 0x0a0c0606, 0x6c482424, 0xe4b85c5c,
    0x5d9fc2c2, 0x6ebdd3d3, 0xef43acac, 0xa6c46262,
    0xa8399191, 0xa4319595, 0x37d3e4e4, 0x8bf27979,
    0x32d5e7e7, 0x438bc8c8, 0x596e3737, 0xb7da6d6d,
    0x8c018d8d, 0x64b1d5d5, 0xd29c4e4e, 0xe049a9a9,
    0xb4d86c6c, 0xfaac5656, 0x07f3f4f4, 0x25cfeaea,
    0xafca6565, 0x8ef47a7a, 0xe947aeae, 0x18100808,
    0xd56fbaba, 0x88f07878, 0x6f4a2525, 0x725c2e2e,
    0x24381c1c, 0xf157a6a6, 0xc773b4b4, 0x5197c6c6,
    0x23cbe8e8, 0x7ca1dddd, 0x9ce87474, 0x213e1f1f,
    0xdd964b4b, 0xdc61bdbd, 0x860d8b8b, 0x850f8a8a,
    0x90e07070, 0x427c3e3e, 0xc471b5b5, 0xaacc6666,
    0xd8904848, 0x05060303, 0x01f7f6f6, 0x121c0e0e,
    0xa3c26161, 0x5f6a3535, 0xf9ae5757, 0xd069b9b9,
    0x91178686, 0x5899c1c1, 0x273a1d1d, 0xb9279e9e,
    0x38d9e1e1, 0x13ebf8f8, 0xb32b9898, 0x33221111,
    0xbbd26969, 0x70a9d9d9, 0x89078e8e, 0xa7339494,
    0xb62d9b9b, 0x223c1e1e, 0x92158787, 0x20c9e9e9,
    0x4987cece, 0xffaa5555, 0x78502828, 0x7aa5dfdf,
    0x8f038c8c, 0xf859a1a1, 0x80098989, 0x171a0d0d,
    0xda65bfbf, 0x31d7e6e6, 0xc6844242, 0xb8d06868,
    0xc3824141, 0xb0299999, 0x775a2d2d, 0x111e0f0f,
    0xcb7bb0b0, 0xfca85454, 0xd66dbbbb, 0x3a2c1616
};

const uint32_t B5Te2_S[256] = {
    0x63a5c663, 0x7c84f87c, 0x7799ee77, 0x7b8df67b,
    0xf20dfff2, 0x6bbdd66b, 0x6fb1de6f, 0xc55491c5,
    0x30506030, 0x01030201, 0x67a9ce67, 0x2b7d562b,
    0xfe19e7fe, 0xd762b5d7, 0xabe64dab, 0x769aec76,
    0xca458fca, 0x829d1f82, 0xc94089c9, 0x7d87fa7d,
    0xfa15effa, 0x59ebb259, 0x47c98e47, 0xf00bfbf0,
    0xadec41ad, 0xd467b3d4, 0xa2fd5fa2, 0xafea45af,
    0x9cbf239c, 0xa4f753a4, 0x7296e472, 0xc05b9bc0,
    0xb7c275b7, 0xfd1ce1fd, 0x93ae3d93, 0x266a4c26,
    0x365a6c36, 0x3f417e3f, 0xf702f5f7, 0xcc4f83cc,
    0x345c6834, 0xa5f451a5, 0xe534d1e5, 0xf108f9f1,
    0x7193e271, 0xd873abd8, 0x31536231, 0x153f2a15,
    0x040c0804, 0xc75295c7, 0x23654623, 0xc35e9dc3,
    0x18283018, 0x96a13796, 0x050f0a05, 0x9ab52f9a,
    0x07090e07, 0x12362412, 0x809b1b80, 0xe23ddfe2,
    0xeb26cdeb, 0x27694e27, 0xb2cd7fb2, 0x759fea75,
    0x091b1209, 0x839e1d83, 0x2c74582c, 0x1a2e341a,
    0x1b2d361b, 0x6eb2dc6e, 0x5aeeb45a, 0xa0fb5ba0,
    0x52f6a452, 0x3b4d763b, 0xd661b7d6, 0xb3ce7db3,
    0x297b5229, 0xe33edde3, 0x2f715e2f, 0x84971384,
    0x53f5a653, 0xd168b9d1, 0x00000000, 0xed2cc1ed,
    0x20604020, 0xfc1fe3fc, 0xb1c879b1, 0x5bedb65b,
    0x6abed46a, 0xcb468dcb, 0xbed967be, 0x394b7239,
    0x4ade944a, 0x4cd4984c, 0x58e8b058, 0xcf4a85cf,
    0xd06bbbd0, 0xef2ac5ef, 0xaae54faa, 0xfb16edfb,
    0x43c58643, 0x4dd79a4d, 0x33556633, 0x85941185,
    0x45cf8a45, 0xf910e9f9, 0x02060402, 0x7f81fe7f,
    0x50f0a050, 0x3c44783c, 0x9fba259f, 0xa8e34ba8,
    0x51f3a251, 0xa3fe5da3, 0x40c08040, 0x8f8a058f,
    0x92ad3f92, 0x9dbc219d, 0x38487038, 0xf504f1f5,
    0xbcdf63bc, 0xb6c177b6, 0xda75afda, 0x21634221,
    0x10302010, 0xff1ae5ff, 0xf30efdf3, 0xd26dbfd2,
    0xcd4c81cd, 0x0c14180c, 0x13352613, 0xec2fc3ec,
    0x5fe1be5f, 0x97a23597, 0x44cc8844, 0x17392e17,
    0xc45793c4, 0xa7f255a7, 0x7e82fc7e, 0x3d477a3d,
    0x64acc864, 0x5de7ba5d, 0x192b3219, 0x7395e673,
    0x60a0c060, 0x81981981, 0x4fd19e4f, 0xdc7fa3dc,
    0x22664422, 0x2a7e542a, 0x90ab3b90, 0x88830b88,
    0x46ca8c46, 0xee29c7ee, 0xb8d36bb8, 0x143c2814,
    0xde79a7de, 0x5ee2bc5e, 0x0b1d160b, 0xdb76addb,
    0xe03bdbe0, 0x32566432, 0x3a4e743a, 0x0a1e140a,
    0x49db9249, 0x060a0c06, 0x246c4824, 0x5ce4b85c,
    0xc25d9fc2, 0xd36ebdd3, 0xacef43ac, 0x62a6c462,
    0x91a83991, 0x95a43195, 0xe437d3e4, 0x798bf279,
    0xe732d5e7, 0xc8438bc8, 0x37596e37, 0x6db7da6d,
    0x8d8c018d, 0xd564b1d5, 0x4ed29c4e, 0xa9e049a9,
    0x6cb4d86c, 0x56faac56, 0xf407f3f4, 0xea25cfea,
    0x65afca65, 0x7a8ef47a, 0xaee947ae, 0x08181008,
    0xbad56fba, 0x7888f078, 0x256f4a25, 0x2e725c2e,
    0x1c24381c, 0xa6f157a6, 0xb4c773b4, 0xc65197c6,
    0xe823cbe8, 0xdd7ca1dd, 0x749ce874, 0x1f213e1f,
    0x4bdd964b, 0xbddc61bd, 0x8b860d8b, 0x8a850f8a,
    0x7090e070, 0x3e427c3e, 0xb5c471b5, 0x66aacc66,
    0x48d89048, 0x03050603, 0xf601f7f6, 0x0e121c0e,
    0x61a3c261, 0x355f6a35, 0x57f9ae57, 0xb9d069b9,
    0x86911786, 0xc15899c1, 0x1d273a1d, 0x9eb9279e,
    0xe138d9e1, 0xf813ebf8, 0x98b32b98, 0x11332211,
    0x69bbd269, 0xd970a9d9, 0x8e89078e, 0x94a73394,
    0x9bb62d9b, 0x1e223c1e, 0x87921587, 0xe920c9e9,
    0xce4987ce, 0x55ffaa55, 0x28785028, 0xdf7aa5df,
    0x8c8f038c, 0xa1f859a1, 0x89800989, 0x0d171a0d,
    0xbfda65bf, 0xe631d7e6, 0x42c68442, 0x68b8d068,
    0x41c38241, 0x99b02999, 0x2d775a2d, 0x0f111e0f,
    0xb0cb7bb0, 0x54fca854, 0xbbd66dbb, 0x163a2c16
};

const uint32_t B5Te3_S[256] = {
    0x6363a5c6, 0x7c7c84f8, 0x777799ee, 0x7b7b8df6,
    0xf2f20dff, 0x6b6bbdd6, 0x6f6fb1de, 0xc5c55491,
    0x30305060, 0x01010302, 0x6767a9ce, 0x2b2b7d56,
    0xfefe19e7, 0xd7d762b5, 0xababe64d, 0x76769aec,
    0xcaca458f, 0x82829d1f, 0xc9c94089, 0x7d7d87fa,
    0xfafa15ef, 0x5959ebb2, 0x4747c98e, 0xf0f00bfb,
    0xadadec41, 0xd4d467b3, 0xa2a2fd5f, 0xafafea45,
    0x9c9cbf23, 0xa4a4f753, 0x727296e4, 0xc0c05b9b,
    0xb7b7c275, 0xfdfd1ce1, 0x9393ae3d, 0x26266a4c,
    0x36365a6c, 0x3f3f417e, 0xf7f702f5, 0xcccc4f83,
    0x34345c68, 0xa5a5f451, 0xe5e534d1, 0xf1f108f9,
    0x717193e2, 0xd8d873ab, 0x31315362, 0x15153f2a,
    0x04040c08, 0xc7c75295, 0x23236546, 0xc3c35e9d,
    0x18182830, 0x9696a137, 0x05050f0a, 0x9a9ab52f,
    0x0707090e, 0x12123624, 0x80809b1b, 0xe2e23ddf,
    0xebeb26cd, 0x2727694e, 0xb2b2cd7f, 0x75759fea,
    0x09091b12, 0x83839e1d, 0x2c2c7458, 0x1a1a2e34,
    0x1b1b2d36, 0x6e6eb2dc, 0x5a5aeeb4, 0xa0a0fb5b,
    0x5252f6a4, 0x3b3b4d76, 0xd6d661b7, 0xb3b3ce7d,
    0x29297b52, 0xe3e33edd, 0x2f2f715e, 0x84849713,
    0x5353f5a6, 0xd1d168b9, 0x00000000, 0xeded2cc1,
    0x20206040, 0xfcfc1fe3, 0xb1b1c879, 0x5b5bedb6,
    0x6a6abed4, 0xcbcb468d, 0xbebed967, 0x39394b72,
    0x4a4ade94, 0x4c4cd498, 0x5858e8b0, 0xcfcf4a85,
    0xd0d06bbb, 0xefef2ac5, 0xaaaae54f, 0xfbfb16ed,
    0x4343c586, 0x4d4dd79a, 0x33335566, 0x85859411,
    0x4545cf8a, 0xf9f910e9, 0x02020604, 0x7f7f81fe,
    0x5050f0a0, 0x3c3c4478, 0x9f9fba25, 0xa8a8e34b,
    0x5151f3a2, 0xa3a3fe5d, 0x4040c080, 0x8f8f8a05,
    0x9292ad3f, 0x9d9dbc21, 0x38384870, 0xf5f504f1,
    0xbcbcdf63, 0xb6b6c177, 0xdada75af, 0x21216342,
    0x10103020, 0xffff1ae5, 0xf3f30efd, 0xd2d26dbf,
    0xcdcd4c81, 0x0c0c1418, 0x13133526, 0xecec2fc3,
    0x5f5fe1be, 0x9797a235, 0x4444cc88, 0x1717392e,
    0xc4c45793, 0xa7a7f255, 0x7e7e82fc, 0x3d3d477a,
    0x6464acc8, 0x5d5de7ba, 0x19192b32, 0x737395e6,
    0x6060a0c0, 0x81819819, 0x4f4fd19e, 0xdcdc7fa3,
    0x22226644, 0x2a2a7e54, 0x9090ab3b, 0x8888830b,
    0x4646ca8c, 0xeeee29c7, 0xb8b8d36b, 0x14143c28,
    0xdede79a7, 0x5e5ee2bc, 0x0b0b1d16, 0xdbdb76ad,
    0xe0e03bdb, 0x32325664, 0x3a3a4e74, 0x0a0a1e14,
    0x4949db92, 0x06060a0c, 0x24246c48, 0x5c5ce4b8,
    0xc2c25d9f, 0xd3d36ebd, 0xacacef43, 0x6262a6c4,
    0x9191a839, 0x9595a431, 0xe4e437d3, 0x79798bf2,
    0xe7e732d5, 0xc8c8438b, 0x3737596e, 0x6d6db7da,
    0x8d8d8c01, 0xd5d564b1, 0x4e4ed29c, 0xa9a9e049,
    0x6c6cb4d8, 0x5656faac, 0xf4f407f3, 0xeaea25cf,
    0x6565afca, 0x7a7a8ef4, 0xaeaee947, 0x08081810,
    0xbabad56f, 0x787888f0, 0x25256f4a, 0x2e2e725c,
    0x1c1c2438, 0xa6a6f157, 0xb4b4c773, 0xc6c65197,
    0xe8e823cb, 0xdddd7ca1, 0x74749ce8, 0x1f1f213e,
    0x4b4bdd96, 0xbdbddc61, 0x8b8b860d, 0x8a8a850f,
    0x707090e0, 0x3e3e427c, 0xb5b5c471, 0x6666aacc,
    0x4848d890, 0x03030506, 0xf6f601f7, 0x0e0e121c,
    0x6161a3c2, 0x35355f6a, 0x5757f9ae, 0xb9b9d069,
    0x86869117, 0xc1c15899, 0x1d1d273a, 0x9e9eb927,
    0xe1e138d9, 0xf8f813eb, 0x9898b32b, 0x11113322,
    0x6969bbd2, 0xd9d970a9, 0x8e8e8907, 0x9494a733,
    0x9b9bb62d, 0x1e1e223c, 0x87879215, 0xe9e920c9,
    0xcece4987, 0x5555ffaa, 0x28287850, 0xdfdf7aa5,
    0x8c8c8f03, 0xa1a1f859, 0x89898009, 0x0d0d171a,
    0xbfbfda65, 0xe6e631d7, 0x4242c684, 0x6868b8d0,
    0x4141c382, 0x9999b029, 0x2d2d775a, 0x0f0f111e,
    0xb0b0cb7b, 0x5454fca8, 0xbbbbd66d, 0x16163a2c
};

const uint32_t B5Te4_S[256] = {
    0x63636363, 0x7c7c7c7c, 0x77777777, 0x7b7b7b7b,
    0xf2f2f2f2, 0x6b6b6b6b, 0x6f6f6f6f, 0xc5c5c5c5,
    0x30303030, 0x01010101, 0x67676767, 0x2b2b2b2b,
    0xfefefefe, 0xd7d7d7d7, 0xabababab, 0x76767676,
    0xcacacaca, 0x82828282, 0xc9c9c9c9, 0x7d7d7d7d,
    0xfafafafa, 0x59595959, 0x47474747, 0xf0f0f0f0,
    0xadadadad, 0xd4d4d4d4, 0xa2a2a2a2, 0xafafafaf,
    0x9c9c9c9c, 0xa4a4a4a4, 0x72727272, 0xc0c0c0c0,
    0xb7b7b7b7, 0xfdfdfdfd, 0x93939393, 0x26262626,
    0x36363636, 0x3f3f3f3f, 0xf7f7f7f7, 0xcccccccc,
    0x34343434, 0xa5a5a5a5, 0xe5e5e5e5, 0xf1f1f1f1,
    0x71717171, 0xd8d8d8d8, 0x31313131, 0x15151515,
    0x04040404, 0xc7c7c7c7, 0x23232323, 0xc3c3c3c3,
    0x18181818, 0x96969696, 0x05050505, 0x9a9a9a9a,
    0x07070707, 0x12121212, 0x80808080, 0xe2e2e2e2,
    0xebebebeb, 0x27272727, 0xb2b2b2b2, 0x75757575,
    0x09090909, 0x83838383, 0x2c2c2c2c, 0x1a1a1a1a,
    0x1b1b1b1b, 0x6e6e6e6e, 0x5a5a5a5a, 0xa0a0a0a0,
    0x52525252, 0x3b3b3b3b, 0xd6d6d6d6, 0xb3b3b3b3,
    0x29292929, 0xe3e3e3e3, 0x2f2f2f2f, 0x84848484,
    0x53535353, 0xd1d1d1d1, 0x00000000, 0xedededed,
    0x20202020, 0xfcfcfcfc, 0xb1b1b1b1, 0x5b5b5b5b,
    0x6a6a6a6a, 0xcbcbcbcb, 0xbebebebe, 0x39393939,
    0x4a4a4a4a, 0x4c4c4c4c, 0x58585858, 0xcfcfcfcf,
    0xd0d0d0d0, 0xefefefef, 0xaaaaaaaa, 0xfbfbfbfb,
    0x43434343, 0x4d4d4d4d, 0x33333333, 0x85858585,
    0x45454545, 0xf9f9f9f9, 0x02020202, 0x7f7f7f7f,
    0x50505050, 0x3c3c3c3c, 0x9f9f9f9f, 0xa8a8a8a8,
    0x51515151, 0xa3a3a3a3, 0x40404040, 0x8f8f8f8f,
    0x92929292, 0x9d9d9d9d, 0x38383838, 0xf5f5f5f5,
    0xbcbcbcbc, 0xb6b6b6b6, 0xdadadada, 0x21212121,
    0x10101010, 0xffffffff, 0xf3f3f3f3, 0xd2d2d2d2,
    0xcdcdcdcd, 0x0c0c0c0c, 0x13131313, 0xecececec,
    0x5f5f5f5f, 0x97979797, 0x44444444, 0x17171717,
    0xc4c4c4c4, 0xa7a7a7a7, 0x7e7e7e7e, 0x3d3d3d3d,
    0x64646464, 0x5d5d5d5d, 0x19191919, 0x73737373,
    0x60606060, 0x81818181, 0x4f4f4f4f, 0xdcdcdcdc,
    0x22222222, 0x2a2a2a2a, 0x90909090, 0x88888888,
    0x46464646, 0xeeeeeeee, 0xb8b8b8b8, 0x14141414,
    0xdededede, 0x5e5e5e5e, 0x0b0b0b0b, 0xdbdbdbdb,
    0xe0e0e0e0, 0x32323232, 0x3a3a3a3a, 0x0a0a0a0a,
    0x49494949, 0x06060606, 0x24242424, 0x5c5c5c5c,
    0xc2c2c2c2, 0xd3d3d3d3, 0xacacacac, 0x62626262,
    0x91919191, 0x95959595, 0xe4e4e4e4, 0x79797979,
    0xe7e7e7e7, 0xc8c8c8c8, 0x37373737, 0x6d6d6d6d,
    0x8d8d8d8d, 0xd5d5d5d5, 0x4e4e4e4e, 0xa9a9a9a9,
    0x6c6c6c6c, 0x56565656, 0xf4f4f4f4, 0xeaeaeaea,
    0x65656565, 0x7a7a7a7a, 0xaeaeaeae, 0x08080808,
    0xbabababa, 0x78787878, 0x25252525, 0x2e2e2e2e,
    0x1c1c1c1c, 0xa6a6a6a6, 0xb4b4b4b4, 0xc6c6c6c6,
    0xe8e8e8e8, 0xdddddddd, 0x74747474, 0x1f1f1f1f,
    0x4b4b4b4b, 0xbdbdbdbd, 0x8b8b8b8b, 0x8a8a8a8a,
    0x70707070, 0x3e3e3e3e, 0xb5b5b5b5, 0x66666666,
    0x48484848, 0x03030303, 0xf6f6f6f6, 0x0e0e0e0e,
    0x61616161, 0x35353535, 0x57575757, 0xb9b9b9b9,
    0x86868686, 0xc1c1c1c1, 0x1d1d1d1d, 0x9e9e9e9e,
    0xe1e1e1e1, 0xf8f8f8f8, 0x98989898, 0x11111111,
    0x69696969, 0xd9d9d9d9, 0x8e8e8e8e, 0x94949494,
    0x9b9b9b9b, 0x1e1e1e1e, 0x87878787, 0xe9e9e9e9,
    0xcececece, 0x55555555, 0x28282828, 0xdfdfdfdf,
    0x8c8c8c8c, 0xa1a1a1a1, 0x89898989, 0x0d0d0d0d,
    0xbfbfbfbf, 0xe6e6e6e6, 0x42424242, 0x68686868,
    0x41414141, 0x99999999, 0x2d2d2d2d, 0x0f0f0f0f,
    0xb0b0b0b0, 0x54545454, 0xbbbbbbbb, 0x16161616
};

const uint32_t B5Td0_S[256] = {
    0x51f4a750U, 0x7e416553U, 0x1a17a4c3U, 0x3a275e96U,
    0x3bab6bcbU, 0x1f9d45f1U, 0xacfa58abU, 0x4be30393U,
    0x2030fa55U, 0xad766df6U, 0x88cc7691U, 0xf5024c25U,
    0x4fe5d7fcU, 0xc52acbd7U, 0x26354480U, 0xb562a38fU,
    0xdeb15a49U, 0x25ba1b67U, 0x45ea0e98U, 0x5dfec0e1U,
    0xc32f7502U, 0x814cf012U, 0x8d4697a3U, 0x6bd3f9c6U,
    0x038f5fe7U, 0x15929c95U, 0xbf6d7aebU, 0x955259daU,
    0xd4be832dU, 0x587421d3U, 0x49e06929U, 0x8ec9c844U,
    0x75c2896aU, 0xf48e7978U, 0x99583e6bU, 0x27b971ddU,
    0xbee14fb6U, 0xf088ad17U, 0xc920ac66U, 0x7dce3ab4U,
    0x63df4a18U, 0xe51a3182U, 0x97513360U, 0x62537f45U,
    0xb16477e0U, 0xbb6bae84U, 0xfe81a01cU, 0xf9082b94U,
    0x70486858U, 0x8f45fd19U, 0x94de6c87U, 0x527bf8b7U,
    0xab73d323U, 0x724b02e2U, 0xe31f8f57U, 0x6655ab2aU,
    0xb2eb2807U, 0x2fb5c203U, 0x86c57b9aU, 0xd33708a5U,
    0x302887f2U, 0x23bfa5b2U, 0x02036abaU, 0xed16825cU,
    0x8acf1c2bU, 0xa779b492U, 0xf307f2f0U, 0x4e69e2a1U,
    0x65daf4cdU, 0x0605bed5U, 0xd134621fU, 0xc4a6fe8aU,
    0x342e539dU, 0xa2f355a0U, 0x058ae132U, 0xa4f6eb75U,
    0x0b83ec39U, 0x4060efaaU, 0x5e719f06U, 0xbd6e1051U,
    0x3e218af9U, 0x96dd063dU, 0xdd3e05aeU, 0x4de6bd46U,
    0x91548db5U, 0x71c45d05U, 0x0406d46fU, 0x605015ffU,
    0x1998fb24U, 0xd6bde997U, 0x894043ccU, 0x67d99e77U,
    0xb0e842bdU, 0x07898b88U, 0xe7195b38U, 0x79c8eedbU,
    0xa17c0a47U, 0x7c420fe9U, 0xf8841ec9U, 0x00000000U,
    0x09808683U, 0x322bed48U, 0x1e1170acU, 0x6c5a724eU,
    0xfd0efffbU, 0x0f853856U, 0x3daed51eU, 0x362d3927U,
    0x0a0fd964U, 0x685ca621U, 0x9b5b54d1U, 0x24362e3aU,
    0x0c0a67b1U, 0x9357e70fU, 0xb4ee96d2U, 0x1b9b919eU,
    0x80c0c54fU, 0x61dc20a2U, 0x5a774b69U, 0x1c121a16U,
    0xe293ba0aU, 0xc0a02ae5U, 0x3c22e043U, 0x121b171dU,
    0x0e090d0bU, 0xf28bc7adU, 0x2db6a8b9U, 0x141ea9c8U,
    0x57f11985U, 0xaf75074cU, 0xee99ddbbU, 0xa37f60fdU,
    0xf701269fU, 0x5c72f5bcU, 0x44663bc5U, 0x5bfb7e34U,
    0x8b432976U, 0xcb23c6dcU, 0xb6edfc68U, 0xb8e4f163U,
    0xd731dccaU, 0x42638510U, 0x13972240U, 0x84c61120U,
    0x854a247dU, 0xd2bb3df8U, 0xaef93211U, 0xc729a16dU,
    0x1d9e2f4bU, 0xdcb230f3U, 0x0d8652ecU, 0x77c1e3d0U,
    0x2bb3166cU, 0xa970b999U, 0x119448faU, 0x47e96422U,
    0xa8fc8cc4U, 0xa0f03f1aU, 0x567d2cd8U, 0x223390efU,
    0x87494ec7U, 0xd938d1c1U, 0x8ccaa2feU, 0x98d40b36U,
    0xa6f581cfU, 0xa57ade28U, 0xdab78e26U, 0x3fadbfa4U,
    0x2c3a9de4U, 0x5078920dU, 0x6a5fcc9bU, 0x547e4662U,
    0xf68d13c2U, 0x90d8b8e8U, 0x2e39f75eU, 0x82c3aff5U,
    0x9f5d80beU, 0x69d0937cU, 0x6fd52da9U, 0xcf2512b3U,
    0xc8ac993bU, 0x10187da7U, 0xe89c636eU, 0xdb3bbb7bU,
    0xcd267809U, 0x6e5918f4U, 0xec9ab701U, 0x834f9aa8U,
    0xe6956e65U, 0xaaffe67eU, 0x21bccf08U, 0xef15e8e6U,
    0xbae79bd9U, 0x4a6f36ceU, 0xea9f09d4U, 0x29b07cd6U,
    0x31a4b2afU, 0x2a3f2331U, 0xc6a59430U, 0x35a266c0U,
    0x744ebc37U, 0xfc82caa6U, 0xe090d0b0U, 0x33a7d815U,
    0xf104984aU, 0x41ecdaf7U, 0x7fcd500eU, 0x1791f62fU,
    0x764dd68dU, 0x43efb04dU, 0xccaa4d54U, 0xe49604dfU,
    0x9ed1b5e3U, 0x4c6a881bU, 0xc12c1fb8U, 0x4665517fU,
    0x9d5eea04U, 0x018c355dU, 0xfa877473U, 0xfb0b412eU,
    0xb3671d5aU, 0x92dbd252U, 0xe9105633U, 0x6dd64713U,
    0x9ad7618cU, 0x37a10c7aU, 0x59f8148eU, 0xeb133c89U,
    0xcea927eeU, 0xb761c935U, 0xe11ce5edU, 0x7a47b13cU,
    0x9cd2df59U, 0x55f2733fU, 0x1814ce79U, 0x73c737bfU,
    0x53f7cdeaU, 0x5ffdaa5bU, 0xdf3d6f14U, 0x7844db86U,
    0xcaaff381U, 0xb968c43eU, 0x3824342cU, 0xc2a3405fU,
    0x161dc372U, 0xbce2250cU, 0x283c498bU, 0xff0d9541U,
    0x39a80171U, 0x080cb3deU, 0xd8b4e49cU, 0x6456c190U,
    0x7bcb8461U, 0xd532b670U, 0x486c5c74U, 0xd0b85742U,
};

const uint32_t B5Td1_S[256] = {
    0x5051f4a7U, 0x537e4165U, 0xc31a17a4U, 0x963a275eU,
    0xcb3bab6bU, 0xf11f9d45U, 0xabacfa58U, 0x934be303U,
    0x552030faU, 0xf6ad766dU, 0x9188cc76U, 0x25f5024cU,
    0xfc4fe5d7U, 0xd7c52acbU, 0x80263544U, 0x8fb562a3U,
    0x49deb15aU, 0x6725ba1bU, 0x9845ea0eU, 0xe15dfec0U,
    0x02c32f75U, 0x12814cf0U, 0xa38d4697U, 0xc66bd3f9U,
    0xe7038f5fU, 0x9515929cU, 0xebbf6d7aU, 0xda955259U,
    0x2dd4be83U, 0xd3587421U, 0x2949e069U, 0x448ec9c8U,
    0x6a75c289U, 0x78f48e79U, 0x6b99583eU, 0xdd27b971U,
    0xb6bee14fU, 0x17f088adU, 0x66c920acU, 0xb47dce3aU,
    0x1863df4aU, 0x82e51a31U, 0x60975133U, 0x4562537fU,
    0xe0b16477U, 0x84bb6baeU, 0x1cfe81a0U, 0x94f9082bU,
    0x58704868U, 0x198f45fdU, 0x8794de6cU, 0xb7527bf8U,
    0x23ab73d3U, 0xe2724b02U, 0x57e31f8fU, 0x2a6655abU,
    0x07b2eb28U, 0x032fb5c2U, 0x9a86c57bU, 0xa5d33708U,
    0xf2302887U, 0xb223bfa5U, 0xba02036aU, 0x5ced1682U,
    0x2b8acf1cU, 0x92a779b4U, 0xf0f307f2U, 0xa14e69e2U,
    0xcd65daf4U, 0xd50605beU, 0x1fd13462U, 0x8ac4a6feU,
    0x9d342e53U, 0xa0a2f355U, 0x32058ae1U, 0x75a4f6ebU,
    0x390b83ecU, 0xaa4060efU, 0x065e719fU, 0x51bd6e10U,
    0xf93e218aU, 0x3d96dd06U, 0xaedd3e05U, 0x464de6bdU,
    0xb591548dU, 0x0571c45dU, 0x6f0406d4U, 0xff605015U,
    0x241998fbU, 0x97d6bde9U, 0xcc894043U, 0x7767d99eU,
    0xbdb0e842U, 0x8807898bU, 0x38e7195bU, 0xdb79c8eeU,
    0x47a17c0aU, 0xe97c420fU, 0xc9f8841eU, 0x00000000U,
    0x83098086U, 0x48322bedU, 0xac1e1170U, 0x4e6c5a72U,
    0xfbfd0effU, 0x560f8538U, 0x1e3daed5U, 0x27362d39U,
    0x640a0fd9U, 0x21685ca6U, 0xd19b5b54U, 0x3a24362eU,
    0xb10c0a67U, 0x0f9357e7U, 0xd2b4ee96U, 0x9e1b9b91U,
    0x4f80c0c5U, 0xa261dc20U, 0x695a774bU, 0x161c121aU,
    0x0ae293baU, 0xe5c0a02aU, 0x433c22e0U, 0x1d121b17U,
    0x0b0e090dU, 0xadf28bc7U, 0xb92db6a8U, 0xc8141ea9U,
    0x8557f119U, 0x4caf7507U, 0xbbee99ddU, 0xfda37f60U,
    0x9ff70126U, 0xbc5c72f5U, 0xc544663bU, 0x345bfb7eU,
    0x768b4329U, 0xdccb23c6U, 0x68b6edfcU, 0x63b8e4f1U,
    0xcad731dcU, 0x10426385U, 0x40139722U, 0x2084c611U,
    0x7d854a24U, 0xf8d2bb3dU, 0x11aef932U, 0x6dc729a1U,
    0x4b1d9e2fU, 0xf3dcb230U, 0xec0d8652U, 0xd077c1e3U,
    0x6c2bb316U, 0x99a970b9U, 0xfa119448U, 0x2247e964U,
    0xc4a8fc8cU, 0x1aa0f03fU, 0xd8567d2cU, 0xef223390U,
    0xc787494eU, 0xc1d938d1U, 0xfe8ccaa2U, 0x3698d40bU,
    0xcfa6f581U, 0x28a57adeU, 0x26dab78eU, 0xa43fadbfU,
    0xe42c3a9dU, 0x0d507892U, 0x9b6a5fccU, 0x62547e46U,
    0xc2f68d13U, 0xe890d8b8U, 0x5e2e39f7U, 0xf582c3afU,
    0xbe9f5d80U, 0x7c69d093U, 0xa96fd52dU, 0xb3cf2512U,
    0x3bc8ac99U, 0xa710187dU, 0x6ee89c63U, 0x7bdb3bbbU,
    0x09cd2678U, 0xf46e5918U, 0x01ec9ab7U, 0xa8834f9aU,
    0x65e6956eU, 0x7eaaffe6U, 0x0821bccfU, 0xe6ef15e8U,
    0xd9bae79bU, 0xce4a6f36U, 0xd4ea9f09U, 0xd629b07cU,
    0xaf31a4b2U, 0x312a3f23U, 0x30c6a594U, 0xc035a266U,
    0x37744ebcU, 0xa6fc82caU, 0xb0e090d0U, 0x1533a7d8U,
    0x4af10498U, 0xf741ecdaU, 0x0e7fcd50U, 0x2f1791f6U,
    0x8d764dd6U, 0x4d43efb0U, 0x54ccaa4dU, 0xdfe49604U,
    0xe39ed1b5U, 0x1b4c6a88U, 0xb8c12c1fU, 0x7f466551U,
    0x049d5eeaU, 0x5d018c35U, 0x73fa8774U, 0x2efb0b41U,
    0x5ab3671dU, 0x5292dbd2U, 0x33e91056U, 0x136dd647U,
    0x8c9ad761U, 0x7a37a10cU, 0x8e59f814U, 0x89eb133cU,
    0xeecea927U, 0x35b761c9U, 0xede11ce5U, 0x3c7a47b1U,
    0x599cd2dfU, 0x3f55f273U, 0x791814ceU, 0xbf73c737U,
    0xea53f7cdU, 0x5b5ffdaaU, 0x14df3d6fU, 0x867844dbU,
    0x81caaff3U, 0x3eb968c4U, 0x2c382434U, 0x5fc2a340U,
    0x72161dc3U, 0x0cbce225U, 0x8b283c49U, 0x41ff0d95U,
    0x7139a801U, 0xde080cb3U, 0x9cd8b4e4U, 0x906456c1U,
    0x617bcb84U, 0x70d532b6U, 0x74486c5cU, 0x42d0b857U,
};

const uint32_t B5Td2_S[256] = {
    0xa75051f4U, 0x65537e41U, 0xa4c31a17U, 0x5e963a27U,
    0x6bcb3babU, 0x45f11f9dU, 0x58abacfaU, 0x03934be3U,
    0xfa552030U, 0x6df6ad76U, 0x769188ccU, 0x4c25f502U,
    0xd7fc4fe5U, 0xcbd7c52aU, 0x44802635U, 0xa38fb562U,
    0x5a49deb1U, 0x1b6725baU, 0x0e9845eaU, 0xc0e15dfeU,
    0x7502c32fU, 0xf012814cU, 0x97a38d46U, 0xf9c66bd3U,
    0x5fe7038fU, 0x9c951592U, 0x7aebbf6dU, 0x59da9552U,
    0x832dd4beU, 0x21d35874U, 0x692949e0U, 0xc8448ec9U,
    0x896a75c2U, 0x7978f48eU, 0x3e6b9958U, 0x71dd27b9U,
    0x4fb6bee1U, 0xad17f088U, 0xac66c920U, 0x3ab47dceU,
    0x4a1863dfU, 0x3182e51aU, 0x33609751U, 0x7f456253U,
    0x77e0b164U, 0xae84bb6bU, 0xa01cfe81U, 0x2b94f908U,
    0x68587048U, 0xfd198f45U, 0x6c8794deU, 0xf8b7527bU,
    0xd323ab73U, 0x02e2724bU, 0x8f57e31fU, 0xab2a6655U,
    0x2807b2ebU, 0xc2032fb5U, 0x7b9a86c5U, 0x08a5d337U,
    0x87f23028U, 0xa5b223bfU, 0x6aba0203U, 0x825ced16U,
    0x1c2b8acfU, 0xb492a779U, 0xf2f0f307U, 0xe2a14e69U,
    0xf4cd65daU, 0xbed50605U, 0x621fd134U, 0xfe8ac4a6U,
    0x539d342eU, 0x55a0a2f3U, 0xe132058aU, 0xeb75a4f6U,
    0xec390b83U, 0xefaa4060U, 0x9f065e71U, 0x1051bd6eU,
    0x8af93e21U, 0x063d96ddU, 0x05aedd3eU, 0xbd464de6U,
    0x8db59154U, 0x5d0571c4U, 0xd46f0406U, 0x15ff6050U,
    0xfb241998U, 0xe997d6bdU, 0x43cc8940U, 0x9e7767d9U,
    0x42bdb0e8U, 0x8b880789U, 0x5b38e719U, 0xeedb79c8U,
    0x0a47a17cU, 0x0fe97c42U, 0x1ec9f884U, 0x00000000U,
    0x86830980U, 0xed48322bU, 0x70ac1e11U, 0x724e6c5aU,
    0xfffbfd0eU, 0x38560f85U, 0xd51e3daeU, 0x3927362dU,
    0xd9640a0fU, 0xa621685cU, 0x54d19b5bU, 0x2e3a2436U,
    0x67b10c0aU, 0xe70f9357U, 0x96d2b4eeU, 0x919e1b9bU,
    0xc54f80c0U, 0x20a261dcU, 0x4b695a77U, 0x1a161c12U,
    0xba0ae293U, 0x2ae5c0a0U, 0xe0433c22U, 0x171d121bU,
    0x0d0b0e09U, 0xc7adf28bU, 0xa8b92db6U, 0xa9c8141eU,
    0x198557f1U, 0x074caf75U, 0xddbbee99U, 0x60fda37fU,
    0x269ff701U, 0xf5bc5c72U, 0x3bc54466U, 0x7e345bfbU,
    0x29768b43U, 0xc6dccb23U, 0xfc68b6edU, 0xf163b8e4U,
    0xdccad731U, 0x85104263U, 0x22401397U, 0x112084c6U,
    0x247d854aU, 0x3df8d2bbU, 0x3211aef9U, 0xa16dc729U,
    0x2f4b1d9eU, 0x30f3dcb2U, 0x52ec0d86U, 0xe3d077c1U,
    0x166c2bb3U, 0xb999a970U, 0x48fa1194U, 0x642247e9U,
    0x8cc4a8fcU, 0x3f1aa0f0U, 0x2cd8567dU, 0x90ef2233U,
    0x4ec78749U, 0xd1c1d938U, 0xa2fe8ccaU, 0x0b3698d4U,
    0x81cfa6f5U, 0xde28a57aU, 0x8e26dab7U, 0xbfa43fadU,
    0x9de42c3aU, 0x920d5078U, 0xcc9b6a5fU, 0x4662547eU,
    0x13c2f68dU, 0xb8e890d8U, 0xf75e2e39U, 0xaff582c3U,
    0x80be9f5dU, 0x937c69d0U, 0x2da96fd5U, 0x12b3cf25U,
    0x993bc8acU, 0x7da71018U, 0x636ee89cU, 0xbb7bdb3bU,
    0x7809cd26U, 0x18f46e59U, 0xb701ec9aU, 0x9aa8834fU,
    0x6e65e695U, 0xe67eaaffU, 0xcf0821bcU, 0xe8e6ef15U,
    0x9bd9bae7U, 0x36ce4a6fU, 0x09d4ea9fU, 0x7cd629b0U,
    0xb2af31a4U, 0x23312a3fU, 0x9430c6a5U, 0x66c035a2U,
    0xbc37744eU, 0xcaa6fc82U, 0xd0b0e090U, 0xd81533a7U,
    0x984af104U, 0xdaf741ecU, 0x500e7fcdU, 0xf62f1791U,
    0xd68d764dU, 0xb04d43efU, 0x4d54ccaaU, 0x04dfe496U,
    0xb5e39ed1U, 0x881b4c6aU, 0x1fb8c12cU, 0x517f4665U,
    0xea049d5eU, 0x355d018cU, 0x7473fa87U, 0x412efb0bU,
    0x1d5ab367U, 0xd25292dbU, 0x5633e910U, 0x47136dd6U,
    0x618c9ad7U, 0x0c7a37a1U, 0x148e59f8U, 0x3c89eb13U,
    0x27eecea9U, 0xc935b761U, 0xe5ede11cU, 0xb13c7a47U,
    0xdf599cd2U, 0x733f55f2U, 0xce791814U, 0x37bf73c7U,
    0xcdea53f7U, 0xaa5b5ffdU, 0x6f14df3dU, 0xdb867844U,
    0xf381caafU, 0xc43eb968U, 0x342c3824U, 0x405fc2a3U,
    0xc372161dU, 0x250cbce2U, 0x498b283cU, 0x9541ff0dU,
    0x017139a8U, 0xb3de080cU, 0xe49cd8b4U, 0xc1906456U,
    0x84617bcbU, 0xb670d532U, 0x5c74486cU, 0x5742d0b8U,
};

const uint32_t B5Td3_S[256] = {
    0xf4a75051U, 0x4165537eU, 0x17a4c31aU, 0x275e963aU,
    0xab6bcb3bU, 0x9d45f11fU, 0xfa58abacU, 0xe303934bU,
    0x30fa5520U, 0x766df6adU, 0xcc769188U, 0x024c25f5U,
    0xe5d7fc4fU, 0x2acbd7c5U, 0x35448026U, 0x62a38fb5U,
    0xb15a49deU, 0xba1b6725U, 0xea0e9845U, 0xfec0e15dU,
    0x2f7502c3U, 0x4cf01281U, 0x4697a38dU, 0xd3f9c66bU,
    0x8f5fe703U, 0x929c9515U, 0x6d7aebbfU, 0x5259da95U,
    0xbe832dd4U, 0x7421d358U, 0xe0692949U, 0xc9c8448eU,
    0xc2896a75U, 0x8e7978f4U, 0x583e6b99U, 0xb971dd27U,
    0xe14fb6beU, 0x88ad17f0U, 0x20ac66c9U, 0xce3ab47dU,
    0xdf4a1863U, 0x1a3182e5U, 0x51336097U, 0x537f4562U,
    0x6477e0b1U, 0x6bae84bbU, 0x81a01cfeU, 0x082b94f9U,
    0x48685870U, 0x45fd198fU, 0xde6c8794U, 0x7bf8b752U,
    0x73d323abU, 0x4b02e272U, 0x1f8f57e3U, 0x55ab2a66U,
    0xeb2807b2U, 0xb5c2032fU, 0xc57b9a86U, 0x3708a5d3U,
    0x2887f230U, 0xbfa5b223U, 0x036aba02U, 0x16825cedU,
    0xcf1c2b8aU, 0x79b492a7U, 0x07f2f0f3U, 0x69e2a14eU,
    0xdaf4cd65U, 0x05bed506U, 0x34621fd1U, 0xa6fe8ac4U,
    0x2e539d34U, 0xf355a0a2U, 0x8ae13205U, 0xf6eb75a4U,
    0x83ec390bU, 0x60efaa40U, 0x719f065eU, 0x6e1051bdU,
    0x218af93eU, 0xdd063d96U, 0x3e05aeddU, 0xe6bd464dU,
    0x548db591U, 0xc45d0571U, 0x06d46f04U, 0x5015ff60U,
    0x98fb2419U, 0xbde997d6U, 0x4043cc89U, 0xd99e7767U,
    0xe842bdb0U, 0x898b8807U, 0x195b38e7U, 0xc8eedb79U,
    0x7c0a47a1U, 0x420fe97cU, 0x841ec9f8U, 0x00000000U,
    0x80868309U, 0x2bed4832U, 0x1170ac1eU, 0x5a724e6cU,
    0x0efffbfdU, 0x8538560fU, 0xaed51e3dU, 0x2d392736U,
    0x0fd9640aU, 0x5ca62168U, 0x5b54d19bU, 0x362e3a24U,
    0x0a67b10cU, 0x57e70f93U, 0xee96d2b4U, 0x9b919e1bU,
    0xc0c54f80U, 0xdc20a261U, 0x774b695aU, 0x121a161cU,
    0x93ba0ae2U, 0xa02ae5c0U, 0x22e0433cU, 0x1b171d12U,
    0x090d0b0eU, 0x8bc7adf2U, 0xb6a8b92dU, 0x1ea9c814U,
    0xf1198557U, 0x75074cafU, 0x99ddbbeeU, 0x7f60fda3U,
    0x01269ff7U, 0x72f5bc5cU, 0x663bc544U, 0xfb7e345bU,
    0x4329768bU, 0x23c6dccbU, 0xedfc68b6U, 0xe4f163b8U,
    0x31dccad7U, 0x63851042U, 0x97224013U, 0xc6112084U,
    0x4a247d85U, 0xbb3df8d2U, 0xf93211aeU, 0x29a16dc7U,
    0x9e2f4b1dU, 0xb230f3dcU, 0x8652ec0dU, 0xc1e3d077U,
    0xb3166c2bU, 0x70b999a9U, 0x9448fa11U, 0xe9642247U,
    0xfc8cc4a8U, 0xf03f1aa0U, 0x7d2cd856U, 0x3390ef22U,
    0x494ec787U, 0x38d1c1d9U, 0xcaa2fe8cU, 0xd40b3698U,
    0xf581cfa6U, 0x7ade28a5U, 0xb78e26daU, 0xadbfa43fU,
    0x3a9de42cU, 0x78920d50U, 0x5fcc9b6aU, 0x7e466254U,
    0x8d13c2f6U, 0xd8b8e890U, 0x39f75e2eU, 0xc3aff582U,
    0x5d80be9fU, 0xd0937c69U, 0xd52da96fU, 0x2512b3cfU,
    0xac993bc8U, 0x187da710U, 0x9c636ee8U, 0x3bbb7bdbU,
    0x267809cdU, 0x5918f46eU, 0x9ab701ecU, 0x4f9aa883U,
    0x956e65e6U, 0xffe67eaaU, 0xbccf0821U, 0x15e8e6efU,
    0xe79bd9baU, 0x6f36ce4aU, 0x9f09d4eaU, 0xb07cd629U,
    0xa4b2af31U, 0x3f23312aU, 0xa59430c6U, 0xa266c035U,
    0x4ebc3774U, 0x82caa6fcU, 0x90d0b0e0U, 0xa7d81533U,
    0x04984af1U, 0xecdaf741U, 0xcd500e7fU, 0x91f62f17U,
    0x4dd68d76U, 0xefb04d43U, 0xaa4d54ccU, 0x9604dfe4U,
    0xd1b5e39eU, 0x6a881b4cU, 0x2c1fb8c1U, 0x65517f46U,
    0x5eea049dU, 0x8c355d01U, 0x877473faU, 0x0b412efbU,
    0x671d5ab3U, 0xdbd25292U, 0x105633e9U, 0xd647136dU,
    0xd7618c9aU, 0xa10c7a37U, 0xf8148e59U, 0x133c89ebU,
    0xa927eeceU, 0x61c935b7U, 0x1ce5ede1U, 0x47b13c7aU,
    0xd2df599cU, 0xf2733f55U, 0x14ce7918U, 0xc737bf73U,
    0xf7cdea53U, 0xfdaa5b5fU, 0x3d6f14dfU, 0x44db8678U,
    0xaff381caU, 0x68c43eb9U, 0x24342c38U, 0xa3405fc2U,
    0x1dc37216U, 0xe2250cbcU, 0x3c498b28U, 0x0d9541ffU,
    0xa8017139U, 0x0cb3de08U, 0xb4e49cd8U, 0x56c19064U,
    0xcb84617bU, 0x32b670d5U, 0x6c5c7448U, 0xb85742d0U,
};

const uint32_t B5Td4_S[256] = {
    0x52525252U, 0x09090909U, 0x6a6a6a6aU, 0xd5d5d5d5U,
    0x30303030U, 0x36363636U, 0xa5a5a5a5U, 0x38383838U,
    0xbfbfbfbfU, 0x40404040U, 0xa3a3a3a3U, 0x9e9e9e9eU,
    0x81818181U, 0xf3f3f3f3U, 0xd7d7d7d7U, 0xfbfbfbfbU,
    0x7c7c7c7cU, 0xe3e3e3e3U, 0x39393939U, 0x82828282U,
    0x9b9b9b9bU, 0x2f2f2f2fU, 0xffffffffU, 0x87878787U,
    0x34343434U, 0x8e8e8e8eU, 0x43434343U, 0x44444444U,
    0xc4c4c4c4U, 0xdedededeU, 0xe9e9e9e9U, 0xcbcbcbcbU,
    0x54545454U, 0x7b7b7b7bU, 0x94949494U, 0x32323232U,
    0xa6a6a6a6U, 0xc2c2c2c2U, 0x23232323U, 0x3d3d3d3dU,
    0xeeeeeeeeU, 0x4c4c4c4cU, 0x95959595U, 0x0b0b0b0bU,
    0x42424242U, 0xfafafafaU, 0xc3c3c3c3U, 0x4e4e4e4eU,
    0x08080808U, 0x2e2e2e2eU, 0xa1a1a1a1U, 0x66666666U,
    0x28282828U, 0xd9d9d9d9U, 0x24242424U, 0xb2b2b2b2U,
    0x76767676U, 0x5b5b5b5bU, 0xa2a2a2a2U, 0x49494949U,
    0x6d6d6d6dU, 0x8b8b8b8bU, 0xd1d1d1d1U, 0x25252525U,
    0x72727272U, 0xf8f8f8f8U, 0xf6f6f6f6U, 0x64646464U,
    0x86868686U, 0x68686868U, 0x98989898U, 0x16161616U,
    0xd4d4d4d4U, 0xa4a4a4a4U, 0x5c5c5c5cU, 0xccccccccU,
    0x5d5d5d5dU, 0x65656565U, 0xb6b6b6b6U, 0x92929292U,
    0x6c6c6c6cU, 0x70707070U, 0x48484848U, 0x50505050U,
    0xfdfdfdfdU, 0xededededU, 0xb9b9b9b9U, 0xdadadadaU,
    0x5e5e5e5eU, 0x15151515U, 0x46464646U, 0x57575757U,
    0xa7a7a7a7U, 0x8d8d8d8dU, 0x9d9d9d9dU, 0x84848484U,
    0x90909090U, 0xd8d8d8d8U, 0xababababU, 0x00000000U,
    0x8c8c8c8cU, 0xbcbcbcbcU, 0xd3d3d3d3U, 0x0a0a0a0aU,
    0xf7f7f7f7U, 0xe4e4e4e4U, 0x58585858U, 0x05050505U,
    0xb8b8b8b8U, 0xb3b3b3b3U, 0x45454545U, 0x06060606U,
    0xd0d0d0d0U, 0x2c2c2c2cU, 0x1e1e1e1eU, 0x8f8f8f8fU,
    0xcacacacaU, 0x3f3f3f3fU, 0x0f0f0f0fU, 0x02020202U,
    0xc1c1c1c1U, 0xafafafafU, 0xbdbdbdbdU, 0x03030303U,
    0x01010101U, 0x13131313U, 0x8a8a8a8aU, 0x6b6b6b6bU,
    0x3a3a3a3aU, 0x91919191U, 0x11111111U, 0x41414141U,
    0x4f4f4f4fU, 0x67676767U, 0xdcdcdcdcU, 0xeaeaeaeaU,
    0x97979797U, 0xf2f2f2f2U, 0xcfcfcfcfU, 0xcecececeU,
    0xf0f0f0f0U, 0xb4b4b4b4U, 0xe6e6e6e6U, 0x73737373U,
    0x96969696U, 0xacacacacU, 0x74747474U, 0x22222222U,
    0xe7e7e7e7U, 0xadadadadU, 0x35353535U, 0x85858585U,
    0xe2e2e2e2U, 0xf9f9f9f9U, 0x37373737U, 0xe8e8e8e8U,
    0x1c1c1c1cU, 0x75757575U, 0xdfdfdfdfU, 0x6e6e6e6eU,
    0x47474747U, 0xf1f1f1f1U, 0x1a1a1a1aU, 0x71717171U,
    0x1d1d1d1dU, 0x29292929U, 0xc5c5c5c5U, 0x89898989U,
    0x6f6f6f6fU, 0xb7b7b7b7U, 0x62626262U, 0x0e0e0e0eU,
    0xaaaaaaaaU, 0x18181818U, 0xbebebebeU, 0x1b1b1b1bU,
    0xfcfcfcfcU, 0x56565656U, 0x3e3e3e3eU, 0x4b4b4b4bU,
    0xc6c6c6c6U, 0xd2d2d2d2U, 0x79797979U, 0x20202020U,
    0x9a9a9a9aU, 0xdbdbdbdbU, 0xc0c0c0c0U, 0xfefefefeU,
    0x78787878U, 0xcdcdcdcdU, 0x5a5a5a5aU, 0xf4f4f4f4U,
    0x1f1f1f1fU, 0xddddddddU, 0xa8a8a8a8U, 0x33333333U,
    0x88888888U, 0x07070707U, 0xc7c7c7c7U, 0x31313131U,
    0xb1b1b1b1U, 0x12121212U, 0x10101010U, 0x59595959U,
    0x27272727U, 0x80808080U, 0xececececU, 0x5f5f5f5fU,
    0x60606060U, 0x51515151U, 0x7f7f7f7fU, 0xa9a9a9a9U,
    0x19191919U, 0xb5b5b5b5U, 0x4a4a4a4aU, 0x0d0d0d0dU,
    0x2d2d2d2dU, 0xe5e5e5e5U, 0x7a7a7a7aU, 0x9f9f9f9fU,
    0x93939393U, 0xc9c9c9c9U, 0x9c9c9c9cU, 0xefefefefU,
    0xa0a0a0a0U, 0xe0e0e0e0U, 0x3b3b3b3bU, 0x4d4d4d4dU,
    0xaeaeaeaeU, 0x2a2a2a2aU, 0xf5f5f5f5U, 0xb0b0b0b0U,
    0xc8c8c8c8U, 0xebebebebU, 0xbbbbbbbbU, 0x3c3c3c3cU,
    0x83838383U, 0x53535353U, 0x99999999U, 0x61616161U,
    0x17171717U, 0x2b2b2b2bU, 0x04040404U, 0x7e7e7e7eU,
    0xbabababaU, 0x77777777U, 0xd6d6d6d6U, 0x26262626U,
    0xe1e1e1e1U, 0x69696969U, 0x14141414U, 0x63636363U,
    0x55555555U, 0x21212121U, 0x0c0c0c0cU, 0x7d7d7d7dU,
}; 

const uint32_t B5_rcon[] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1B000000, 0x36000000
    /* for 128-bit blocks, Rijndael never uses more than 10 rcon values */
};

/**
 * @brief Convert a 4 bytes array into a 32 bit word.
 * @param pt Byte array to be converted.
 * @return 32-bit word extracted from input array.
 */
uint32_t B5_AES256_GETUINT32 (const uint8_t *pt)
{
    uint16_t ph,pl;
    uint32_t p;
    
    ph = ((uint16_t) pt[0]) << 8;
    ph ^= ((uint16_t) pt[1]) & 0x00ff;
    pl = ((uint16_t) pt[2]) << 8;
    pl ^= ((uint16_t) pt[3]) & 0x00ff;
    p = (((uint32_t) ph) << 16) ^ pl;
    return p;
}

/**
 * @brief Convert a 32 bit word into a 4 bytes array.
 * @param ct Pointer to converted array.
 * @param st 32-bit word to be converted.
 */
void B5_AES256_PUTUINT32 (uint8_t *ct, const uint32_t st)
{
    ct[0] = (uint8_t)((st) >> 24);
    ct[1] = (uint8_t)((st) >> 16);
    ct[2] = (uint8_t)((st) >>  8);
    ct[3] = (uint8_t)(st);
}

//  Expand the cipher key <cipherKey> into the encryption key schedule <rk>
//  return <Nr> = the number of rounds for the given cipher key size (10, 12, 14)
//  The expanded key schedule array comprises [4*(Nr + 1)] 32 bits words
//  <keyBits> is the key length, it can be = 128, 192, or 256 bits
/**
 * @brief Expand the cipher key into the encryption key schedule.
 * @param ctx Pointer to the current AES256 context.
 * @param rk Encryption key schedule.
 * @param cipherKey Key to be expanded.
 * @param keyBits Key length. Key length. See \ref aesKeys for supported sizes.
 * @return Number of rounds for the given cipher key size (10, 12, 14).
 */
static uint8_t B5_rijndaelKeySetupEnc (B5_tAesCtx *ctx, uint32_t *rk, const uint8_t *cipherKey, int16_t keyBits) 
{
    int16_t i;
    uint32_t temp;
    uint32_t ut0, ut1, ut2, ut3, ut4;
    
    i = 0;
    rk[0] = B5_AES256_GETUINT32(cipherKey     );
    rk[1] = B5_AES256_GETUINT32(cipherKey +  4);
    rk[2] = B5_AES256_GETUINT32(cipherKey +  8);
    rk[3] = B5_AES256_GETUINT32(cipherKey + 12);
    
    if (keyBits == 128) 
    {
        for (;;) 
        {
            temp  = rk[3];
            memcpy(&ut0, &ctx->Te4[(temp >> 16) & 0xff], sizeof(uint32_t));
            memcpy(&ut1, &ctx->Te4[(temp >>  8) & 0xff], sizeof(uint32_t));
            memcpy(&ut2, &ctx->Te4[(temp      ) & 0xff], sizeof(uint32_t));
            memcpy(&ut3, &ctx->Te4[(temp >> 24)       ], sizeof(uint32_t));
            memcpy(&ut4, &B5_rcon[i], sizeof(uint32_t));
            
            rk[4] = rk[0] ^
                    (ut0 & 0xff000000) ^
                    (ut1 & 0x00ff0000) ^
                    (ut2 & 0x0000ff00) ^
                    (ut3 & 0x000000ff) ^
                    ut4;
            
            rk[5] = rk[1] ^ rk[4];
            rk[6] = rk[2] ^ rk[5];
            rk[7] = rk[3] ^ rk[6];
            
            if (++i == 10) return 10;
            rk += 4;
        }
    }
    
    rk[4] = B5_AES256_GETUINT32(cipherKey + 16);
    rk[5] = B5_AES256_GETUINT32(cipherKey + 20);
    
    if (keyBits == 192) 
    {
        for (;;) 
        {
            temp = rk[5];
            memcpy(&ut0, &ctx->Te4[(temp >> 16) & 0xff], sizeof(uint32_t));
            memcpy(&ut1, &ctx->Te4[(temp >>  8) & 0xff], sizeof(uint32_t));
            memcpy(&ut2, &ctx->Te4[(temp      ) & 0xff], sizeof(uint32_t));
            memcpy(&ut3, &ctx->Te4[(temp >> 24)       ], sizeof(uint32_t));
            memcpy(&ut4, &B5_rcon[i], sizeof(uint32_t));
            
            rk[ 6] = rk[ 0] ^
                    (ut0 & 0xff000000) ^
                    (ut1 & 0x00ff0000) ^
                    (ut2 & 0x0000ff00) ^
                    (ut3 & 0x000000ff) ^
                    ut4;
            
            rk[ 7] = rk[ 1] ^ rk[ 6];
            rk[ 8] = rk[ 2] ^ rk[ 7];
            rk[ 9] = rk[ 3] ^ rk[ 8];
            
            if (++i == 8) return 12;
            rk[10] = rk[ 4] ^ rk[ 9];
            rk[11] = rk[ 5] ^ rk[10];
            rk += 6;
        }
    }
    
    rk[6] = B5_AES256_GETUINT32(cipherKey + 24);
    rk[7] = B5_AES256_GETUINT32(cipherKey + 28);
    
    if (keyBits == 256) 
    {
        for (;;) 
        {
            temp = rk[ 7];
            memcpy(&ut0, &ctx->Te4[(temp >> 16) & 0xff], sizeof(uint32_t));
            memcpy(&ut1, &ctx->Te4[(temp >>  8) & 0xff], sizeof(uint32_t));
            memcpy(&ut2, &ctx->Te4[(temp      ) & 0xff], sizeof(uint32_t));
            memcpy(&ut3, &ctx->Te4[(temp >> 24)       ], sizeof(uint32_t));
            memcpy(&ut4, &B5_rcon[i], sizeof(uint32_t));
            
            rk[ 8] = rk[ 0] ^
                    (ut0 & 0xff000000) ^
                    (ut1 & 0x00ff0000) ^
                    (ut2 & 0x0000ff00) ^
                    (ut3 & 0x000000ff) ^
                    ut4;
            
            rk[ 9] = rk[ 1] ^ rk[ 8];
            rk[10] = rk[ 2] ^ rk[ 9];
            rk[11] = rk[ 3] ^ rk[10];
            
            if (++i == 7) return 14;
            temp = rk[11];
            memcpy(&ut0, &ctx->Te4[(temp >> 24)       ], sizeof(uint32_t));
            memcpy(&ut1, &ctx->Te4[(temp >> 16) & 0xff], sizeof(uint32_t));
            memcpy(&ut2, &ctx->Te4[(temp >>  8) & 0xff], sizeof(uint32_t));
            memcpy(&ut3, &ctx->Te4[(temp      ) & 0xff], sizeof(uint32_t));
            
            rk[ 12] = rk[ 4] ^
                    (ut0 & 0xff000000) ^
                    (ut1 & 0x00ff0000) ^
                    (ut2 & 0x0000ff00) ^
                    (ut3 & 0x000000ff);
            
            rk[13] = rk[ 5] ^ rk[12];
            rk[14] = rk[ 6] ^ rk[13];
            rk[15] = rk[ 7] ^ rk[14];
            
            rk += 8;
        }
    }
    return 0;
}

/**
 * @brief Expand the cipher key into the decryption key schedule.
 * @param ctx Pointer to the current AES256 context.
 * @param rk Encryption key schedule.
 * @param cipherKey Key to be expanded.
 * @param keyBits Key length. See \ref aesKeys for supported sizes.
 * @return Number of rounds for the given cipher key size (10, 12, 14).
 */
static uint8_t B5_rijndaelKeySetupDec (B5_tAesCtx *ctx, uint32_t *rk, const uint8_t *cipherKey, int16_t keyBits)
{ 
    int Nr, i, j;
    uint32_t temp;
    
    /* expand the cipher key: */
    Nr = B5_rijndaelKeySetupEnc(ctx, rk, cipherKey, keyBits);
    /* invert the order of the round keys: */
    for (i = 0, j = 4*Nr; i < j; i += 4, j -= 4) {
        temp = rk[i    ]; rk[i    ] = rk[j    ]; rk[j    ] = temp;
        temp = rk[i + 1]; rk[i + 1] = rk[j + 1]; rk[j + 1] = temp;
        temp = rk[i + 2]; rk[i + 2] = rk[j + 2]; rk[j + 2] = temp;
        temp = rk[i + 3]; rk[i + 3] = rk[j + 3]; rk[j + 3] = temp;
    }
    /* apply the inverse MixColumn transform to all round keys but the first and the last: */
    for (i = 1; i < Nr; i++) {
        rk += 4;
        rk[0] =
                ctx->Td0[ctx->Te4[(rk[0] >> 24)       ] & 0xff] ^
                ctx->Td1[ctx->Te4[(rk[0] >> 16) & 0xff] & 0xff] ^
                ctx->Td2[ctx->Te4[(rk[0] >>  8) & 0xff] & 0xff] ^
                ctx->Td3[ctx->Te4[(rk[0]      ) & 0xff] & 0xff];
        rk[1] =
                ctx->Td0[ctx->Te4[(rk[1] >> 24)       ] & 0xff] ^
                ctx->Td1[ctx->Te4[(rk[1] >> 16) & 0xff] & 0xff] ^
                ctx->Td2[ctx->Te4[(rk[1] >>  8) & 0xff] & 0xff] ^
                ctx->Td3[ctx->Te4[(rk[1]      ) & 0xff] & 0xff];
        rk[2] =
                ctx->Td0[ctx->Te4[(rk[2] >> 24)       ] & 0xff] ^
                ctx->Td1[ctx->Te4[(rk[2] >> 16) & 0xff] & 0xff] ^
                ctx->Td2[ctx->Te4[(rk[2] >>  8) & 0xff] & 0xff] ^
                ctx->Td3[ctx->Te4[(rk[2]      ) & 0xff] & 0xff];
        rk[3] =
                ctx->Td0[ctx->Te4[(rk[3] >> 24)       ] & 0xff] ^
                ctx->Td1[ctx->Te4[(rk[3] >> 16) & 0xff] & 0xff] ^
                ctx->Td2[ctx->Te4[(rk[3] >>  8) & 0xff] & 0xff] ^
                ctx->Td3[ctx->Te4[(rk[3]      ) & 0xff] & 0xff];
    }
    return (uint8_t) Nr;
}

/**
 * @brief AES single block encryption.
 * @param ctx Pointer to the current AES256 context.
 * @param rk round keys
 * @param Nr number of rounds (depends on the original key length)
 * @param pt input block
 * @param ct output block
 */
static void B5_rijndaelEncrypt (B5_tAesCtx *ctx, uint32_t *rk, int16_t Nr, const uint8_t *pt, uint8_t *ct)
{
    uint32_t s0, s1, s2, s3, t0, t1, t2, t3;
    /*
     * map byte array block to cipher state
     * and add initial round key:
     */
    s0 = B5_AES256_GETUINT32(pt     ) ^ rk[0];
    s1 = B5_AES256_GETUINT32(pt +  4) ^ rk[1];
    s2 = B5_AES256_GETUINT32(pt +  8) ^ rk[2];
    s3 = B5_AES256_GETUINT32(pt + 12) ^ rk[3];
    
    
    /* round 1: */
    t0 = ctx->Te0[s0 >> 24] ^ ctx->Te1[(s1 >> 16) & 0xff] ^ ctx->Te2[(s2 >>  8) & 0xff] ^ ctx->Te3[s3 & 0xff] ^ rk[ 4];
    t1 = ctx->Te0[s1 >> 24] ^ ctx->Te1[(s2 >> 16) & 0xff] ^ ctx->Te2[(s3 >>  8) & 0xff] ^ ctx->Te3[s0 & 0xff] ^ rk[ 5];
    t2 = ctx->Te0[s2 >> 24] ^ ctx->Te1[(s3 >> 16) & 0xff] ^ ctx->Te2[(s0 >>  8) & 0xff] ^ ctx->Te3[s1 & 0xff] ^ rk[ 6];
    t3 = ctx->Te0[s3 >> 24] ^ ctx->Te1[(s0 >> 16) & 0xff] ^ ctx->Te2[(s1 >>  8) & 0xff] ^ ctx->Te3[s2 & 0xff] ^ rk[ 7];
    /* round 2: */
    s0 = ctx->Te0[t0 >> 24] ^ ctx->Te1[(t1 >> 16) & 0xff] ^ ctx->Te2[(t2 >>  8) & 0xff] ^ ctx->Te3[t3 & 0xff] ^ rk[ 8];
    s1 = ctx->Te0[t1 >> 24] ^ ctx->Te1[(t2 >> 16) & 0xff] ^ ctx->Te2[(t3 >>  8) & 0xff] ^ ctx->Te3[t0 & 0xff] ^ rk[ 9];
    s2 = ctx->Te0[t2 >> 24] ^ ctx->Te1[(t3 >> 16) & 0xff] ^ ctx->Te2[(t0 >>  8) & 0xff] ^ ctx->Te3[t1 & 0xff] ^ rk[10];
    s3 = ctx->Te0[t3 >> 24] ^ ctx->Te1[(t0 >> 16) & 0xff] ^ ctx->Te2[(t1 >>  8) & 0xff] ^ ctx->Te3[t2 & 0xff] ^ rk[11];
    /* round 3: */
    t0 = ctx->Te0[s0 >> 24] ^ ctx->Te1[(s1 >> 16) & 0xff] ^ ctx->Te2[(s2 >>  8) & 0xff] ^ ctx->Te3[s3 & 0xff] ^ rk[12];
    t1 = ctx->Te0[s1 >> 24] ^ ctx->Te1[(s2 >> 16) & 0xff] ^ ctx->Te2[(s3 >>  8) & 0xff] ^ ctx->Te3[s0 & 0xff] ^ rk[13];
    t2 = ctx->Te0[s2 >> 24] ^ ctx->Te1[(s3 >> 16) & 0xff] ^ ctx->Te2[(s0 >>  8) & 0xff] ^ ctx->Te3[s1 & 0xff] ^ rk[14];
    t3 = ctx->Te0[s3 >> 24] ^ ctx->Te1[(s0 >> 16) & 0xff] ^ ctx->Te2[(s1 >>  8) & 0xff] ^ ctx->Te3[s2 & 0xff] ^ rk[15];
    /* round 4: */
    s0 = ctx->Te0[t0 >> 24] ^ ctx->Te1[(t1 >> 16) & 0xff] ^ ctx->Te2[(t2 >>  8) & 0xff] ^ ctx->Te3[t3 & 0xff] ^ rk[16];
    s1 = ctx->Te0[t1 >> 24] ^ ctx->Te1[(t2 >> 16) & 0xff] ^ ctx->Te2[(t3 >>  8) & 0xff] ^ ctx->Te3[t0 & 0xff] ^ rk[17];
    s2 = ctx->Te0[t2 >> 24] ^ ctx->Te1[(t3 >> 16) & 0xff] ^ ctx->Te2[(t0 >>  8) & 0xff] ^ ctx->Te3[t1 & 0xff] ^ rk[18];
    s3 = ctx->Te0[t3 >> 24] ^ ctx->Te1[(t0 >> 16) & 0xff] ^ ctx->Te2[(t1 >>  8) & 0xff] ^ ctx->Te3[t2 & 0xff] ^ rk[19];
    /* round 5: */
    t0 = ctx->Te0[s0 >> 24] ^ ctx->Te1[(s1 >> 16) & 0xff] ^ ctx->Te2[(s2 >>  8) & 0xff] ^ ctx->Te3[s3 & 0xff] ^ rk[20];
    t1 = ctx->Te0[s1 >> 24] ^ ctx->Te1[(s2 >> 16) & 0xff] ^ ctx->Te2[(s3 >>  8) & 0xff] ^ ctx->Te3[s0 & 0xff] ^ rk[21];
    t2 = ctx->Te0[s2 >> 24] ^ ctx->Te1[(s3 >> 16) & 0xff] ^ ctx->Te2[(s0 >>  8) & 0xff] ^ ctx->Te3[s1 & 0xff] ^ rk[22];
    t3 = ctx->Te0[s3 >> 24] ^ ctx->Te1[(s0 >> 16) & 0xff] ^ ctx->Te2[(s1 >>  8) & 0xff] ^ ctx->Te3[s2 & 0xff] ^ rk[23];
    /* round 6: */
    s0 = ctx->Te0[t0 >> 24] ^ ctx->Te1[(t1 >> 16) & 0xff] ^ ctx->Te2[(t2 >>  8) & 0xff] ^ ctx->Te3[t3 & 0xff] ^ rk[24];
    s1 = ctx->Te0[t1 >> 24] ^ ctx->Te1[(t2 >> 16) & 0xff] ^ ctx->Te2[(t3 >>  8) & 0xff] ^ ctx->Te3[t0 & 0xff] ^ rk[25];
    s2 = ctx->Te0[t2 >> 24] ^ ctx->Te1[(t3 >> 16) & 0xff] ^ ctx->Te2[(t0 >>  8) & 0xff] ^ ctx->Te3[t1 & 0xff] ^ rk[26];
    s3 = ctx->Te0[t3 >> 24] ^ ctx->Te1[(t0 >> 16) & 0xff] ^ ctx->Te2[(t1 >>  8) & 0xff] ^ ctx->Te3[t2 & 0xff] ^ rk[27];
    /* round 7: */
    t0 = ctx->Te0[s0 >> 24] ^ ctx->Te1[(s1 >> 16) & 0xff] ^ ctx->Te2[(s2 >>  8) & 0xff] ^ ctx->Te3[s3 & 0xff] ^ rk[28];
    t1 = ctx->Te0[s1 >> 24] ^ ctx->Te1[(s2 >> 16) & 0xff] ^ ctx->Te2[(s3 >>  8) & 0xff] ^ ctx->Te3[s0 & 0xff] ^ rk[29];
    t2 = ctx->Te0[s2 >> 24] ^ ctx->Te1[(s3 >> 16) & 0xff] ^ ctx->Te2[(s0 >>  8) & 0xff] ^ ctx->Te3[s1 & 0xff] ^ rk[30];
    t3 = ctx->Te0[s3 >> 24] ^ ctx->Te1[(s0 >> 16) & 0xff] ^ ctx->Te2[(s1 >>  8) & 0xff] ^ ctx->Te3[s2 & 0xff] ^ rk[31];
    /* round 8: */
    s0 = ctx->Te0[t0 >> 24] ^ ctx->Te1[(t1 >> 16) & 0xff] ^ ctx->Te2[(t2 >>  8) & 0xff] ^ ctx->Te3[t3 & 0xff] ^ rk[32];
    s1 = ctx->Te0[t1 >> 24] ^ ctx->Te1[(t2 >> 16) & 0xff] ^ ctx->Te2[(t3 >>  8) & 0xff] ^ ctx->Te3[t0 & 0xff] ^ rk[33];
    s2 = ctx->Te0[t2 >> 24] ^ ctx->Te1[(t3 >> 16) & 0xff] ^ ctx->Te2[(t0 >>  8) & 0xff] ^ ctx->Te3[t1 & 0xff] ^ rk[34];
    s3 = ctx->Te0[t3 >> 24] ^ ctx->Te1[(t0 >> 16) & 0xff] ^ ctx->Te2[(t1 >>  8) & 0xff] ^ ctx->Te3[t2 & 0xff] ^ rk[35];
    /* round 9: */
    t0 = ctx->Te0[s0 >> 24] ^ ctx->Te1[(s1 >> 16) & 0xff] ^ ctx->Te2[(s2 >>  8) & 0xff] ^ ctx->Te3[s3 & 0xff] ^ rk[36];
    t1 = ctx->Te0[s1 >> 24] ^ ctx->Te1[(s2 >> 16) & 0xff] ^ ctx->Te2[(s3 >>  8) & 0xff] ^ ctx->Te3[s0 & 0xff] ^ rk[37];
    t2 = ctx->Te0[s2 >> 24] ^ ctx->Te1[(s3 >> 16) & 0xff] ^ ctx->Te2[(s0 >>  8) & 0xff] ^ ctx->Te3[s1 & 0xff] ^ rk[38];
    t3 = ctx->Te0[s3 >> 24] ^ ctx->Te1[(s0 >> 16) & 0xff] ^ ctx->Te2[(s1 >>  8) & 0xff] ^ ctx->Te3[s2 & 0xff] ^ rk[39];
    if (Nr > 10) {
        /* round 10: */
        s0 = ctx->Te0[t0 >> 24] ^ ctx->Te1[(t1 >> 16) & 0xff] ^ ctx->Te2[(t2 >>  8) & 0xff] ^ ctx->Te3[t3 & 0xff] ^ rk[40];
        s1 = ctx->Te0[t1 >> 24] ^ ctx->Te1[(t2 >> 16) & 0xff] ^ ctx->Te2[(t3 >>  8) & 0xff] ^ ctx->Te3[t0 & 0xff] ^ rk[41];
        s2 = ctx->Te0[t2 >> 24] ^ ctx->Te1[(t3 >> 16) & 0xff] ^ ctx->Te2[(t0 >>  8) & 0xff] ^ ctx->Te3[t1 & 0xff] ^ rk[42];
        s3 = ctx->Te0[t3 >> 24] ^ ctx->Te1[(t0 >> 16) & 0xff] ^ ctx->Te2[(t1 >>  8) & 0xff] ^ ctx->Te3[t2 & 0xff] ^ rk[43];
        /* round 11: */
        t0 = ctx->Te0[s0 >> 24] ^ ctx->Te1[(s1 >> 16) & 0xff] ^ ctx->Te2[(s2 >>  8) & 0xff] ^ ctx->Te3[s3 & 0xff] ^ rk[44];
        t1 = ctx->Te0[s1 >> 24] ^ ctx->Te1[(s2 >> 16) & 0xff] ^ ctx->Te2[(s3 >>  8) & 0xff] ^ ctx->Te3[s0 & 0xff] ^ rk[45];
        t2 = ctx->Te0[s2 >> 24] ^ ctx->Te1[(s3 >> 16) & 0xff] ^ ctx->Te2[(s0 >>  8) & 0xff] ^ ctx->Te3[s1 & 0xff] ^ rk[46];
        t3 = ctx->Te0[s3 >> 24] ^ ctx->Te1[(s0 >> 16) & 0xff] ^ ctx->Te2[(s1 >>  8) & 0xff] ^ ctx->Te3[s2 & 0xff] ^ rk[47];
        if (Nr > 12) {
            /* round 12: */
            s0 = ctx->Te0[t0 >> 24] ^ ctx->Te1[(t1 >> 16) & 0xff] ^ ctx->Te2[(t2 >>  8) & 0xff] ^ ctx->Te3[t3 & 0xff] ^ rk[48];
            s1 = ctx->Te0[t1 >> 24] ^ ctx->Te1[(t2 >> 16) & 0xff] ^ ctx->Te2[(t3 >>  8) & 0xff] ^ ctx->Te3[t0 & 0xff] ^ rk[49];
            s2 = ctx->Te0[t2 >> 24] ^ ctx->Te1[(t3 >> 16) & 0xff] ^ ctx->Te2[(t0 >>  8) & 0xff] ^ ctx->Te3[t1 & 0xff] ^ rk[50];
            s3 = ctx->Te0[t3 >> 24] ^ ctx->Te1[(t0 >> 16) & 0xff] ^ ctx->Te2[(t1 >>  8) & 0xff] ^ ctx->Te3[t2 & 0xff] ^ rk[51];
            /* round 13: */
            t0 = ctx->Te0[s0 >> 24] ^ ctx->Te1[(s1 >> 16) & 0xff] ^ ctx->Te2[(s2 >>  8) & 0xff] ^ ctx->Te3[s3 & 0xff] ^ rk[52];
            t1 = ctx->Te0[s1 >> 24] ^ ctx->Te1[(s2 >> 16) & 0xff] ^ ctx->Te2[(s3 >>  8) & 0xff] ^ ctx->Te3[s0 & 0xff] ^ rk[53];
            t2 = ctx->Te0[s2 >> 24] ^ ctx->Te1[(s3 >> 16) & 0xff] ^ ctx->Te2[(s0 >>  8) & 0xff] ^ ctx->Te3[s1 & 0xff] ^ rk[54];
            t3 = ctx->Te0[s3 >> 24] ^ ctx->Te1[(s0 >> 16) & 0xff] ^ ctx->Te2[(s1 >>  8) & 0xff] ^ ctx->Te3[s2 & 0xff] ^ rk[55];
        }
    }
    rk += Nr << 2;
    /*
     * apply last round and
     * map cipher Te to Te array block:
     */
    s0 =
            (ctx->Te4[(t0 >> 24)       ] & 0xff000000) ^
            (ctx->Te4[(t1 >> 16) & 0xff] & 0x00ff0000) ^
            (ctx->Te4[(t2 >>  8) & 0xff] & 0x0000ff00) ^
            (ctx->Te4[(t3      ) & 0xff] & 0x000000ff) ^
            rk[0];
    B5_AES256_PUTUINT32(ct     , s0);
    s1 =
            (ctx->Te4[(t1 >> 24)       ] & 0xff000000) ^
            (ctx->Te4[(t2 >> 16) & 0xff] & 0x00ff0000) ^
            (ctx->Te4[(t3 >>  8) & 0xff] & 0x0000ff00) ^
            (ctx->Te4[(t0      ) & 0xff] & 0x000000ff) ^
            rk[1];
    B5_AES256_PUTUINT32(ct +  4, s1);
    s2 =
            (ctx->Te4[(t2 >> 24)       ] & 0xff000000) ^
            (ctx->Te4[(t3 >> 16) & 0xff] & 0x00ff0000) ^
            (ctx->Te4[(t0 >>  8) & 0xff] & 0x0000ff00) ^
            (ctx->Te4[(t1      ) & 0xff] & 0x000000ff) ^
            rk[2];
    B5_AES256_PUTUINT32(ct +  8, s2);
    s3 =
            (ctx->Te4[(t3 >> 24)       ] & 0xff000000) ^
            (ctx->Te4[(t0 >> 16) & 0xff] & 0x00ff0000) ^
            (ctx->Te4[(t1 >>  8) & 0xff] & 0x0000ff00) ^
            (ctx->Te4[(t2      ) & 0xff] & 0x000000ff) ^
            rk[3];
    B5_AES256_PUTUINT32(ct + 12, s3);
}

/**
 * @brief AES single block decryption.
 * @param ctx Pointer to the current AES256 context.
 * @param rk round keys
 * @param Nr number of rounds (depends on the original key length)
 * @param ct output block
 * @param pt input block
 */
void B5_rijndaelDecrypt (B5_tAesCtx *ctx, uint32_t *rk, int16_t Nr, const uint8_t *ct, uint8_t *pt)
{
    uint32_t s0, s1, s2, s3, t0, t1, t2, t3;
    
    /*
     map byte array block to cipher state
     and add initial round key:
     */
    s0 = B5_AES256_GETUINT32(ct     ) ^ rk[0];
    s1 = B5_AES256_GETUINT32(ct +  4) ^ rk[1];
    s2 = B5_AES256_GETUINT32(ct +  8) ^ rk[2];
    s3 = B5_AES256_GETUINT32(ct + 12) ^ rk[3];
    
    
    /* round 1: */
    t0 = ctx->Td0[s0 >> 24] ^ ctx->Td1[(s3 >> 16) & 0xff] ^ ctx->Td2[(s2 >>  8) & 0xff] ^ ctx->Td3[s1 & 0xff] ^ rk[ 4];
    t1 = ctx->Td0[s1 >> 24] ^ ctx->Td1[(s0 >> 16) & 0xff] ^ ctx->Td2[(s3 >>  8) & 0xff] ^ ctx->Td3[s2 & 0xff] ^ rk[ 5];
    t2 = ctx->Td0[s2 >> 24] ^ ctx->Td1[(s1 >> 16) & 0xff] ^ ctx->Td2[(s0 >>  8) & 0xff] ^ ctx->Td3[s3 & 0xff] ^ rk[ 6];
    t3 = ctx->Td0[s3 >> 24] ^ ctx->Td1[(s2 >> 16) & 0xff] ^ ctx->Td2[(s1 >>  8) & 0xff] ^ ctx->Td3[s0 & 0xff] ^ rk[ 7];
    /* round 2: */
    s0 = ctx->Td0[t0 >> 24] ^ ctx->Td1[(t3 >> 16) & 0xff] ^ ctx->Td2[(t2 >>  8) & 0xff] ^ ctx->Td3[t1 & 0xff] ^ rk[ 8];
    s1 = ctx->Td0[t1 >> 24] ^ ctx->Td1[(t0 >> 16) & 0xff] ^ ctx->Td2[(t3 >>  8) & 0xff] ^ ctx->Td3[t2 & 0xff] ^ rk[ 9];
    s2 = ctx->Td0[t2 >> 24] ^ ctx->Td1[(t1 >> 16) & 0xff] ^ ctx->Td2[(t0 >>  8) & 0xff] ^ ctx->Td3[t3 & 0xff] ^ rk[10];
    s3 = ctx->Td0[t3 >> 24] ^ ctx->Td1[(t2 >> 16) & 0xff] ^ ctx->Td2[(t1 >>  8) & 0xff] ^ ctx->Td3[t0 & 0xff] ^ rk[11];
    /* round 3: */
    t0 = ctx->Td0[s0 >> 24] ^ ctx->Td1[(s3 >> 16) & 0xff] ^ ctx->Td2[(s2 >>  8) & 0xff] ^ ctx->Td3[s1 & 0xff] ^ rk[12];
    t1 = ctx->Td0[s1 >> 24] ^ ctx->Td1[(s0 >> 16) & 0xff] ^ ctx->Td2[(s3 >>  8) & 0xff] ^ ctx->Td3[s2 & 0xff] ^ rk[13];
    t2 = ctx->Td0[s2 >> 24] ^ ctx->Td1[(s1 >> 16) & 0xff] ^ ctx->Td2[(s0 >>  8) & 0xff] ^ ctx->Td3[s3 & 0xff] ^ rk[14];
    t3 = ctx->Td0[s3 >> 24] ^ ctx->Td1[(s2 >> 16) & 0xff] ^ ctx->Td2[(s1 >>  8) & 0xff] ^ ctx->Td3[s0 & 0xff] ^ rk[15];
    /* round 4: */
    s0 = ctx->Td0[t0 >> 24] ^ ctx->Td1[(t3 >> 16) & 0xff] ^ ctx->Td2[(t2 >>  8) & 0xff] ^ ctx->Td3[t1 & 0xff] ^ rk[16];
    s1 = ctx->Td0[t1 >> 24] ^ ctx->Td1[(t0 >> 16) & 0xff] ^ ctx->Td2[(t3 >>  8) & 0xff] ^ ctx->Td3[t2 & 0xff] ^ rk[17];
    s2 = ctx->Td0[t2 >> 24] ^ ctx->Td1[(t1 >> 16) & 0xff] ^ ctx->Td2[(t0 >>  8) & 0xff] ^ ctx->Td3[t3 & 0xff] ^ rk[18];
    s3 = ctx->Td0[t3 >> 24] ^ ctx->Td1[(t2 >> 16) & 0xff] ^ ctx->Td2[(t1 >>  8) & 0xff] ^ ctx->Td3[t0 & 0xff] ^ rk[19];
    /* round 5: */
    t0 = ctx->Td0[s0 >> 24] ^ ctx->Td1[(s3 >> 16) & 0xff] ^ ctx->Td2[(s2 >>  8) & 0xff] ^ ctx->Td3[s1 & 0xff] ^ rk[20];
    t1 = ctx->Td0[s1 >> 24] ^ ctx->Td1[(s0 >> 16) & 0xff] ^ ctx->Td2[(s3 >>  8) & 0xff] ^ ctx->Td3[s2 & 0xff] ^ rk[21];
    t2 = ctx->Td0[s2 >> 24] ^ ctx->Td1[(s1 >> 16) & 0xff] ^ ctx->Td2[(s0 >>  8) & 0xff] ^ ctx->Td3[s3 & 0xff] ^ rk[22];
    t3 = ctx->Td0[s3 >> 24] ^ ctx->Td1[(s2 >> 16) & 0xff] ^ ctx->Td2[(s1 >>  8) & 0xff] ^ ctx->Td3[s0 & 0xff] ^ rk[23];
    /* round 6: */
    s0 = ctx->Td0[t0 >> 24] ^ ctx->Td1[(t3 >> 16) & 0xff] ^ ctx->Td2[(t2 >>  8) & 0xff] ^ ctx->Td3[t1 & 0xff] ^ rk[24];
    s1 = ctx->Td0[t1 >> 24] ^ ctx->Td1[(t0 >> 16) & 0xff] ^ ctx->Td2[(t3 >>  8) & 0xff] ^ ctx->Td3[t2 & 0xff] ^ rk[25];
    s2 = ctx->Td0[t2 >> 24] ^ ctx->Td1[(t1 >> 16) & 0xff] ^ ctx->Td2[(t0 >>  8) & 0xff] ^ ctx->Td3[t3 & 0xff] ^ rk[26];
    s3 = ctx->Td0[t3 >> 24] ^ ctx->Td1[(t2 >> 16) & 0xff] ^ ctx->Td2[(t1 >>  8) & 0xff] ^ ctx->Td3[t0 & 0xff] ^ rk[27];
    /* round 7: */
    t0 = ctx->Td0[s0 >> 24] ^ ctx->Td1[(s3 >> 16) & 0xff] ^ ctx->Td2[(s2 >>  8) & 0xff] ^ ctx->Td3[s1 & 0xff] ^ rk[28];
    t1 = ctx->Td0[s1 >> 24] ^ ctx->Td1[(s0 >> 16) & 0xff] ^ ctx->Td2[(s3 >>  8) & 0xff] ^ ctx->Td3[s2 & 0xff] ^ rk[29];
    t2 = ctx->Td0[s2 >> 24] ^ ctx->Td1[(s1 >> 16) & 0xff] ^ ctx->Td2[(s0 >>  8) & 0xff] ^ ctx->Td3[s3 & 0xff] ^ rk[30];
    t3 = ctx->Td0[s3 >> 24] ^ ctx->Td1[(s2 >> 16) & 0xff] ^ ctx->Td2[(s1 >>  8) & 0xff] ^ ctx->Td3[s0 & 0xff] ^ rk[31];
    /* round 8: */
    s0 = ctx->Td0[t0 >> 24] ^ ctx->Td1[(t3 >> 16) & 0xff] ^ ctx->Td2[(t2 >>  8) & 0xff] ^ ctx->Td3[t1 & 0xff] ^ rk[32];
    s1 = ctx->Td0[t1 >> 24] ^ ctx->Td1[(t0 >> 16) & 0xff] ^ ctx->Td2[(t3 >>  8) & 0xff] ^ ctx->Td3[t2 & 0xff] ^ rk[33];
    s2 = ctx->Td0[t2 >> 24] ^ ctx->Td1[(t1 >> 16) & 0xff] ^ ctx->Td2[(t0 >>  8) & 0xff] ^ ctx->Td3[t3 & 0xff] ^ rk[34];
    s3 = ctx->Td0[t3 >> 24] ^ ctx->Td1[(t2 >> 16) & 0xff] ^ ctx->Td2[(t1 >>  8) & 0xff] ^ ctx->Td3[t0 & 0xff] ^ rk[35];
    /* round 9: */
    t0 = ctx->Td0[s0 >> 24] ^ ctx->Td1[(s3 >> 16) & 0xff] ^ ctx->Td2[(s2 >>  8) & 0xff] ^ ctx->Td3[s1 & 0xff] ^ rk[36];
    t1 = ctx->Td0[s1 >> 24] ^ ctx->Td1[(s0 >> 16) & 0xff] ^ ctx->Td2[(s3 >>  8) & 0xff] ^ ctx->Td3[s2 & 0xff] ^ rk[37];
    t2 = ctx->Td0[s2 >> 24] ^ ctx->Td1[(s1 >> 16) & 0xff] ^ ctx->Td2[(s0 >>  8) & 0xff] ^ ctx->Td3[s3 & 0xff] ^ rk[38];
    t3 = ctx->Td0[s3 >> 24] ^ ctx->Td1[(s2 >> 16) & 0xff] ^ ctx->Td2[(s1 >>  8) & 0xff] ^ ctx->Td3[s0 & 0xff] ^ rk[39];
    if (Nr > 10) {
        /* round 10: */
        s0 = ctx->Td0[t0 >> 24] ^ ctx->Td1[(t3 >> 16) & 0xff] ^ ctx->Td2[(t2 >>  8) & 0xff] ^ ctx->Td3[t1 & 0xff] ^ rk[40];
        s1 = ctx->Td0[t1 >> 24] ^ ctx->Td1[(t0 >> 16) & 0xff] ^ ctx->Td2[(t3 >>  8) & 0xff] ^ ctx->Td3[t2 & 0xff] ^ rk[41];
        s2 = ctx->Td0[t2 >> 24] ^ ctx->Td1[(t1 >> 16) & 0xff] ^ ctx->Td2[(t0 >>  8) & 0xff] ^ ctx->Td3[t3 & 0xff] ^ rk[42];
        s3 = ctx->Td0[t3 >> 24] ^ ctx->Td1[(t2 >> 16) & 0xff] ^ ctx->Td2[(t1 >>  8) & 0xff] ^ ctx->Td3[t0 & 0xff] ^ rk[43];
        /* round 11: */
        t0 = ctx->Td0[s0 >> 24] ^ ctx->Td1[(s3 >> 16) & 0xff] ^ ctx->Td2[(s2 >>  8) & 0xff] ^ ctx->Td3[s1 & 0xff] ^ rk[44];
        t1 = ctx->Td0[s1 >> 24] ^ ctx->Td1[(s0 >> 16) & 0xff] ^ ctx->Td2[(s3 >>  8) & 0xff] ^ ctx->Td3[s2 & 0xff] ^ rk[45];
        t2 = ctx->Td0[s2 >> 24] ^ ctx->Td1[(s1 >> 16) & 0xff] ^ ctx->Td2[(s0 >>  8) & 0xff] ^ ctx->Td3[s3 & 0xff] ^ rk[46];
        t3 = ctx->Td0[s3 >> 24] ^ ctx->Td1[(s2 >> 16) & 0xff] ^ ctx->Td2[(s1 >>  8) & 0xff] ^ ctx->Td3[s0 & 0xff] ^ rk[47];
        if (Nr > 12) {
            /* round 12: */
            s0 = ctx->Td0[t0 >> 24] ^ ctx->Td1[(t3 >> 16) & 0xff] ^ ctx->Td2[(t2 >>  8) & 0xff] ^ ctx->Td3[t1 & 0xff] ^ rk[48];
            s1 = ctx->Td0[t1 >> 24] ^ ctx->Td1[(t0 >> 16) & 0xff] ^ ctx->Td2[(t3 >>  8) & 0xff] ^ ctx->Td3[t2 & 0xff] ^ rk[49];
            s2 = ctx->Td0[t2 >> 24] ^ ctx->Td1[(t1 >> 16) & 0xff] ^ ctx->Td2[(t0 >>  8) & 0xff] ^ ctx->Td3[t3 & 0xff] ^ rk[50];
            s3 = ctx->Td0[t3 >> 24] ^ ctx->Td1[(t2 >> 16) & 0xff] ^ ctx->Td2[(t1 >>  8) & 0xff] ^ ctx->Td3[t0 & 0xff] ^ rk[51];
            /* round 13: */
            t0 = ctx->Td0[s0 >> 24] ^ ctx->Td1[(s3 >> 16) & 0xff] ^ ctx->Td2[(s2 >>  8) & 0xff] ^ ctx->Td3[s1 & 0xff] ^ rk[52];
            t1 = ctx->Td0[s1 >> 24] ^ ctx->Td1[(s0 >> 16) & 0xff] ^ ctx->Td2[(s3 >>  8) & 0xff] ^ ctx->Td3[s2 & 0xff] ^ rk[53];
            t2 = ctx->Td0[s2 >> 24] ^ ctx->Td1[(s1 >> 16) & 0xff] ^ ctx->Td2[(s0 >>  8) & 0xff] ^ ctx->Td3[s3 & 0xff] ^ rk[54];
            t3 = ctx->Td0[s3 >> 24] ^ ctx->Td1[(s2 >> 16) & 0xff] ^ ctx->Td2[(s1 >>  8) & 0xff] ^ ctx->Td3[s0 & 0xff] ^ rk[55];
        }
    }
    rk += Nr << 2;
    /*
     * apply last round and
     * map cipher state to byte array block:
     */
    s0 =
            (ctx->Td4[(t0 >> 24)       ] & 0xff000000) ^
            (ctx->Td4[(t3 >> 16) & 0xff] & 0x00ff0000) ^
            (ctx->Td4[(t2 >>  8) & 0xff] & 0x0000ff00) ^
            (ctx->Td4[(t1      ) & 0xff] & 0x000000ff) ^
            rk[0];
    B5_AES256_PUTUINT32(pt     , s0);
    s1 =
            (ctx->Td4[(t1 >> 24)       ] & 0xff000000) ^
            (ctx->Td4[(t0 >> 16) & 0xff] & 0x00ff0000) ^
            (ctx->Td4[(t3 >>  8) & 0xff] & 0x0000ff00) ^
            (ctx->Td4[(t2      ) & 0xff] & 0x000000ff) ^
            rk[1];
    B5_AES256_PUTUINT32(pt +  4, s1);
    s2 =
            (ctx->Td4[(t2 >> 24)       ] & 0xff000000) ^
            (ctx->Td4[(t1 >> 16) & 0xff] & 0x00ff0000) ^
            (ctx->Td4[(t0 >>  8) & 0xff] & 0x0000ff00) ^
            (ctx->Td4[(t3      ) & 0xff] & 0x000000ff) ^
            rk[2];
    B5_AES256_PUTUINT32(pt +  8, s2);
    s3 =
            (ctx->Td4[(t3 >> 24)       ] & 0xff000000) ^
            (ctx->Td4[(t2 >> 16) & 0xff] & 0x00ff0000) ^
            (ctx->Td4[(t1 >>  8) & 0xff] & 0x0000ff00) ^
            (ctx->Td4[(t0      ) & 0xff] & 0x000000ff) ^
            rk[3];
    B5_AES256_PUTUINT32(pt + 12, s3);
}

int32_t B5_Aes256_Init (B5_tAesCtx *ctx, const uint8_t *Key, int16_t keySize, uint8_t aesMode)
{
    if(Key == NULL) 
        return B5_AES256_RES_INVALID_ARGUMENT;
    
    if(ctx == NULL)
        return  B5_AES256_RES_INVALID_CONTEXT;
    
    memset(ctx, 0, sizeof(B5_tAesCtx));
    
    if ((aesMode < B5_AES256_OFB) || (aesMode > B5_AES256_CTR ))
        return B5_AES256_RES_INVALID_MODE;
    
    if((keySize != B5_AES_128) && (keySize != B5_AES_192) && (keySize != B5_AES_256)) 
        return B5_AES256_RES_INVALID_KEY_SIZE;
    
    ctx->mode = aesMode;
    
    ctx->Te0 = B5Te0_S;
    ctx->Te1 = B5Te1_S;
    ctx->Te2 = B5Te2_S;
    ctx->Te3 = B5Te3_S;
    ctx->Te4 = B5Te4_S;
    ctx->Td0 = B5Td0_S;
    ctx->Td1 = B5Td1_S;
    ctx->Td2 = B5Td2_S;
    ctx->Td3 = B5Td3_S;
    ctx->Td4 = B5Td4_S;
    
    memset(ctx->InitVector, 0x55, B5_AES_IV_SIZE);
    
    if ((ctx->mode == B5_AES256_ECB_ENC) || (ctx->mode == B5_AES256_OFB) || (ctx->mode == B5_AES256_CBC_ENC) || (ctx->mode == B5_AES256_CTR) || (ctx->mode == B5_AES256_CFB_ENC) || (ctx->mode == B5_AES256_CFB_DEC)) 
    {
        ctx->Nr = B5_rijndaelKeySetupEnc(ctx, ctx->rk, Key, keySize<<3);
    } 
    else if ((ctx->mode == B5_AES256_ECB_DEC) || (ctx->mode == B5_AES256_CBC_DEC) )  
    {
        ctx->Nr = B5_rijndaelKeySetupDec(ctx, ctx->rk, Key, keySize<<3);
    }
    else
    {
        return B5_AES256_RES_INVALID_MODE;        
    }
    
    return B5_AES256_RES_OK;
}

int32_t B5_Aes256_SetIV (B5_tAesCtx    *ctx, const uint8_t *IV)
{
    if(ctx == NULL)
        return  B5_AES256_RES_INVALID_CONTEXT;
    
    
    if(IV == NULL) 
        return B5_AES256_RES_INVALID_ARGUMENT;
    
    
    if ( (ctx->mode != B5_AES256_OFB) && (ctx->mode != B5_AES256_CBC_ENC) && (ctx->mode != B5_AES256_CBC_DEC) && (ctx->mode != B5_AES256_CTR) && 
         (ctx->mode != B5_AES256_CFB_ENC) &&  (ctx->mode != B5_AES256_CFB_DEC))    
        return B5_AES256_RES_INVALID_MODE; 
    
    memcpy(ctx->InitVector, IV, B5_AES_IV_SIZE);
    
    return B5_AES256_RES_OK;
}

int32_t B5_Aes256_Update (B5_tAesCtx	*ctx, uint8_t *encData, uint8_t *clrData, int16_t nBlk)
{
    int16_t    i, j, cb;
    uint8_t    tmp[B5_AES_BLK_SIZE];
    
    
    
    if(ctx == NULL)
        return  B5_AES256_RES_INVALID_CONTEXT;
    
    
    if((encData == NULL) || (clrData == NULL) || (nBlk <= 0))
        return B5_AES256_RES_INVALID_ARGUMENT;
    
    
    switch(ctx->mode) {
        
        
        case B5_AES256_CTR: 
        {
            for (i = 0; i < nBlk; i++) 
            {
                B5_rijndaelEncrypt(ctx, ctx->rk, ctx->Nr, ctx->InitVector, encData);
                for (j = 0; j < B5_AES_BLK_SIZE; j++) 
                {
                    //*encData = *clrData++ ^ encData[j];
                    //*encData++;
                    /*Working*/
					*encData = *encData ^ *clrData;
					encData++;
					clrData++;
                }
                
                j = 15;
                do {
                    ctx->InitVector[j]++;
                    cb = ctx->InitVector[j] == 0;
                } while( j-- && cb ); 
            }
            
            break;
        }
        
        
        case B5_AES256_OFB: 
        {
            for (i = 0; i < nBlk; i++) 
            {
                B5_rijndaelEncrypt(ctx, ctx->rk, ctx->Nr, ctx->InitVector, ctx->InitVector);
                for (j = 0; j < 16; j++) 
                {
                    *encData++ = *clrData++ ^ ctx->InitVector[j];
                }
            }
            
            break;
        }
        
        
        
        case B5_AES256_ECB_ENC:
        {
            for (i = 0; i < nBlk; i++) 
            {
                B5_rijndaelEncrypt(ctx, ctx->rk, ctx->Nr, clrData, encData);
                clrData += 16;
                encData += 16;
            }
            
            break;
        }
        
        
        case B5_AES256_ECB_DEC:
        {
            for (i = 0; i < nBlk; i++) 
            {
                B5_rijndaelDecrypt(ctx, ctx->rk, ctx->Nr, encData, clrData);
                clrData += 16;
                encData += 16;
            }
            
            break;
        }
        
        
        case B5_AES256_CBC_ENC:
        {
            for (i = 0; i < nBlk; i++) 
            {
                for (j = 0; j < 16; j++) 
                {
                    tmp[j] = clrData[j] ^ ctx->InitVector[j];
                }      
                
                B5_rijndaelEncrypt(ctx, ctx->rk, ctx->Nr, tmp, encData);
                for (j = 0; j < 16; j++) 
                {
                    ctx->InitVector[j] = encData[j];
                }
                
                clrData += 16;
                encData += 16;
            }
            
            break;
        }
        
        
        case B5_AES256_CBC_DEC:
        {
            for (i = 0; i < nBlk; i++) 
            {
                for (j = 0; j < 16; j++) 
                {
                    tmp[j] = encData[j];
                }
                
                B5_rijndaelDecrypt(ctx, ctx->rk, ctx->Nr, encData, clrData);
                for (j = 0; j < 16; j++) 
                {
                    clrData[j] ^= ctx->InitVector[j];
                    ctx->InitVector[j] = tmp[j];
                }            
                
                clrData += 16;
                encData += 16;
            }
            
            break;
        }
        
        
        case B5_AES256_CFB_ENC:
        {
            for (i = 0; i < nBlk; i++) 
            {
                B5_rijndaelEncrypt(ctx, ctx->rk, ctx->Nr, ctx->InitVector, tmp);             
                for (j = 0; j < 16; j++) 
                {
                    encData[j] = clrData[j] ^ tmp[j];
                    ctx->InitVector[j] = encData[j]; 
                }      
                
                
                clrData += 16;
                encData += 16;
            }
            
            break;
        }
        
        
        case B5_AES256_CFB_DEC:
        {
            for (i = 0; i < nBlk; i++) 
            {
                B5_rijndaelEncrypt(ctx, ctx->rk, ctx->Nr, ctx->InitVector, tmp);
                for (j = 0; j < 16; j++) 
                {
                    ctx->InitVector[j] = encData[j];
                    clrData[j] = encData[j] ^ tmp[j];
                }            
                
                clrData += 16;
                encData += 16;
            }
            
            break;
        }   
        
        
        default:
        {
            
            return B5_AES256_RES_INVALID_MODE;
        }
        
    }
    
    
    return B5_AES256_RES_OK;
}

int32_t B5_Aes256_Finit (B5_tAesCtx    *ctx)
{
    return B5_AES256_RES_OK;
}

int32_t B5_CmacAes256_Sign (const uint8_t *data, int32_t dataLen, const uint8_t *Key, int16_t keySize, uint8_t *rSignature)
{
    uint8_t        K1[B5_AES_BLK_SIZE], K2[B5_AES_BLK_SIZE];
    uint8_t        L[B5_AES_BLK_SIZE], C[B5_AES_BLK_SIZE], Z[B5_AES_BLK_SIZE];
    uint8_t        MN[B5_AES_BLK_SIZE];
    uint8_t        Rb16[16]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87};
    int32_t        i;
    B5_tAesCtx      aesCtx;
    
    
    if((data == NULL) || (dataLen < 0) || (Key == NULL) || (rSignature == NULL))
        return B5_CMAC_AES256_RES_INVALID_ARGUMENT;
    
    if((keySize != B5_CMAC_AES_128) && (keySize != B5_CMAC_AES_192) && (keySize != B5_CMAC_AES_256)) 
        return B5_CMAC_AES256_RES_INVALID_KEY_SIZE;
    
    
    memset(Z, 0x00, sizeof(Z));
    
    
    // Init AES to prepare K1 and K2 subKeys
    B5_Aes256_Init(&aesCtx, Key, keySize, B5_AES256_ECB_ENC);
    B5_Aes256_Update(&aesCtx, L, Z, 1);
    
    
    // Prepare K1
    for (i=0; i < (B5_AES_BLK_SIZE-1); i++)
        K1[i] = (L[i] << 1) + (L[i+1] >> 7);
    K1[i] = L[i] << 1;
    
    if ((L[0] & 0x80) == 0x80) 
    {
        for (i=0; i < B5_AES_BLK_SIZE; i++)
            K1[i] ^= Rb16[i];
    }
    
    
    // Prepare K2
    for (i=0; i < (B5_AES_BLK_SIZE-1); i++)
        K2[i] = (K1[i] << 1) + (K1[i+1] >> 7);
    K2[i] = K1[i] << 1;
    
    if ((K1[0] & 0x80) == 0x80) {
        for (i=0; i < B5_AES_BLK_SIZE; i++)
            K2[i] ^= Rb16[i];
    }
    
    
    // Calculate MAC (from 1 to N-1 blk)
    memcpy(C, Z, sizeof(Z));
    while(dataLen > B5_AES_BLK_SIZE)
    {
        for (i=0; i < B5_AES_BLK_SIZE; i++)
            C[i] ^= data[i];
        
        B5_Aes256_Update(&aesCtx, C, C, 1);
        
        dataLen -= B5_AES_BLK_SIZE;
        data += B5_AES_BLK_SIZE;
    }
    
    
    // Last Block
    if(dataLen == B5_AES_BLK_SIZE)
    {
        for (i=0; i < dataLen; i++)
            MN[i] = K1[i] ^ data[i];
    }
    else
    {
        for (i=0; i < B5_AES_BLK_SIZE; i++)
            MN[i] = K2[i];	
        for (i=0; i < dataLen; i++)
            MN[i] ^= data[i];
        MN[dataLen] ^= 0x80;
    }
    
    
    for (i=0; i<B5_AES_BLK_SIZE; i++)
        C[i] ^= MN[i];
    
    B5_Aes256_Update(&aesCtx, rSignature, C, 1);
    B5_Aes256_Finit(&aesCtx);
    
    
    return B5_CMAC_AES256_RES_OK;
}

int32_t B5_CmacAes256_Init (B5_tCmacAesCtx *ctx, const uint8_t *Key, int16_t keySize)
{
    uint8_t    Z[16];
    uint8_t    L[16];
    uint8_t    Rb16[16]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87};
    int32_t    i;
    
    
    if(Key == NULL) 
        return B5_CMAC_AES256_RES_INVALID_ARGUMENT;
    
    if(ctx == NULL)
        return  B5_CMAC_AES256_RES_INVALID_CONTEXT;
    
    memset(ctx, 0, sizeof(B5_tCmacAesCtx));
    
    
    memset(Z, 0x00, sizeof(Z));
    
    
    // Init AES to prepare K1 and K2 subKeys
    B5_Aes256_Init(&ctx->aesCtx, Key, keySize, B5_AES256_ECB_ENC);
    B5_Aes256_Update(&ctx->aesCtx, L, Z, 1);
    
    // Prepare K1
    for (i=0; i < (B5_AES_BLK_SIZE-1); i++)
        ctx->K1[i] = (L[i] << 1) + (L[i+1] >> 7);
    ctx->K1[i] = L[i] << 1;
    
    if ((L[0] & 0x80) == 0x80) 
    {
        for (i=0; i < B5_AES_BLK_SIZE; i++)
            ctx->K1[i] ^= Rb16[i];
    }
    
    // Prepare K2
    for (i=0; i < (B5_AES_BLK_SIZE-1); i++)
        ctx->K2[i] = (ctx->K1[i] << 1) + (ctx->K1[i+1] >> 7);
    ctx->K2[i] = ctx->K1[i] << 1;
    
    if ((ctx->K1[0] & 0x80) == 0x80) {
        for (i=0; i < B5_AES_BLK_SIZE; i++)
            ctx->K2[i] ^= Rb16[i];
    }
    
    memcpy(ctx->C, Z, sizeof(Z));
    
    ctx->tmpBlkLen = 0;
    
    return B5_CMAC_AES256_RES_OK;
}

int32_t B5_CmacAes256_Update (B5_tCmacAesCtx *ctx, const uint8_t *data, int32_t dataLen)
{
    int32_t    i;
    
    
    if(ctx == NULL)
        return  B5_CMAC_AES256_RES_INVALID_CONTEXT;
    
    
    if((data == NULL) || (dataLen < 0))
        return B5_CMAC_AES256_RES_INVALID_ARGUMENT;
    
    
    if(dataLen == 0)
        return B5_CMAC_AES256_RES_OK;
    
    
    if(ctx->tmpBlkLen > 0)
    {
        // Not enough
        if((dataLen + ctx->tmpBlkLen) <= B5_AES_BLK_SIZE)
        {
            memcpy(&ctx->tmpBlk[ctx->tmpBlkLen], data, dataLen);
            ctx->tmpBlkLen += dataLen;
            return B5_CMAC_AES256_RES_OK;
        }
        
        // Process the first block (merging tmpBlk and data) and adjust data pointer
        memcpy(&ctx->tmpBlk[ctx->tmpBlkLen], data, B5_AES_BLK_SIZE-ctx->tmpBlkLen);
        for (i=0; i < B5_AES_BLK_SIZE; i++)
            ctx->C[i] ^= ctx->tmpBlk[i];
                B5_Aes256_Update(&ctx->aesCtx, ctx->C, ctx->C, 1);   
        data += (B5_AES_BLK_SIZE-ctx->tmpBlkLen);
        dataLen -= (B5_AES_BLK_SIZE-ctx->tmpBlkLen);
        ctx->tmpBlkLen = 0;
    }
    
    
    
    // Other Blocks
    while (dataLen > B5_AES_BLK_SIZE) 
    {
        for (i=0; i < B5_AES_BLK_SIZE; i++)
            ctx->C[i] ^= data[i];   
                B5_Aes256_Update(&ctx->aesCtx, ctx->C, ctx->C, 1);   
        dataLen -= B5_AES_BLK_SIZE;
        data += B5_AES_BLK_SIZE;
    }
    
    
    memcpy(&ctx->tmpBlk[0], data, dataLen);
    ctx->tmpBlkLen = dataLen;
    
    
    return B5_CMAC_AES256_RES_OK;
}

int32_t B5_CmacAes256_Finit (B5_tCmacAesCtx *ctx, uint8_t *rSignature)
{
    int32_t    i;
    uint8_t    MN[B5_AES_BLK_SIZE];
    
    
    if(ctx == NULL)
        return B5_CMAC_AES256_RES_INVALID_CONTEXT;
    
    if(rSignature == NULL)
        return B5_CMAC_AES256_RES_INVALID_ARGUMENT;
    
    
    
    // Last Block
    if(ctx->tmpBlkLen == B5_AES_BLK_SIZE)
    {
        for (i=0; i < ctx->tmpBlkLen; i++)
            MN[i] = ctx->K1[i] ^ ctx->tmpBlk[i];
    }
    else
    {
        for (i=0; i < B5_AES_BLK_SIZE; i++)
            MN[i] = ctx->K2[i];	
        for (i=0; i < ctx->tmpBlkLen; i++)
            MN[i] ^= ctx->tmpBlk[i];
        MN[ctx->tmpBlkLen] ^= 0x80;
    }
    
    
    for (i=0; i<B5_AES_BLK_SIZE; i++)
        ctx->C[i] ^= MN[i];
    
    B5_Aes256_Update(&ctx->aesCtx, rSignature, ctx->C, 1);   

    
    return B5_CMAC_AES256_RES_OK;
}

int32_t B5_CmacAes256_Reset (B5_tCmacAesCtx *ctx)
{
    if(ctx == NULL)
        return B5_CMAC_AES256_RES_INVALID_CONTEXT;    
    
    memset(ctx->C, 0, sizeof(ctx->C));
    ctx->tmpBlkLen = 0;
    
    return B5_CMAC_AES256_RES_OK;
}
