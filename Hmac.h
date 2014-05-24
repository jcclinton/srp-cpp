/**
 * mangos-zero is a full featured server for World of Warcraft in its vanilla
 * version, supporting clients for patch 1.12.x.
 *
 * Copyright (C) 2005-2014  MaNGOS project <http://getmangos.eu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * World of Warcraft, and all World of Warcraft or Warcraft art, images,
 * and lore are copyrighted by Blizzard Entertainment, Inc.
 */

#ifndef _AUTH_HMAC_H
#define _AUTH_HMAC_H

#include "Common.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>

#define SEED_KEY_SIZE 16

/**
 * @brief
 *
 */
class HmacHash
{
    public:
        /**
         * @brief
         *
         */
        HmacHash();
        /**
         * @brief
         *
         */
        ~HmacHash();
        /**
         * @brief
         *
         * @param bn
         */
        void UpdateBigNumber(BigNumber* bn);
        /**
         * @brief
         *
         * @param data
         * @param length
         */
        void UpdateData(const uint8* data, int length);
        /**
         * @brief
         *
         */
        void Initialize();
        /**
         * @brief
         *
         */
        void Finalize();
        /**
         * @brief
         *
         * @return uint8
         */
        uint8* GetDigest() { return m_digest; }
        /**
         * @brief
         *
         * @return int
         */
        int GetLength() { return SHA_DIGEST_LENGTH; }
    private:
        HMAC_CTX m_ctx; /**< TODO */
        uint8 m_key[SEED_KEY_SIZE]; /**< TODO */
        uint8 m_digest[SHA_DIGEST_LENGTH]; /**< TODO */
};
#endif
