/**
  ******************************************************************************
  * File Name          : pbkdf2.c
  * Description        : Low level PBKDF2 implementation.
  ******************************************************************************
  *
  * Copyright © 2016-present Blu5 Group <https://www.blu5group.com>
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

#include "pbkdf2.h"

/** out = x ^ y.
*  out, x and y may alias. */
static  void xor_bb(uint8_t *out, const uint8_t *x, const uint8_t *y, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++)
		out[i] = x[i] ^ y[i];
}

static void F(const B5_tHmacSha256Ctx *startctx,
	uint32_t counter,
	const uint8_t *salt, size_t nsalt,
	uint32_t iterations,
	uint8_t *out)
{
	uint8_t U[B5_SHA256_DIGEST_SIZE];
	B5_tHmacSha256Ctx ctx = *startctx;
	uint8_t countbuf[4];
	uint32_t i;
	countbuf[0] = ((counter >> 3 * 8) & 0xFF);
	countbuf[1] = ((counter >> 2 * 8) & 0xFF);
	countbuf[2] = ((counter >> 1 * 8) & 0xFF);
	countbuf[3] = (counter & 0xFF);

	/* First iteration:
	*   U_1 = PRF(P, S || INT_32_BE(i))
	*/

	B5_HmacSha256_Update(&ctx, salt, nsalt);
	B5_HmacSha256_Update(&ctx, countbuf, sizeof(countbuf));
	B5_HmacSha256_Finit(&ctx, U);
	memcpy(out, U, B5_SHA256_DIGEST_SIZE);

	/* Subsequent iterations:
	*   U_c = PRF(P, U_{c-1})
	*/
	for (i = 1; i < iterations; i++)
	{
		ctx = *startctx;
		B5_HmacSha256_Update(&ctx, U, B5_SHA256_DIGEST_SIZE);
		B5_HmacSha256_Finit(&ctx, U);
		xor_bb(out, out, U, B5_SHA256_DIGEST_SIZE);
	}
}

void PBKDF2HmacSha256(	const uint8_t *pw,
									size_t npw,
									const uint8_t *salt,
									size_t nsalt,
									uint32_t iterations,
									uint8_t *out,
									size_t nout)
{
	uint32_t counter = 1;
	uint8_t block[B5_SHA256_DIGEST_SIZE];
	size_t taken;

	/* Starting point for inner loop. */
	B5_tHmacSha256Ctx ctx;
	B5_HmacSha256_Init(&ctx, pw, (int16_t)npw);

	while(nout)
	{
		F(&ctx, counter, salt, nsalt, iterations, block);
		taken = (nout < B5_SHA256_DIGEST_SIZE)?(nout):(B5_SHA256_DIGEST_SIZE);
		memcpy(out, block, taken);
		out += taken;
		nout -= taken;
		counter++;
	}
}
