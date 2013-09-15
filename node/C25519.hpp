/*
 * ZeroTier One - Global Peer to Peer Ethernet
 * Copyright (C) 2012-2013  ZeroTier Networks LLC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * --
 *
 * ZeroTier may be used and distributed under the terms of the GPLv3, which
 * are available at: http://www.gnu.org/licenses/gpl-3.0.html
 *
 * If you would like to embed ZeroTier into a commercial application or
 * redistribute it in a modified binary form, please contact ZeroTier Networks
 * LLC. Start here: http://www.zerotier.com/
 */

#ifndef _ZT_C25519_HPP
#define _ZT_C25519_HPP

#include "Array.hpp"

namespace ZeroTier {

#define ZT_C25519_PUBLIC_KEY_LEN 64
#define ZT_C25519_PRIVATE_KEY_LEN 64
#define ZT_C25519_SIGNATURE_LEN 96

/**
 * C25519 elliptic curve key agreement and signing
 */
class C25519
{
public:
	/**
	 * Public key (both crypto and signing)
	 */
	typedef Array<unsigned char,ZT_C25519_PUBLIC_KEY_LEN> Public; // crypto key, signing key (both 32 bytes)

	/**
	 * Private key (both crypto and signing)
	 */
	typedef Array<unsigned char,ZT_C25519_PRIVATE_KEY_LEN> Private; // crypto key, signing key (both 32 bytes)

	/**
	 * Message signature
	 */
	typedef Array<unsigned char,ZT_C25519_SIGNATURE_LEN> Signature;

	/**
	 * Public/private key pair
	 */
	typedef struct {
		Public pub;
		Private priv;
	} Pair;

	/**
	 * Generate a C25519 elliptic curve key pair
	 */
	static Pair generate()
		throw();

	/**
	 * Perform C25519 ECC key agreement
	 *
	 * Actual key bytes are generated from one or more SHA-512 digests of
	 * the raw result of key agreement.
	 *
	 * @param mine My key pair including secret
	 * @param their Their public key
	 * @param keybuf Buffer to fill
	 * @param keylen Number of key bytes to generate
	 */
	static void agree(const Pair &mine,const Public &their,void *keybuf,unsigned int keylen)
		throw();

	/**
	 * Sign a message with a sender's key pair
	 *
	 * This takes the SHA-521 of msg[] and then signs the first 32 bytes of this
	 * digest, returning it and the 64-byte ed25519 signature in signature[].
	 * This results in a signature that verifies both the signer's authenticity
	 * and the integrity of the message.
	 *
	 * This is based on the original ed25519 code from NaCl and the SUPERCOP
	 * cipher benchmark suite, but with the modification that it always
	 * produces a signature of fixed 96-byte length based on the hash of an
	 * arbitrary-length message.
	 *
	 * @param Key pair to sign with
	 * @param msg Message to sign
	 * @param len Length of message in bytes
	 * @param signature Buffer to fill with signature -- MUST be 96 bytes in length
	 */
	static void sign(const Pair &mine,const void *msg,unsigned int len,void *signature)
		throw();

	/**
	 * Sign a message with a sender's key pair
	 *
	 * @param Key pair to sign with
	 * @param msg Message to sign
	 * @param len Length of message in bytes
	 * @return Signature
	 */
	static Signature sign(const Pair &mine,const void *msg,unsigned int len)
		throw()
	{
		Signature sig;
		sign(mine,msg,len,sig.data);
		return sig;
	}

	/**
	 * Verify a message's signature
	 *
	 * @param their Public key to verify against
	 * @param msg Message to verify signature integrity against
	 * @param len Length of message in bytes
	 * @param signature 96-byte signature
	 * @return True if signature is valid and the message is authentic and unmodified
	 */
	static bool verify(const Public &their,const void *msg,unsigned int len,const void *signature)
		throw();

	/**
	 * Verify a message's signature
	 *
	 * @param their Public key to verify against
	 * @param msg Message to verify signature integrity against
	 * @param len Length of message in bytes
	 * @param signature 96-byte signature
	 * @return True if signature is valid and the message is authentic and unmodified
	 */
	static inline bool verify(const Public &their,const void *msg,unsigned int len,const Signature &signature)
		throw()
	{
		return verify(their,msg,len,signature.data);
	}
};

} // namespace ZeroTier

#endif