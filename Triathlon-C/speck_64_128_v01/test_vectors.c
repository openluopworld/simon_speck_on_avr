/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * FELICS - Fair Evaluation of Lightweight Cryptographic Systems
 *
 * Copyright (C) 2015 University of Luxembourg
 *
 * Written in 2015 by Yann Le Corre <yann.lecorre@uni.lu>
 *
 * This file is part of FELICS.
 *
 * FELICS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * FELICS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdint.h>

#include "test_vectors.h"

/*
 *
 * Speck Test vectors
 *
 */
const uint8_t expectedPlaintext[BLOCK_SIZE] =
{
    /* p = 0x3b726574, 0x7475432d */
    0x2d, 0x43, 0x75, 0x74,
    0x74, 0x65, 0x72, 0x3b
};

const uint8_t expectedKey[KEY_SIZE] =
{
    /* 0x1b, 0x1a, 0x19, 0x18, 0x13, 0x12, 0x11, 0x10, 0x0b, 0x0a, 0x09, 0x08, 0x03, 0x02, 0x01, 0x00; */
    0x00, 0x01, 0x02, 0x03,
    0x08, 0x09, 0x0a, 0x0b,
    0x10, 0x11, 0x12, 0x13,
    0x18, 0x19, 0x1a, 0x1b
};

const uint8_t expectedCiphertext[BLOCK_SIZE] =
{
    /* c = 0x8c6fa548, 0x454e028b */
    0x8b, 0x02, 0x4e, 0x45,
    0x48, 0xa5, 0x6f, 0x8c
};