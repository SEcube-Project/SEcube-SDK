/**
  ******************************************************************************
  * File Name          : sha256.c
  * Description        : This file includes the implementation of the functions
  *                      for computing the SHA256 and HMAC-SHA256 algorithms.
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

#include "sha256.h"

void B5_SHA256_GETUINT32(uint32_t *n,const uint8_t *b, int32_t i)
{
    *n =      ( (uint32_t) b[i    ] << 24 )
            | ( (uint32_t) b[i + 1] << 16 )
            | ( (uint32_t) b[i + 2] <<  8 )
            | ( (uint32_t) b[i + 3]       );
}

void B5_SHA256_PUTUINT32(uint32_t n,uint8_t *b, int32_t i)                       
{                                               
    b[i    ] = (uint8_t) ( n >> 24 );      
    b[i + 1] = (uint8_t) ( n >> 16 );      
    b[i + 2] = (uint8_t) ( n >>  8 );      
    b[i + 3] = (uint8_t) ( n       );
}


#define B5_SHA256_SHR(x,n) ((x & 0xFFFFFFFF) >> n)
#define B5_SHA256_ROTR(x,n) (B5_SHA256_SHR(x,n) | (x << (32 - n)))

#define B5_SHA256_S0(x) (B5_SHA256_ROTR(x, 7) ^ B5_SHA256_ROTR(x,18) ^  B5_SHA256_SHR(x, 3))
#define B5_SHA256_S1(x) (B5_SHA256_ROTR(x,17) ^ B5_SHA256_ROTR(x,19) ^  B5_SHA256_SHR(x,10))

#define B5_SHA256_S2(x) (B5_SHA256_ROTR(x, 2) ^ B5_SHA256_ROTR(x,13) ^ B5_SHA256_ROTR(x,22))
#define B5_SHA256_S3(x) (B5_SHA256_ROTR(x, 6) ^ B5_SHA256_ROTR(x,11) ^ B5_SHA256_ROTR(x,25))

#define B5_SHA256_F0(x,y,z) ((x & y) | (z & (x | y)))
#define B5_SHA256_F1(x,y,z) (z ^ (x & (y ^ z)))

#define B5_SHA256_R(t)                                            \
(                                                       \
    ctx->W[t] = B5_SHA256_S1(ctx->W[t -  2]) + ctx->W[t -  7] +   \
           B5_SHA256_S0(ctx->W[t - 15]) + ctx->W[t - 16]          \
)

void B5_SHA256_P(uint32_t a,uint32_t b,uint32_t c,uint32_t *d,uint32_t e,uint32_t f,uint32_t g,uint32_t *h,uint32_t x,uint32_t K)
{
uint32_t temp1, temp2;
    temp1 = *h + B5_SHA256_S3(e) + B5_SHA256_F1(e,f,g) + K + x;      
    temp2 = B5_SHA256_S2(a) + B5_SHA256_F0(a,b,c);                  
    *d += temp1; *h = temp1 + temp2;        
}

// Inner padding (ipad)
#define B5_HMAC_IPAD 0x36

// Outer padding (opad)
#define B5_HMAC_OPAD 0x5C

static void B5_Sha256ProcessBlock(B5_tSha256Ctx *ctx, const uint8_t *data)
{
    uint32_t A, B, C, D, E, F, G, H;

    B5_SHA256_GETUINT32( &ctx->W[0],  data,  0 );
    B5_SHA256_GETUINT32( &ctx->W[1],  data,  4 );
    B5_SHA256_GETUINT32( &ctx->W[2],  data,  8 );
    B5_SHA256_GETUINT32( &ctx->W[3],  data, 12 );
    B5_SHA256_GETUINT32( &ctx->W[4],  data, 16 );
    B5_SHA256_GETUINT32( &ctx->W[5],  data, 20 );
    B5_SHA256_GETUINT32( &ctx->W[6],  data, 24 );
    B5_SHA256_GETUINT32( &ctx->W[7],  data, 28 );
    B5_SHA256_GETUINT32( &ctx->W[8],  data, 32 );
    B5_SHA256_GETUINT32( &ctx->W[9],  data, 36 );
    B5_SHA256_GETUINT32( &ctx->W[10], data, 40 );
    B5_SHA256_GETUINT32( &ctx->W[11], data, 44 );
    B5_SHA256_GETUINT32( &ctx->W[12], data, 48 );
    B5_SHA256_GETUINT32( &ctx->W[13], data, 52 );
    B5_SHA256_GETUINT32( &ctx->W[14], data, 56 );
    B5_SHA256_GETUINT32( &ctx->W[15], data, 60 );


    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];
    F = ctx->state[5];
    G = ctx->state[6];
    H = ctx->state[7];


    B5_SHA256_P( A, B, C, &D, E, F, G, &H, ctx->W[ 0], 0x428A2F98 );
    B5_SHA256_P( H, A, B, &C, D, E, F, &G, ctx->W[ 1], 0x71374491 );
    B5_SHA256_P( G, H, A, &B, C, D, E, &F, ctx->W[ 2], 0xB5C0FBCF );
    B5_SHA256_P( F, G, H, &A, B, C, D, &E, ctx->W[ 3], 0xE9B5DBA5 );
    B5_SHA256_P( E, F, G, &H, A, B, C, &D, ctx->W[ 4], 0x3956C25B );
    B5_SHA256_P( D, E, F, &G, H, A, B, &C, ctx->W[ 5], 0x59F111F1 );
    B5_SHA256_P( C, D, E, &F, G, H, A, &B, ctx->W[ 6], 0x923F82A4 );
    B5_SHA256_P( B, C, D, &E, F, G, H, &A, ctx->W[ 7], 0xAB1C5ED5 );
    B5_SHA256_P( A, B, C, &D, E, F, G, &H, ctx->W[ 8], 0xD807AA98 );
    B5_SHA256_P( H, A, B, &C, D, E, F, &G, ctx->W[ 9], 0x12835B01 );
    B5_SHA256_P( G, H, A, &B, C, D, E, &F, ctx->W[10], 0x243185BE );
    B5_SHA256_P( F, G, H, &A, B, C, D, &E, ctx->W[11], 0x550C7DC3 );
    B5_SHA256_P( E, F, G, &H, A, B, C, &D, ctx->W[12], 0x72BE5D74 );
    B5_SHA256_P( D, E, F, &G, H, A, B, &C, ctx->W[13], 0x80DEB1FE );
    B5_SHA256_P( C, D, E, &F, G, H, A, &B, ctx->W[14], 0x9BDC06A7 );
    B5_SHA256_P( B, C, D, &E, F, G, H, &A, ctx->W[15], 0xC19BF174 );
    B5_SHA256_P( A, B, C, &D, E, F, G, &H, B5_SHA256_R(16), 0xE49B69C1 );
    B5_SHA256_P( H, A, B, &C, D, E, F, &G, B5_SHA256_R(17), 0xEFBE4786 );
    B5_SHA256_P( G, H, A, &B, C, D, E, &F, B5_SHA256_R(18), 0x0FC19DC6 );
    B5_SHA256_P( F, G, H, &A, B, C, D, &E, B5_SHA256_R(19), 0x240CA1CC );
    B5_SHA256_P( E, F, G, &H, A, B, C, &D, B5_SHA256_R(20), 0x2DE92C6F );
    B5_SHA256_P( D, E, F, &G, H, A, B, &C, B5_SHA256_R(21), 0x4A7484AA );
    B5_SHA256_P( C, D, E, &F, G, H, A, &B, B5_SHA256_R(22), 0x5CB0A9DC );
    B5_SHA256_P( B, C, D, &E, F, G, H, &A, B5_SHA256_R(23), 0x76F988DA );
    B5_SHA256_P( A, B, C, &D, E, F, G, &H, B5_SHA256_R(24), 0x983E5152 );
    B5_SHA256_P( H, A, B, &C, D, E, F, &G, B5_SHA256_R(25), 0xA831C66D );
    B5_SHA256_P( G, H, A, &B, C, D, E, &F, B5_SHA256_R(26), 0xB00327C8 );
    B5_SHA256_P( F, G, H, &A, B, C, D, &E, B5_SHA256_R(27), 0xBF597FC7 );
    B5_SHA256_P( E, F, G, &H, A, B, C, &D, B5_SHA256_R(28), 0xC6E00BF3 );
    B5_SHA256_P( D, E, F, &G, H, A, B, &C, B5_SHA256_R(29), 0xD5A79147 );
    B5_SHA256_P( C, D, E, &F, G, H, A, &B, B5_SHA256_R(30), 0x06CA6351 );
    B5_SHA256_P( B, C, D, &E, F, G, H, &A, B5_SHA256_R(31), 0x14292967 );
    B5_SHA256_P( A, B, C, &D, E, F, G, &H, B5_SHA256_R(32), 0x27B70A85 );
    B5_SHA256_P( H, A, B, &C, D, E, F, &G, B5_SHA256_R(33), 0x2E1B2138 );
    B5_SHA256_P( G, H, A, &B, C, D, E, &F, B5_SHA256_R(34), 0x4D2C6DFC );
    B5_SHA256_P( F, G, H, &A, B, C, D, &E, B5_SHA256_R(35), 0x53380D13 );
    B5_SHA256_P( E, F, G, &H, A, B, C, &D, B5_SHA256_R(36), 0x650A7354 );
    B5_SHA256_P( D, E, F, &G, H, A, B, &C, B5_SHA256_R(37), 0x766A0ABB );
    B5_SHA256_P( C, D, E, &F, G, H, A, &B, B5_SHA256_R(38), 0x81C2C92E );
    B5_SHA256_P( B, C, D, &E, F, G, H, &A, B5_SHA256_R(39), 0x92722C85 );
    B5_SHA256_P( A, B, C, &D, E, F, G, &H, B5_SHA256_R(40), 0xA2BFE8A1 );
    B5_SHA256_P( H, A, B, &C, D, E, F, &G, B5_SHA256_R(41), 0xA81A664B );
    B5_SHA256_P( G, H, A, &B, C, D, E, &F, B5_SHA256_R(42), 0xC24B8B70 );
    B5_SHA256_P( F, G, H, &A, B, C, D, &E, B5_SHA256_R(43), 0xC76C51A3 );
    B5_SHA256_P( E, F, G, &H, A, B, C, &D, B5_SHA256_R(44), 0xD192E819 );
    B5_SHA256_P( D, E, F, &G, H, A, B, &C, B5_SHA256_R(45), 0xD6990624 );
    B5_SHA256_P( C, D, E, &F, G, H, A, &B, B5_SHA256_R(46), 0xF40E3585 );
    B5_SHA256_P( B, C, D, &E, F, G, H, &A, B5_SHA256_R(47), 0x106AA070 );
    B5_SHA256_P( A, B, C, &D, E, F, G, &H, B5_SHA256_R(48), 0x19A4C116 );
    B5_SHA256_P( H, A, B, &C, D, E, F, &G, B5_SHA256_R(49), 0x1E376C08 );
    B5_SHA256_P( G, H, A, &B, C, D, E, &F, B5_SHA256_R(50), 0x2748774C );
    B5_SHA256_P( F, G, H, &A, B, C, D, &E, B5_SHA256_R(51), 0x34B0BCB5 );
    B5_SHA256_P( E, F, G, &H, A, B, C, &D, B5_SHA256_R(52), 0x391C0CB3 );
    B5_SHA256_P( D, E, F, &G, H, A, B, &C, B5_SHA256_R(53), 0x4ED8AA4A );
    B5_SHA256_P( C, D, E, &F, G, H, A, &B, B5_SHA256_R(54), 0x5B9CCA4F );
    B5_SHA256_P( B, C, D, &E, F, G, H, &A, B5_SHA256_R(55), 0x682E6FF3 );
    B5_SHA256_P( A, B, C, &D, E, F, G, &H, B5_SHA256_R(56), 0x748F82EE );
    B5_SHA256_P( H, A, B, &C, D, E, F, &G, B5_SHA256_R(57), 0x78A5636F );
    B5_SHA256_P( G, H, A, &B, C, D, E, &F, B5_SHA256_R(58), 0x84C87814 );
    B5_SHA256_P( F, G, H, &A, B, C, D, &E, B5_SHA256_R(59), 0x8CC70208 );
    B5_SHA256_P( E, F, G, &H, A, B, C, &D, B5_SHA256_R(60), 0x90BEFFFA );
    B5_SHA256_P( D, E, F, &G, H, A, B, &C, B5_SHA256_R(61), 0xA4506CEB );
    B5_SHA256_P( C, D, E, &F, G, H, A, &B, B5_SHA256_R(62), 0xBEF9A3F7 );
    B5_SHA256_P( B, C, D, &E, F, G, H, &A, B5_SHA256_R(63), 0xC67178F2 );

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
    ctx->state[4] += E;
    ctx->state[5] += F;
    ctx->state[6] += G;
    ctx->state[7] += H;
}

int32_t B5_Sha256_Init (B5_tSha256Ctx *ctx)
{
    
    if(ctx == NULL)
        return  B5_SHA256_RES_INVALID_CONTEXT;
    
    memset(ctx, 0, sizeof(B5_tSha256Ctx));
        
   
    // Set initial hash value
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x6A09E667;
    ctx->state[1] = 0xBB67AE85;
    ctx->state[2] = 0x3C6EF372;
    ctx->state[3] = 0xA54FF53A;
    ctx->state[4] = 0x510E527F;
    ctx->state[5] = 0x9B05688C;
    ctx->state[6] = 0x1F83D9AB;
    ctx->state[7] = 0x5BE0CD19;
    
   return B5_SHA256_RES_OK;
}

int32_t B5_Sha256_Update (B5_tSha256Ctx *ctx, const uint8_t* data, int32_t dataLen) 
{
    uint32_t left, fill;
    
    if(ctx == NULL)
        return  B5_SHA256_RES_INVALID_CONTEXT;
    
    if((data == NULL) || (dataLen < 0))
        return B5_SHA256_RES_INVALID_ARGUMENT;
        


    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += dataLen;
    ctx->total[0] &= 0xFFFFFFFF;

    if( ctx->total[0] < (uint32_t) dataLen )
        ctx->total[1]++;

    if( left && ((uint32_t) dataLen >= fill) )
    {
        memcpy( (void *) (ctx->buffer + left),
                (void *) data, fill );
        B5_Sha256ProcessBlock( ctx, ctx->buffer );
        dataLen -= fill;
        data  += fill;
        left = 0;
    }

    while( dataLen >= 64 )
    {
        B5_Sha256ProcessBlock( ctx, data );
        dataLen -= 64;
        data  += 64;
    }

    if( dataLen )
    {
        memcpy( (void *) (ctx->buffer + left),
                (void *) data, dataLen );
    }
    
    
   return B5_SHA256_RES_OK;
}

int32_t B5_Sha256_Finit (B5_tSha256Ctx *ctx, uint8_t* rDigest)
{
    uint8_t    sha2_padding[64];
    uint32_t   last, padn;
    uint32_t   high, low;
    uint8_t    msglen[8];
    
    
    if(ctx == NULL)
        return B5_SHA256_RES_INVALID_CONTEXT;
    
    if(rDigest == NULL)
        return B5_SHA256_RES_INVALID_ARGUMENT;
    
   

    memset(sha2_padding,0x00,64);
    sha2_padding[0] = 0x80;


    high = ( ctx->total[0] >> 29 )
         | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    B5_SHA256_PUTUINT32( high, msglen, 0 );
    B5_SHA256_PUTUINT32( low,  msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    B5_Sha256_Update(ctx, sha2_padding, padn );
    B5_Sha256_Update(ctx, msglen, 8 );
		

    B5_SHA256_PUTUINT32( ctx->state[0], rDigest,  0 );
    B5_SHA256_PUTUINT32( ctx->state[1], rDigest,  4 );
    B5_SHA256_PUTUINT32( ctx->state[2], rDigest,  8 );
    B5_SHA256_PUTUINT32( ctx->state[3], rDigest, 12 );
    B5_SHA256_PUTUINT32( ctx->state[4], rDigest, 16 );
    B5_SHA256_PUTUINT32( ctx->state[5], rDigest, 20 );
    B5_SHA256_PUTUINT32( ctx->state[6], rDigest, 24 );
    B5_SHA256_PUTUINT32( ctx->state[7], rDigest, 28 );
    return B5_SHA256_RES_OK;
}

int32_t B5_HmacSha256_Init (B5_tHmacSha256Ctx *ctx, const uint8_t *Key, int16_t keySize)
{
    int32_t   i;
    uint8_t    digest[B5_SHA256_DIGEST_SIZE];

    
    if(Key == NULL) 
        return B5_HMAC_SHA256_RES_INVALID_ARGUMENT;
    
    if(ctx == NULL)
        return  B5_HMAC_SHA256_RES_INVALID_CONTEXT;
    
    memset(ctx, 0, sizeof(B5_tHmacSha256Ctx));
    
    //The key is longer than the block size?
    if(keySize > B5_SHA256_BLOCK_SIZE)
    {
        // Initialize the hash function context
        B5_Sha256_Init(&ctx->shaCtx);
        // Digest the original key
        B5_Sha256_Update(&ctx->shaCtx, Key, keySize);
        // Finalize the message digest computation
        B5_Sha256_Finit(&ctx->shaCtx, digest);
        
        Key = digest;
        keySize = B5_SHA256_DIGEST_SIZE;
    }
 
    
    memset( ctx->iPad, B5_HMAC_IPAD, 64 );
    memset( ctx->oPad, B5_HMAC_OPAD, 64 );
    
    
    for( i = 0; i < keySize; i++ )
    {
        ctx->iPad[i] = (unsigned char)( ctx->iPad[i] ^ Key[i] );
        ctx->oPad[i] = (unsigned char)( ctx->oPad[i] ^ Key[i] );
    }
    
    
    // Initialize context for the first pass
		B5_Sha256_Init(&ctx->shaCtx);
    
    // Start with the inner pad
    B5_Sha256_Update(&ctx->shaCtx, ctx->iPad, B5_SHA256_BLOCK_SIZE);    
    
    return B5_HMAC_SHA256_RES_OK;
}

int32_t B5_HmacSha256_Update (B5_tHmacSha256Ctx *ctx, const uint8_t *data, int32_t dataLen)
{
    if(ctx == NULL){
        return  B5_HMAC_SHA256_RES_INVALID_CONTEXT;
    }
    
    
    if((data == NULL) || (dataLen < 0)){
        return B5_HMAC_SHA256_RES_INVALID_ARGUMENT;
    }
    
    
    if(dataLen == 0)
        return B5_HMAC_SHA256_RES_OK;
    
    
    // Digest the message (first pass)
    B5_Sha256_Update(&ctx->shaCtx, data, dataLen);
    
    return B5_HMAC_SHA256_RES_OK;
}

int32_t B5_HmacSha256_Finit (B5_tHmacSha256Ctx *ctx, uint8_t *rDigest)
{
    uint8_t    digest[B5_SHA256_DIGEST_SIZE];

    if(ctx == NULL)
        return  B5_HMAC_SHA256_RES_INVALID_CONTEXT;
    
    if(rDigest == NULL)
        return B5_HMAC_SHA256_RES_INVALID_ARGUMENT;
    
    
    // Finish the first pass
		B5_Sha256_Finit(&ctx->shaCtx, digest);    
    
    // Initialize context for the second pass
		B5_Sha256_Init(&ctx->shaCtx);
    // Start with outer pad
    B5_Sha256_Update(&ctx->shaCtx, ctx->oPad, B5_SHA256_BLOCK_SIZE);
    // Then digest the result of the first hash
    B5_Sha256_Update(&ctx->shaCtx, digest, B5_SHA256_DIGEST_SIZE);
    // Finish the second pass
    B5_Sha256_Finit(&ctx->shaCtx, rDigest);
    return B5_HMAC_SHA256_RES_OK;
}


