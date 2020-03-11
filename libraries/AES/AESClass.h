/*
  SAMR3 - AES
    Created on: 01.01.2020
    Author: Georgi Angelov
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA   
 */

#ifndef _AES_H_INCLUDED
#define _AES_H_INCLUDED

#include <Arduino.h>

/** AES processing mode. */
enum aes_encrypt_mode
{
    AES_DECRYPTION = 0, /**< Decryption of data will be performed */
    AES_ENCRYPTION,     /**< Encryption of data will be performed */
};

/** AES cryptographic key size. */
enum aes_key_size
{
    AES_KEY_SIZE_128 = 0, /**< AES key size is 128-bit */
    AES_KEY_SIZE_192,     /**< AES key size is 192-bit */
    AES_KEY_SIZE_256,     /**< AES key size is 256-bit */
};

/** AES start mode. */
enum aes_start_mode
{
    AES_MANUAL_START = 0, /**< Manual start mode */
    AES_AUTO_START,       /**< Auto start mode */
};

/** AES operation mode. */
enum aes_operation_mode
{
    AES_ECB_MODE = 0, /**< Electronic Codebook (ECB) */
    AES_CBC_MODE,     /**< Cipher Block Chaining (CBC) */
    AES_OFB_MODE,     /**< Output Feedback (OFB) */
    AES_CFB_MODE,     /**< Cipher Feedback (CFB) */
    AES_CTR_MODE,     /**< Counter (CTR) */
    AES_CCM_MODE,     /**< Counter (CCM) */
    AES_GCM_MODE,     /**< Galois Counter Mode (GCM) */
};

/** AES Cipher FeedBack (CFB) size. */
enum aes_cfb_size
{
    AES_CFB_SIZE_128 = 0, /**< Cipher feedback data size is 128-bit */
    AES_CFB_SIZE_64,      /**< Cipher feedback data size is 64-bit */
    AES_CFB_SIZE_32,      /**< Cipher feedback data size is 32-bit */
    AES_CFB_SIZE_16,      /**< Cipher feedback data size is 16-bit */
    AES_CFB_SIZE_8,       /**< Cipher feedback data size is 8-bit */
};

/** AES countermeasure type */
enum aes_countermeature_type
{
    AES_COUNTERMEASURE_TYPE_disabled = 0x0, /**< Countermeasure type all disabled */
    AES_COUNTERMEASURE_TYPE_1 = 0x01,       /**< Countermeasure1 enabled */
    AES_COUNTERMEASURE_TYPE_2 = 0x02,       /**< Countermeasure2 enabled */
    AES_COUNTERMEASURE_TYPE_3 = 0x04,       /**< Countermeasure3 enabled */
    AES_COUNTERMEASURE_TYPE_4 = 0x08,       /**< Countermeasure4 enabled */
    AES_COUNTERMEASURE_TYPE_ALL = 0x0F,     /**< Countermeasure type all enabled */
};

/* AES encryption complete. */
#define AES_ENCRYPTION_COMPLETE (1UL << 0)

/* AES GF multiplication complete. */
#define AES_GF_MULTI_COMPLETE (1UL << 1)

typedef struct aes_config_s
{
    /** AES data mode (decryption or encryption) */
    enum aes_encrypt_mode encrypt_mode;
    /** AES key size */
    enum aes_key_size key_size;
    /** Start mode */
    enum aes_start_mode start_mode;
    /** AES cipher operation mode*/
    enum aes_operation_mode opmode;
    /** Cipher feedback data size */
    enum aes_cfb_size cfb_size;
    /** Countermeasure type */
    enum aes_countermeature_type ctype;
    /** Enable XOR key */
    bool enable_xor_key;
    /** Enable key generation */
    bool enable_key_gen;
    /** Last output data mode enable/disable */
    bool lod;
} aes_config_t;

class AESClass
{
public:
    AESClass() { default_config(); }
    ~AESClass() { end(); }

    uint8_t *encode(void *vBlock, void *vKey)
    {
        if (NULL == vBlock || NULL == vKey)
            abort();
        uint8_t *block = (uint8_t *)vBlock;
        uint8_t *masterKey = (uint8_t *)vKey;
        /* Configure the AES. */
        cfg.encrypt_mode = AES_ENCRYPTION;
        cfg.key_size = AES_KEY_SIZE_128;
        cfg.start_mode = AES_AUTO_START;
        cfg.opmode = AES_ECB_MODE;
        cfg.cfb_size = AES_CFB_SIZE_128;
        cfg.lod = false;
        config(&cfg);
        int len = get_block_len(); // or abort()
        for (uint8_t i = 0; i < len; i++)
            block_data[i] = convert_byte_array_to_32_bit(masterKey + (i * (sizeof(uint32_t))));
        write_key(block_data);
        new_message(); /* The initialization vector is not used by the ECB cipher mode. */
        for (uint8_t i = 0; i < len; i++)
            block_data[i] = convert_byte_array_to_32_bit(block + (i * (sizeof(uint32_t))));
        write_data(block_data);
        clear_message();
        wait_ready(); /* Wait for the end of the encryption process. */
        read_data(block_data);
        memcpy(block, block_data, len * sizeof(uint32_t));
        return block;
    }

    uint8_t *decode(void *vBlock, void *vKey)
    {
        if (NULL == vBlock || NULL == vKey)
            abort();
        uint8_t *block = (uint8_t *)vBlock;
        uint8_t *key = (uint8_t *)vKey;
        /* Configure the AES. */
        cfg.encrypt_mode = AES_DECRYPTION;
        cfg.key_size = AES_KEY_SIZE_128;
        cfg.start_mode = AES_AUTO_START;
        cfg.opmode = AES_ECB_MODE;
        cfg.cfb_size = AES_CFB_SIZE_128;
        cfg.lod = false;
        config(&cfg);
        int len = get_block_len(); // or abort()
        for (uint8_t i = 0; i < len; i++)
            block_data[i] = convert_byte_array_to_32_bit(key + (i * (sizeof(uint32_t))));
        write_key(block_data);
        /* The initialization vector is not used by the ECB cipher mode. */
        for (uint8_t i = 0; i < len; i++)
            block_data[i] = convert_byte_array_to_32_bit(block + (i * (sizeof(uint32_t))));
        write_data(block_data);
        wait_ready(); /* Wait for the end of the decryption process. */
        read_data(block_data);
        memcpy(block, block_data, len * sizeof(uint32_t));
        return block;
    }

    void begin()
    {
        disable();
        /* Perform a software reset */
        AES->CTRLA.reg = AES_CTRLA_SWRST;
        /* Initialize the AES with new configurations */
        config();
        enable();
    }

    void config(aes_config_t *c = NULL)
    {
        if (NULL == c)
            default_config();
        else
            memcpy(&cfg, c, sizeof(aes_config_t));
        uint32_t ul_mode = (cfg.encrypt_mode << AES_CTRLA_CIPHER_Pos) |
                           (cfg.start_mode << AES_CTRLA_STARTMODE_Pos) |
                           (cfg.key_size << AES_CTRLA_KEYSIZE_Pos) |
                           (cfg.opmode << AES_CTRLA_AESMODE_Pos) |
                           (cfg.cfb_size << AES_CTRLA_CFBS_Pos) |
                           (AES_CTRLA_CTYPE(cfg.ctype)) |
                           (cfg.enable_xor_key << AES_CTRLA_XORKEY_Pos) |
                           (cfg.enable_key_gen << AES_CTRLA_KEYGEN_Pos) |
                           (cfg.lod << AES_CTRLA_LOD_Pos);
        if (AES->CTRLA.reg & AES_CTRLA_ENABLE)
        {
            disable();
            AES->CTRLA.reg = ul_mode; //mode
            enable();
        }
        else
        {
            AES->CTRLA.reg = ul_mode; //mode
        }
        //AES_DBG("A: %08X\n", AES->CTRLA.reg);
    }

    void end() { disable(); }

    inline void seed(uint32_t s) { AES->RANDSEED.reg = s; }

    uint32_t status()
    {
        uint32_t int_flags = AES->INTFLAG.reg;
        uint32_t status_flags = 0;
        if (int_flags & AES_INTFLAG_ENCCMP)
        {
            status_flags |= AES_ENCRYPTION_COMPLETE;
        }
        if (int_flags & AES_INTFLAG_GFMCMP)
        {
            status_flags |= AES_GF_MULTI_COMPLETE;
        }
        return status_flags;
    }

private:
    aes_config_t cfg;
    uint32_t block_data[8];

    inline void enable() { AES->CTRLA.reg |= AES_CTRLA_ENABLE; }

    void disable()
    {
        /* Disbale interrupt */
        AES->INTENCLR.reg = AES_INTENCLR_MASK;
        /* Clear interrupt flag */
        AES->INTFLAG.reg = AES_INTFLAG_MASK;
        AES->CTRLA.reg &= (~AES_CTRLA_ENABLE);
    }

    void default_config()
    {
        cfg.encrypt_mode = AES_ENCRYPTION;
        cfg.key_size = AES_KEY_SIZE_128;
        cfg.start_mode = AES_MANUAL_START;
        cfg.opmode = AES_ECB_MODE;
        cfg.cfb_size = AES_CFB_SIZE_128;
        cfg.ctype = AES_COUNTERMEASURE_TYPE_ALL;
        cfg.enable_xor_key = false;
        cfg.enable_key_gen = false;
        cfg.lod = false;
    }

    inline void wait_ready()
    {
        while (!(status() & AES_ENCRYPTION_COMPLETE))
        {
        }
    }

    inline void new_message() { AES->CTRLB.reg |= AES_CTRLB_NEWMSG; }

    inline void clear_message() { AES->CTRLB.reg &= ~AES_CTRLB_NEWMSG; }

    int get_block_len()
    {
        switch (cfg.key_size)
        {
        case AES_KEY_SIZE_128:
            return 4;
        case AES_KEY_SIZE_192:
            return 6;
        case AES_KEY_SIZE_256:
            return 8;
        default:
            abort();
            return -1;
        }
    }

    void write_key(const uint32_t *key)
    {
        if (key) /* Validate arguments. */
        {
            for (int i = 0; i < get_block_len(); i++)
                AES->KEYWORD[i].reg = *key++;
        }
    }

    void write_vector(const uint32_t *vector)
    {
        if (cfg.opmode != AES_ECB_MODE)
        {
            for (int i = 0; i < 4; i++)
                AES->INTVECTV[i].reg = *vector++;
        }
    }

    void write_data(const uint32_t *input_data_buffer)
    {
        if (input_data_buffer) /* Validate arguments. */
        {
            AES->DATABUFPTR.reg = 0;
            if (cfg.opmode == AES_CFB_MODE && cfg.cfb_size == AES_CFB_SIZE_64)
            {
                for (int i = 0; i < 2; i++)
                    AES->INDATA.reg = *input_data_buffer++;
            }
            else if (cfg.opmode == AES_CFB_MODE && (cfg.cfb_size == AES_CFB_SIZE_32 || cfg.cfb_size == AES_CFB_SIZE_16))
            {
                AES->INDATA.reg = *input_data_buffer;
            }
            else
            {
                for (int i = 0; i < 4; i++)
                    AES->INDATA.reg = *input_data_buffer++;
            }
        }
    }

    void read_data(uint32_t *output_data_buffer)
    {
        if (output_data_buffer) /* Validate arguments. */
        {
            AES->DATABUFPTR.reg = 0;
            if (cfg.opmode == AES_CFB_MODE && cfg.cfb_size == AES_CFB_SIZE_64)
            {
                for (int i = 0; i < 2; i++)
                    *output_data_buffer++ = AES->INDATA.reg;
            }
            else if (cfg.opmode == AES_CFB_MODE && (cfg.cfb_size == AES_CFB_SIZE_32 || cfg.cfb_size == AES_CFB_SIZE_16))
            {
                *output_data_buffer = AES->INDATA.reg;
            }
            else
            {
                for (int i = 0; i < 4; i++)
                    *output_data_buffer++ = AES->INDATA.reg;
            }
        }
    }
};

#endif